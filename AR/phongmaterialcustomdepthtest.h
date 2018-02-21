#ifndef PHONGMATERIALCUSTOMDEPTHTEST_H
#define PHONGMATERIALCUSTOMDEPTHTEST_H
#include <Qt3DExtras/QPhongMaterial>
#include <QDepthTest>


class PhongMaterialCustomDepthTest: public Qt3DExtras::QPhongMaterial

{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QDepthTest* depthTest READ depthTest WRITE setDepthTest NOTIFY depthTestChanged)
public:
    PhongMaterialCustomDepthTest(Qt3DCore::QNode *parent = nullptr);
    Qt3DRender::QDepthTest* depthTest(){return m_depthTest;}
    void setDepthTest(Qt3DRender::QDepthTest* depthTest);
signals:
    void depthTestChanged();
private:
    Qt3DRender::QDepthTest* m_depthTest;
};

#endif // PHONGMATERIALCUSTOMDEPTHTEST_H
