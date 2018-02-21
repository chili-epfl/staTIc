#include "phongmaterialcustomdepthtest.h"
#include <QEffect>
#include <QTechnique>
#include <QDepthTest>


PhongMaterialCustomDepthTest::PhongMaterialCustomDepthTest(Qt3DCore::QNode *parent):
    Qt3DExtras::QPhongMaterial(parent),
    m_depthTest(Q_NULLPTR)
{

}

void PhongMaterialCustomDepthTest::setDepthTest(Qt3DRender::QDepthTest *depthTest)
{
    if(m_depthTest!=depthTest && m_depthTest!=Q_NULLPTR){
        m_depthTest=depthTest;
        auto effect=this->effect();
        if(effect){
             for(Qt3DRender::QTechnique* technique : effect->techniques()){
                 for(Qt3DRender::QRenderPass* renderPass: technique->renderPasses()){
                   renderPass->addRenderState(m_depthTest);
                 }
             }
        }
        emit depthTestChanged();
    }
}
