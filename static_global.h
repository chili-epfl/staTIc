#ifndef STATIC_GLOBAL_H
#define STATIC_GLOBAL_H
#include <QString>

#if ANDROID
const QString extstr=QString(getenv("EXTERNAL_STORAGE"))+"/staTIc/";
const QString materialsPath(extstr+"Resources/Materials/");
const QString assets3DPath(extstr+"Resources/Assets3D/");
const QString scenariosPath(extstr+"Resources/Scenarios/");
const QString boardsPath(extstr+"Resources/Boards/");

const QString materialsPrivatePath(extstr+"Resources/Materials/Private/");
const QString assets3DPrivatePath(extstr+"Resources/Assets3D/Private/");
const QString scenariosPrivatePath(extstr+"Resources/Scenarios/Private/");
const QString logFilesPath(extstr+"Resources/Private/log_files/");

#else
const QString materialsPath("Resources/Materials/");
const QString assets3DPath("Resources/Assets3D/");
const QString scenariosPath("Resources/Scenarios/");
const QString boardsPath("Resources/Boards/");

const QString materialsPrivatePath("Resources/Materials/Private/");
const QString assets3DPrivatePath("Resources/Assets3D/Private/");
const QString scenariosPrivatePath("Resources/Scenarios/Private/");
const QString logFilesPath("Resources/Private/log_files/");

#endif

#endif // STATIC_GLOBAL_H
