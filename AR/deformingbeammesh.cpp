#include "deformingbeammesh.h"

DeformingBeamMesh::DeformingBeamMesh(Qt3DCore::QNode * parent):
    QGeometryRenderer(parent),
    m_init(false),
    m_segments(8),
    m_length(20),
    m_size(10,10),
    m_keyFrames(20),
    m_exagerate(1),
    customGeometry(0),
    vertexDataBuffer(0),
    normalDataBuffer(0),
    indexDataBuffer(0),
    positionAttribute(0),
    normalAttribute(0),
    indexAttribute(0)
{
    m_displacements.append(QVector4D(0,0,0,0));
    m_displacements.append(QVector4D(0,0,2,0));
    m_displacements.append(QVector4D(0,0,2,0));
    m_displacements.append(QVector4D(0,0,0,0));
}

void DeformingBeamMesh::setLength(qreal val)
{
    if(val!=m_length && val >0){
        m_length=val;
        update();
        emit lengthChanged();
    }
}

void DeformingBeamMesh::setSegments(int val)
{
    if(val!=m_segments && val>1){
        m_segments=val;
        update();
        emit segmentsChanged();
    }
}

void DeformingBeamMesh::setSize(QSizeF size)
{
    if(size!=m_size && !size.isEmpty()){
        m_size=size;
        update();
        emit sizeChanged();
    }
}

void DeformingBeamMesh::setKeyframes(int n_frames)
{
    if(n_frames >=2 && n_frames!=m_keyFrames ){
        m_keyFrames=n_frames;
        update();
        emit keyframesChanged();
    }
}

void DeformingBeamMesh::setDisplacements(QList<QVector4D> displacements)
{
    if(displacements.size()>2){
        m_displacements=displacements;
        update();
        emit displacementsChanged();
    }
}

void DeformingBeamMesh::setExagerate(int val)
{
    if(val>0 && val!=m_exagerate){
        m_exagerate=val;
        update();
        emit exagerateChanged();
    }
}

void DeformingBeamMesh::init(){
    if(m_init) return;

    customGeometry = new Qt3DRender::QGeometry(this);

    vertexDataBuffer =
            new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);

    normalDataBuffer=
            new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);

    indexDataBuffer =
            new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

    // Attributes
    positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(4);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(4 * sizeof(float));
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(normalDataBuffer);
    normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(3 * sizeof(float));
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);


    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(normalAttribute);
    customGeometry->addAttribute(indexAttribute);

    this->setInstanceCount(1);
    this->setBaseVertex(0);
    this->setBaseInstance(0);
    this->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);

    m_init=true;
}

void DeformingBeamMesh::update(){
    if(m_displacements.size()<2) return;
    if(!m_init) init();
    generateGeometry();
    this->setGeometry(customGeometry);
}

