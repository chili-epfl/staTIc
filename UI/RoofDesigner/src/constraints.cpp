#include "constraints.h"
#include <iostream>
#include <QMap>
#include <QString>
#include <QQmlComponent>
#include <QVector2D>
#include <QObject>
#include <QList>
#include <QAbstractListModel>
#include <QDebug>


void *Constraints::CheckMalloc(size_t n) {
    void *r = malloc(n);
    if(!r) {
        printf("out of memory!\n");
        exit(-1);
    }
    return r;
}

int operator <(QVector2D v1, QVector2D v2){
    return v1.x() == v2.x() ? v1.y() < v2.y() : v1.x() < v2.x();
}

void Constraints::compute2d(QObject* sketch) {
    Slvs_hGroup g;
    pointIdsFromPosition.clear();
    linesIdsFromPointIds.clear();
    entityObjects.clear();
    int paramId = 1;
    const int entityOriginId = 1;
    const int entityQuaternionId = 2;
    const int entityPlanId = 3;
    int entityId = 100;
    int constId = 1;
    double qw, qx, qy, qz;

    g = 1;
    /* First, we create our workplane. Its origin corresponds to the origin
     * of our base frame (x y z) = (0 0 0) */
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, 0.0);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, 0.0);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, 0.0);
    sys.entity[sys.entities++] = Slvs_MakePoint3d(entityOriginId, g, paramId - 3, paramId - 2, paramId - 1);
    /* and it is parallel to the xy plane, so it has basis vectors (1 0 0)
     * and (0 1 0). */
    Slvs_MakeQuaternion(1, 0, 0,
                        0, 1, 0, &qw, &qx, &qy, &qz);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, qw);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, qx);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, qy);
    sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, qz);
    sys.entity[sys.entities++] = Slvs_MakeNormal3d(entityQuaternionId, g, paramId - 4, paramId - 3, paramId - 2, paramId - 1);

    sys.entity[sys.entities++] = Slvs_MakeWorkplane(entityPlanId, g, entityOriginId, entityQuaternionId);

    /* Now create a second group. We'll solve group 2, while leaving group 1
     * constant; so the workplane that we've created will be locked down,
     * and the solver can't move it. */
    g = 2;
    /* These points are represented by their coordinates (u v) within the
     * workplane, so they need only two parameters each. */
    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Point")
                && child->property("existing").toBool()) {
            pointIdsFromPosition.insert(QVector2D(child->property("x").toInt(), child->property("y").toInt()), entityId);
            sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, child->property("x").toInt());
            sys.param[sys.params++] = Slvs_MakeParam(paramId++, g, child->property("y").toInt());
            sys.entity[sys.entities++] = Slvs_MakePoint2d(entityId, g, entityPlanId, paramId - 2, paramId - 1);
            entityObjects.insert(entityId++, child);
        }
    }

    // Make line ids begin at the next thousand
    entityId += 1000 - entityId % 1000;

    /* And we create a line segment with those endpoints which are mapped
     * to their ids by their position in pointIdsFromPosition. The map needs
     * the operator< to be overwritten. */
    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Line")
                && child->property("existing").toBool()) {
            int p1Id = getPointId(child, "p1");
            int p2Id = getPointId(child, "p2");
            linesIdsFromPointIds.insert(QVector2D(p1Id, p2Id), entityId);
            sys.entity[sys.entities++] = Slvs_MakeLineSegment(entityId, g, entityPlanId, p1Id, p2Id);
            entityObjects.insert(entityId++, child);
        }
    }

    /* Now we iterate over all children again, in order to make specified
     * constraints.*/
    QList<int> constraintCodes({
                                   SLVS_C_HORIZONTAL,
                                   SLVS_C_VERTICAL,
                                   SLVS_C_PT_PT_DISTANCE,
                                   SLVS_C_EQUAL_LENGTH_LINES,
                                   SLVS_C_PT_PT_DISTANCE,
                                   SLVS_C_PARALLEL,
                                   SLVS_C_PERPENDICULAR,
                                   SLVS_C_ANGLE,
                                   SLVS_C_AT_MIDPOINT,
                                   SLVS_C_WHERE_DRAGGED});

    foreach (QObject* child, sketch->children()) {
        if (!QString::compare(child->property("class_type").toString(), "Constraint")) {
            if (child->property("existing").toBool() &&
                    child->property("type").isValid() &&
                    child->property("type").toInt() >= 0 &&
                    child->property("type").toInt() < constraintCodes.length()){

                int hPtA = -1, hPtB = -1, hEntityA = -1, hEntityB = -1;

                int type = constraintCodes[child->property("type").toInt()];
                double valA = child->property("valA").isValid() ? child->property("valA").toDouble() * sketch->property("scaleFactor").toDouble() : -1.0;

                QObject* ptA = qvariant_cast<QObject*>(child->property("ptA"));
                if (ptA && ptA->property("x").isValid() && ptA->property("y").isValid()) {
                    hPtA = pointIdsFromPosition[QVector2D(ptA->property("x").toInt(), ptA->property("y").toInt())];
                }

                QObject* ptB = qvariant_cast<QObject*>(child->property("ptB"));
                if (ptB && ptB->property("x").isValid() && ptB->property("y").isValid()) {
                    hPtB = pointIdsFromPosition[QVector2D(ptB->property("x").toInt(), ptB->property("y").toInt())];
                }

                QObject* entityA = qvariant_cast<QObject*>(child->property("entityA"));
                if (entityA && getPointId(entityA, "p1") && getPointId(entityA, "p2")) {
                    hEntityA = linesIdsFromPointIds[QVector2D(getPointId(entityA, "p1"), getPointId(entityA, "p2"))];
                }

                QObject* entityB = qvariant_cast<QObject*>(child->property("entityB"));
                if (entityB && getPointId(entityB, "p1") && getPointId(entityB, "p2"))  {
                    hEntityB = linesIdsFromPointIds[QVector2D(getPointId(entityB, "p1"), getPointId(entityB, "p2"))];
                }
                sys.constraint[sys.constraints++] =
                        Slvs_MakeConstraint(
                            constId, g, type, entityPlanId, valA,
                            hPtA, hPtB, hEntityA, hEntityB);
                constraintObjects.insert(constId++, child);
            }
        }
    }

    solve();
}

