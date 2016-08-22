#ifndef MYLOCATION_H
#define MYLOCATION_H

#include "../AttrBase/cvTool.h"

#include <QString>
#include <iostream>
#include <map>
#include <iterator>
using namespace std;

class MyLocation {
private:
    QString imgDir;
    QString imgBasename;
    QString imgFilename;
    //    QString imgXmlFilename;
    map<string, Point> locDetail;

    QString& fPathSlashCheck(QString& fpath);

public:
    MyLocation();
    int loadPosition(QString& fpath);
    int loadPosition(QString& fpath, QString& xmlpath);
    int loadPosition(string fpath);
    Point getPointByName(const char* name);
};

#endif // MYLOCATION_H
