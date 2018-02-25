#ifndef DEFORMINGBEAMMESH_H
#define DEFORMINGBEAMMESH_H
#include <Qt3DCore>
#include <Qt3DRender>
#include <QVector4D>

/*
    The class defined the mesh used to display deformations of beams.
*/


class DeformingBeamMesh: public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int segments READ segments WRITE setSegments NOTIFY segmentsChanged)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int keyframes READ keyframes WRITE setKeyframes NOTIFY keyframesChanged)
    Q_PROPERTY(QVariant displacements READ displacements WRITE setDisplacements NOTIFY displacementsChanged)
    Q_PROPERTY(qreal exagerate READ exagerate WRITE setExagerate NOTIFY exagerateChanged)
    Q_PROPERTY(qreal displacements_step READ displacements_step WRITE setDisplacements_step NOTIFY displacements_stepChanged)
    Q_PROPERTY(QVector2D offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    DeformingBeamMesh(Qt3DCore::QNode * parent = 0);
    ~DeformingBeamMesh();
    void setLength(qreal val);
    void setSegments(int val);
    void setSize(QSizeF size);
    void setKeyframes(int n_frames);
    void setDisplacements(QVariant displacements);
    void setExagerate(qreal val);
    void setDisplacements_step(qreal val);
    void setOffset(QVector2D offset);


    qreal length(){return m_length;}
    int segments(){return m_segments;}
    QSizeF size(){return m_size;}
    int keyframes(){return m_keyFrames;}
    QVariant displacements(){return QVariant::fromValue(m_displacements);}
    qreal exagerate(){return m_exagerate;}
    qreal displacements_step(){return m_displacements_step;}
    QVector2D offset(){return m_offset;}
signals:
    void lengthChanged();
    void segmentsChanged();
    void sizeChanged();
    void keyframesChanged();
    void displacementsChanged();
    void exagerateChanged();
    void displacements_stepChanged();
    void offsetChanged();

private:
    void init();
    void update();
private slots:
    void generateGeometry();

private:
    bool m_init;
    int m_segments;
    qreal m_length;
    QSizeF m_size;
    int m_keyFrames;
    qreal m_exagerate;
    qreal m_displacements_step;
    QVector2D m_offset;
    QList<QVector4D> m_displacements;

    Qt3DRender::QGeometry *customGeometry;

    Qt3DRender::QBuffer *vertexDataBuffer;
    Qt3DRender::QBuffer *normalDataBuffer ;
    Qt3DRender::QBuffer *indexDataBuffer;
    Qt3DRender::QBuffer *uvtextureDataBuffer;

    QByteArray vertexBufferData;
    QByteArray normalBufferData;
    QByteArray indexBufferData;
    QByteArray uvtextureBufferData;


    Qt3DRender::QAttribute *positionAttribute;
    Qt3DRender::QAttribute *normalAttribute;
    Qt3DRender::QAttribute *indexAttribute;
    Qt3DRender::QAttribute *uvtextureAttribute;

};

#endif // DEFORMINGBEAMMESH_H
