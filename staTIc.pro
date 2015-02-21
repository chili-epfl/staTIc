TEMPLATE = app

QT += qml quick widgets
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