void Constraints::solve(){
    Slvs_hGroup g = 2;

    /* If the solver fails, then ask it to report which constraints caused
     * the problem. */
    sys.calculateFaileds = 1;

    /* And solve. */
    Slvs_Solve(&sys, g);

    if(sys.result == SLVS_RESULT_OKAY) {
        /* This loop prints points and lines parameters value to check if all
         * is correctly stored. */
        for (int i = 0; i < sys.entities; i++) {
            Slvs_Entity entity = sys.entity[i];
            /* sys.param[sys.entity[i].param[0] - 1]: it's needed to decrease the index
             * of sys.param since the param with handle 0 is reserved but not stored in the array.*/
            if (entity.type == SLVS_E_POINT_IN_2D){
                entityObjects[entity.h]->setProperty("x", sys.param[entity.param[0] - 1].val);
                entityObjects[entity.h]->setProperty("y", sys.param[entity.param[1] - 1].val);
                entityObjects[entity.h]->setProperty("conflicting", false);
            }
            if (entity.type == SLVS_E_LINE_SEGMENT) {
                entityObjects[entity.h]->setProperty("conflicting", false);
            }

        }
        for (int c = 0; c < sys.constraints; c++) {
            constraintObjects[sys.constraint[c].h]->setProperty("conflicting", false);
        }
    } else {
        std::cout << "solve failed: problematic constraints are:" << std::endl;
        for(int i = 0; i < sys.faileds; i++) {
            Slvs_Constraint failed = sys.constraint[sys.failed[i] - 1];
            std::cout << sys.failed[i] << ": (" << failed.type << ", "
                      << failed.valA << ", " << failed.ptA << ", "
                      << failed.ptB << ", " << failed.entityA << ", "
                      << failed.entityB << ")" << std::endl;
            constraintObjects[failed.h]->setProperty("conflicting", true);
            for (int e = 3; e < sys.entities; e++) {
                Slvs_Entity entity = sys.entity[e];
                if (entity.h == failed.ptA || entity.h == failed.ptB ||
                        entity.h == failed.entityA || entity.h == failed.entityB) {
                    entityObjects[entity.h]->setProperty("conflicting", true);
                }
            }
        }
        if(sys.result != 0) {
            std::cout << ("system inconsistent\n") << std::endl;
        } else {
            std::cout << ("system nonconvergent\n") << std::endl;
        }
    }
}

int Constraints::getPointId(QObject* line, const char* pointPropertyName) const {
    QObject* p = qvariant_cast<QObject*>(line->property(pointPropertyName));
    if (p->property("x").isNull() || p->property("y").isNull()){
        return 0;
    }
    int px = p->property("x").toInt();
    int py = p->property("y").toInt();
    return pointIdsFromPosition[QVector2D(px, py)];
}

void Constraints::apply(QObject* sketch)
{
    int estimated_memory_need=sketch->children().length()*3;
    if(m_allocated_memory<estimated_memory_need){
        m_allocated_memory=estimated_memory_need*3;
        sys.param      = (Slvs_Param*)CheckMalloc(m_allocated_memory*sizeof(sys.param[0]));
        sys.entity     = (Slvs_Entity*)CheckMalloc(m_allocated_memory*sizeof(sys.entity[0]));
        sys.constraint = (Slvs_Constraint*)CheckMalloc(m_allocated_memory*sizeof(sys.constraint[0]));
        sys.failed  = (Slvs_hConstraint*)CheckMalloc(m_allocated_memory*sizeof(sys.failed[0]));
    }

    sys.faileds = m_allocated_memory;
    sys.dragged[0]=sys.dragged[1]=sys.dragged[2]=sys.dragged[3]=0;
    sys.params = sys.constraints = sys.entities = 0;

    if (sketch) {
        compute2d(sketch);
    } else {
        qDebug() << "Null sketch";
    }
}

Constraints::Constraints(QObject *parent):
    QObject(parent)
{
    m_allocated_memory=0;
}

Constraints::~Constraints()
{
    if(m_allocated_memory>0){
        free(sys.param);
        free(sys.entity);
        free(sys.constraint);
        free(sys.failed);
    }
}
