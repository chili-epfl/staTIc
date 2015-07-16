#include "controller.h"
#include <ChilitagsObject.h>

Controller::Controller(QObject* parent):
    QObject(parent)
{

}

void Controller::loadStructure( QString staticsFile){
    /*TODO: Check the nature of the shape 2d-3d*/
    staticsModule=new TwoDimensionalStaticsModule(staticsFile);
}


void Controller::loadStructure( QUrl staticsFile){
        loadStructure(staticsFile.toLocalFile());
}
