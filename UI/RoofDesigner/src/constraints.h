#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include <array>
#include <stdlib.h>
#include <QList>
#include <QHash>
#include <QObject>
#include <QVector2D>
#include <solvespace/include/slvs.h>

class Constraints : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void apply(QObject* sketch);
    Constraints(QObject* parent=Q_NULLPTR);
    ~Constraints();

private:
    void solve();
    void *CheckMalloc(size_t n);
    void compute2d(QObject* sketch);
    int getPointId(QObject* line, const char* pointPropertyName) const;

    int m_allocated_memory;
    Slvs_System sys;
    QHash<int, QObject*> ids2entityObjects;
    QHash<QObject*,int> entityObjects2ids;

    QHash<int, QObject*> constraintObjects;
};

#endif // CONSTRAINTS_H
