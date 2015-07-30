#ifndef STATICSHELPER_H
#define STATICSHELPER_H

#include <QObject>

class StaticsHelper : public QObject
{
    Q_OBJECT
public:
    enum Roles{ENTITY3D,MODEL,VIEWMODEL,F_REACTION,F_INTERNAL,FBD,FBD_ELEMENT};
    explicit StaticsHelper(QObject *parent = 0);
    static QString NameResolution(QString s, Roles input_role, Roles output_role);

signals:

public slots:
};

#endif // STATICSHELPER_H
