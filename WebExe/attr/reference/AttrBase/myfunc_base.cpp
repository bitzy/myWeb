#include "myfunc_base.h"
#include <unistd.h>

#include <QStringList>
#include <QFile>
#include <QDomDocument>

void myFunc_pathSlashAdd(QString &fpath)
{
    int strLength = fpath.length();
    int slashPos = fpath.lastIndexOf("/");
    if(slashPos+1 != strLength) {
        fpath.append("/");
    }
}

void myFunc_confirmDirExist(const QString &fpath)
{
    if(access(fpath.toStdString().c_str(), F_OK) == -1) {
        QString cmd = QString("mkdir %1").arg(fpath);
        system(cmd.toStdString().c_str());
    }
}


void getStdPercent(const char* fpath, double &stdPercent)
{
    QString path(fpath);
    QStringList tmp = path.simplified().split(".");
    QString xmlPath = tmp.at(0);
    xmlPath.append(".xml");

    QFile filedata(xmlPath);
    if(!filedata.open(QFile::ReadOnly)) {
        exit(1);
    }
    QDomDocument doc;
    if(!doc.setContent(&filedata)) {
        filedata.close();
        exit(1);
    }
    QDomElement root = doc.documentElement();
    QDomNode firstNode = root.firstChild().nextSibling();
    //the second child node is attrvalue;
    QDomElement node = firstNode.firstChildElement("sleeveValue");
    QString res = node.text();
    filedata.close();

    stdPercent = res.toDouble();
}