void DeformingBeamMesh::generateGeometry(){

    QByteArray vertexBufferData;
    // 8+4*(m_segments-1) distinct vertices X keyframes
    vertexBufferData.resize((8+4*(m_segments-1)) * (m_keyFrames) * (4) * sizeof(float));
    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());

    QByteArray normalBufferData;
    // 8+4*(m_segments-1) distinct vertices X keyframes
    normalBufferData.resize((8+4*(m_segments-1)) * (m_keyFrames) * (3) * sizeof(float));
    float *rawNormalArray = reinterpret_cast<float *>(normalBufferData.data());

    QByteArray indexBufferData;
    // 4 faces per segment, each split in 2, + 4 tringles on the externals, X keyframes
    indexBufferData.resize( (8 * m_segments + 12 )*  (m_keyFrames) * 3 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    float step=m_length/m_segments;
    qreal half_h=m_size.height()/2;
    qreal half_w=m_size.width()/2;

    QVector<QVector3D> vertices = QVector<QVector3D>();
    QVector<QVector3D> normals = QVector<QVector3D>();
    QVector<ushort> indices=QVector<ushort>();

    // Generate the initial vertices,  just a straight beam...
    QVector<QVector3D> _initial_vertices;
    //Generate vertices for two consecutive faces. Remember CCW
        /*       1------2
        *        |      |
        *        |  x0  |
        *        4------3
        *           \  1'------2'
        *            v |       |
        *              |  x1   |
        *              4'------3'
        */

    QVector3D v1(0, half_h, half_w);
    QVector3D v2(0, half_h, -half_w);
    QVector3D v3(0, -half_h, -half_w);
    QVector3D v4(0, -half_h, half_w);

    QVector3D dx(step,0,0);

    QVector3D v1p,v2p,v3p,v4p;

    for(int i=0; i<m_segments;i++){
        v1p=v1+dx;
        v2p=v2+dx;
        v3p=v3+dx;
        v4p=v4+dx;

        _initial_vertices.push_back(v1);
        _initial_vertices.push_back(v2);
        _initial_vertices.push_back(v3);
        _initial_vertices.push_back(v4);

        v1=v1p;
        v2=v2p;
        v3=v3p;
        v4=v4p;
    }

    _initial_vertices.push_back(v1);
    _initial_vertices.push_back(v2);
    _initial_vertices.push_back(v3);
    _initial_vertices.push_back(v4);

    // Final vertices

    QVector<QVector3D> _final_vertices;
    QVector4D disp;
    int quads_counter=0;

    Q_FOREACH(const QVector3D& v, _initial_vertices){
        double disp_index=(double)(m_displacements.size()-1)*(quads_counter/4)/(m_segments);
        int disp_index_floor=floor(disp_index);
        double dummy;
        double fract=modf(disp_index,&dummy);

        if(fract < 0.01){
            disp=m_displacements[disp_index_floor];
        }
        else{
            disp=(1-fract)*m_displacements[disp_index_floor]+(fract)*m_displacements[disp_index_floor+1];
        }


        //From opengl to Frame3DD
        QVector2D _v(-v.z(),v.y());
        float cos_theta=cos(disp.w());
        float sin_theta=sin(disp.w());
        //Clock wise rotation because of frame3dd
        QVector3D v_rot(v.x(), cos_theta*_v.x()+sin_theta*_v.y(),-sin_theta*_v.x()+cos_theta*_v.y());

//        /*3D case to be tested*/
//        QVector3D v_final=v_rot+m_exagerate*disp.toVector3D();
//        //Back to Opengl
//        _final_vertices.push_back(QVector3D(v_final.x(),v_final.z(),-v_final.y()));

        /*2D case:y is vertical displacement*/
        QVector3D v_final(v_rot.x(),v_rot.z(),-v_rot.y());
        v_final+=m_exagerate*disp.toVector3D();
        //Back to Opengl
        _final_vertices.push_back(v_final);

        quads_counter++;
    }

    //Generate middle KeyFrames vertices
    QVector<QVector3D> _middle_vertices;

    for(int keyframe=1; keyframe<(m_keyFrames-1); keyframe++){
        float ratio=(float)keyframe/(m_keyFrames-1);

        for(int vertexId=0;vertexId<_initial_vertices.size();vertexId++){

            _middle_vertices.push_back((1-ratio)*_initial_vertices[vertexId]
                                       +
                                       ratio*_final_vertices[vertexId]);
        }

    }

    //Putting things together

    Q_FOREACH(const QVector3D& v, _initial_vertices){
        vertices.push_back(v);
    }

    Q_FOREACH(const QVector3D& v, _middle_vertices){
        vertices.push_back(v);
    }
    Q_FOREACH(const QVector3D& v, _final_vertices){
        vertices.push_back(v);
    }

    //Generate indices. They are always the same for any keyframe
    for(int keyFrame=0; keyFrame<m_keyFrames;keyFrame++){
        //Extreme 1
        indices.push_back(3+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(0+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(2+keyFrame*(8+4*(m_segments-1)));

        indices.push_back(2+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(0+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(1+keyFrame*(8+4*(m_segments-1)));

        for(int i=0; i<m_segments;i++){
            // Top 1
            indices.push_back(0+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(5+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(1+4*i+keyFrame*(8+4*(m_segments-1)));
            // Top 2
            indices.push_back(0+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(4+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(5+4*i+keyFrame*(8+4*(m_segments-1)));
            // Front 1
            indices.push_back(0+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(7+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(4+4*i+keyFrame*(8+4*(m_segments-1)));
            // Front 2
            indices.push_back(0+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(3+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(7+4*i+keyFrame*(8+4*(m_segments-1)));
            // Back 1
            indices.push_back(1+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(5+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(6+4*i+keyFrame*(8+4*(m_segments-1)));
            // Back 2
            indices.push_back(1+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(6+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(2+4*i+keyFrame*(8+4*(m_segments-1)));
            // Bottom 1
            indices.push_back(2+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(6+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(7+4*i+keyFrame*(8+4*(m_segments-1)));
            // Bottom 2
            indices.push_back(2+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(7+4*i+keyFrame*(8+4*(m_segments-1)));
            indices.push_back(3+4*i+keyFrame*(8+4*(m_segments-1)));
        }
        //Extreme 2
        indices.push_back(4+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(7+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(6+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));

        indices.push_back(4+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(6+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));
        indices.push_back(5+4*(m_segments-1)+keyFrame*(8+4*(m_segments-1)));
    }

    //Compute Normals
    int offset=(8+4*(m_segments-1));
    for(int keyFrame=0; keyFrame<m_keyFrames;keyFrame++){

        QVector3D n051_prev,n045_prev,n074_prev,
                n037_prev,n156_prev,n162_prev,
                n267_prev,n273_prev;

        QVector3D n051,n045,n074,
                n037,n156,n162,
                n267,n273;

        QVector3D v1=vertices[0+offset*keyFrame];
        QVector3D v2=vertices[1+offset*keyFrame];
        QVector3D v3=vertices[2+offset*keyFrame];
        QVector3D v4=vertices[3+offset*keyFrame];

        for(int i=0; i<m_segments;i++){

            QVector3D v1p=vertices[4+4*i+offset*keyFrame];
            QVector3D v2p=vertices[5+4*i+offset*keyFrame];
            QVector3D v3p=vertices[6+4*i+offset*keyFrame];
            QVector3D v4p=vertices[7+4*i+offset*keyFrame];

            n051=QVector3D::normal(v1,v2p,v2);
            n045=QVector3D::normal(v1,v1p,v2p);
            n074=QVector3D::normal(v1,v4p,v1p);
            n037=QVector3D::normal(v1,v4,v4p);
            n156=QVector3D::normal(v2,v2p,v3p);
            n162=QVector3D::normal(v2,v3p,v3);
            n267=QVector3D::normal(v3,v3p,v4p);
            n273=QVector3D::normal(v3,v4p,v4);
            //When doing the average with the prev, remeber that 0 is 4 in prev, 1 is 5 prev and so on
            if(i==0){
                //Special case for the extreme
                normals.push_back(QVector3D(n051+n045+n074+n037+
                                            n045_prev+n074_prev+QVector3D(-1,0,0)).normalized());
                normals.push_back(QVector3D(n051+n156+n162+
                                            n051_prev+n045_prev+n156_prev+QVector3D(-1,0,0)).normalized());
                normals.push_back(QVector3D(n162+n267+n273+
                                            n162_prev+n156_prev+n267_prev+QVector3D(-1,0,0)).normalized());
                normals.push_back(QVector3D(n037+n273+n273+
                                            n074_prev+n037_prev+n267_prev+n273_prev+QVector3D(-1,0,0)).normalized());
            }
            else{
                normals.push_back(QVector3D(n051+n045+n074+n037+
                                            n045_prev+n074_prev).normalized());
                normals.push_back(QVector3D(n051+n156+n162+
                                            n051_prev+n045_prev+n156_prev).normalized());
                normals.push_back(QVector3D(n162+n267+n273+
                                            n162_prev+n156_prev+n267_prev).normalized());
                normals.push_back(QVector3D(n037+n273+n273+
                                            n074_prev+n037_prev+n267_prev+n273_prev).normalized());

            }

            n051_prev=n051;
            n045_prev=n045;
            n074_prev=n074;
            n037_prev=n037;
            n156_prev=n156;
            n162_prev=n162;
            n267_prev=n267;
            n273_prev=n273;

            v1=v1p;
            v2=v2p;
            v3=v3p;
            v4=v4p;
        }

        normals.push_back(QVector3D(n045_prev+n074_prev+QVector3D(1,0,0)).normalized());
        normals.push_back(QVector3D(n051_prev+n045_prev+n156_prev+QVector3D(1,0,0)).normalized());
        normals.push_back(QVector3D(n162_prev+n156_prev+n267_prev+QVector3D(1,0,0)).normalized());
        normals.push_back(QVector3D(n074_prev+n037_prev+n267_prev+n273_prev+QVector3D(1,0,0)).normalized());

    }




    int idx = 0 ;
    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx] = v.z();
        idx++;
        rawVertexArray[idx] = idx/((8+4*(m_segments-1))*4);
        idx++;
    }

    idx = 0;
    Q_FOREACH (const ushort &index, indices) {
        rawIndexArray[idx++] = index;
    }

    idx = 0;
    Q_FOREACH (const QVector3D &v, normals) {
        rawNormalArray[idx++] = v.x();
        rawNormalArray[idx++] = v.y();
        rawNormalArray[idx++] = v.z();
    }

    vertexDataBuffer->setData(vertexBufferData);
    normalDataBuffer->setData(normalBufferData);
    indexDataBuffer->setData(indexBufferData);

    positionAttribute->setCount(vertices.size());
    normalAttribute->setCount(normals.size());
    indexAttribute->setCount(indices.size());

    //4 faces, 2 trigles per face, 3 indices per triangle
    this->setPrimitiveCount(indices.size());

}


