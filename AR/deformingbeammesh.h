#ifndef DEFORMINGBEAMMESH_H
#define DEFORMINGBEAMMESH_H
#include <Qt3DCore>
#include <Qt3DRender>
#include <QVector4D>

class DeformingBeamMesh: public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int segments READ segments WRITE setSegments NOTIFY segmentsChanged)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int keyframes READ keyframes WRITE setKeyframes NOTIFY keyframesChanged)
    Q_PROPERTY(QList<QVector4D> displacements READ displacements WRITE setDisplacements NOTIFY displacementsChanged)
    Q_PROPERTY(int exagerate READ exagerate WRITE setExagerate NOTIFY exagerateChanged)
public:
    DeformingBeamMesh(Qt3DCore::QNode * parent = 0);

    void setLength(qreal val);
    void setSegments(int val);
    void setSize(QSizeF size);
    void setKeyframes(int n_frames);
    void setDisplacements(QList<QVector4D> displacements);
    void setExagerate(int val);

    qreal length(){return m_length;}
    int segments(){return m_segments;}
    QSizeF size(){return m_size;}
    int keyframes(){return m_keyFrames;}
    QList<QVector4D> displacements(){return m_displacements;}
    int exagerate(){return m_exagerate;}

signals:
    void lengthChanged();
    void segmentsChanged();
    void sizeChanged();
    void keyframesChanged();
    void displacementsChanged();
    void exagerateChanged();
private:
    void init();
    void generateGeometry();
    void update();

private:
    bool m_init;
    int m_segments;
    qreal m_length;
    QSizeF m_size;
    int m_keyFrames;
    int m_exagerate;

    QList<QVector4D> m_displacements;

    Qt3DRender::QGeometry *customGeometry;

    Qt3DRender::QBuffer *vertexDataBuffer;
    Qt3DRender::QBuffer *normalDataBuffer ;
    Qt3DRender::QBuffer *indexDataBuffer;

    Qt3DRender::QAttribute *positionAttribute;
    Qt3DRender::QAttribute *normalAttribute;
    Qt3DRender::QAttribute *indexAttribute;


};

#endif // DEFORMINGBEAMMESH_H
