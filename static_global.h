#ifndef STATIC_GLOBAL_H
#define STATIC_GLOBAL_H
#include <QString>

#if ANDROID
const QString extstr=QString(getenv("EXTERNAL_STORAGE"))+"/StaticAR/";
const QString materialsPath(extstr+"Resources/Materials/");
const QString assets3DPath(extstr+"Resources/Assets3D/");
const QString scenariosPath(extstr+"Resources/Scenarios/");
const QString boardsPath(extstr+"Resources/Boards/");

const QString logFilesPath(extstr+"Resources/Private/log_files/");

#else
const QString materialsPath("Resources/Materials/");
const QString assets3DPath("Resources/Assets3D/");
const QString scenariosPath("Resources/Scenarios/");
const QString boardsPath("Resources/Boards/");


const QString logFilesPath("Resources/Private/log_files/");

#endif

#endif // STATIC_GLOBAL_H
