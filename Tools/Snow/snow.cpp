#include "snow.h"
#include "statics/frame3dd/frame3ddkernel.h"
#include "statics/elements/beam.h"
#include "statics/elements/joint.h"
#include <QImage>
Snow::Snow(QObject *parent) : QObject(parent)
{
    m_vm_manager=Q_NULLPTR;
}

void Snow::findExposedBeams()
{
    if(!m_vm_manager) return;
    const int imageResolution=500;
    Frame3DDKernel* m_static_module=(Frame3DDKernel*) m_vm_manager->staticsModule();
    qreal max_extent=0;
    float max_height=-9999999999;
    Q_FOREACH(JointPtr joint, m_static_module->joints()){
        max_extent=qMax(fabs(joint->scaledPosition().x()),max_extent);
        max_extent=qMax(fabs(joint->scaledPosition().z()),max_extent);
        max_height=qMax(joint->scaledPosition().y(),max_height);

    }
    int scaleFactor=floor(2*max_extent/imageResolution)+1;
    QImage depthBuffer(imageResolution,imageResolution,QImage::Format_RGB888);
    depthBuffer.fill(QColor(255,255,255));
    WeakJointPtr e1,e2;
    QVector2D proj_1,proj_2,pixel_coordinate;
    qreal depth,alpha=0.0;
    int i=0;
    Q_FOREACH(BeamPtr beam, m_static_module->beams()){
        beam->extremes(e1,e2);
        proj_1.setX(e1.toStrongRef()->scaledPosition().x()/scaleFactor+imageResolution/2);
        proj_1.setY(e1.toStrongRef()->scaledPosition().z()/scaleFactor+imageResolution/2);
        proj_2.setX(e2.toStrongRef()->scaledPosition().x()/scaleFactor+imageResolution/2);
        proj_2.setY(e2.toStrongRef()->scaledPosition().z()/scaleFactor+imageResolution/2);
        alpha=0.0;
        while(alpha<=1.0){
            pixel_coordinate=(1.0-alpha)*proj_1+alpha*proj_2;
            depth=max_height-(1.0-alpha)*e1.toStrongRef()->scaledPosition().y()-alpha*e2.toStrongRef()->scaledPosition().y();
            QColor current_color=depthBuffer.pixel(pixel_coordinate.x(),pixel_coordinate.y());
            if( (current_color.red()+current_color.green()) > depth){
                int green= depth/255;
                int red=depth-green*255;
                depthBuffer.setPixelColor((int)pixel_coordinate.x(),(int)pixel_coordinate.y(),QColor(red,green,i));
            }
            alpha+=0.1;
        }
        i++;
        if(i>254) break;
    }
    for(int i=0;i<imageResolution;i++)
        for(int j=0;j<imageResolution;j++){
            QColor c=depthBuffer.pixel(i,j);
            if(c.blue()<255)
                qDebug()<<m_static_module->beams().at(c.blue())->objectName();
        }
}

void Snow::setVMManager(AbstractVMManager * vm_manager)
{
    m_vm_manager=vm_manager;
}


