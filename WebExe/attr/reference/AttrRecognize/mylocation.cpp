#include "mylocation.h"
#include <QFile>
#include <QtXml>

#define sys_testDataForm ":/config/0testDataForm.config"

/////////////////////////////////////////////////////
/// \brief MyLocation::MyLocation
/// \param data
/// \param form
/// \param basetype
/// \return
///
MyLocation::MyLocation()
{
    imgDir.clear();
    imgBasename.clear();
    imgFilename.clear();    
}

/**
 * @brief MyLocation::loadPosition
 *      load the pose data. return the load status.
 * @return
 *      load success: return 1;
 *      else : return error code;
 *
 * error code:
 *      0: testDataForm.config layout is wrong.
 *      -1: .xml or testDataForm.config is missing.
 *      -2: .xml hasn't the pose node.
 *      -3: .xml label data not support the datatype.
 */
int MyLocation::loadPosition(QString& fpath)
{
    QFileInfo imgInfo(fpath);
    imgDir = imgInfo.absolutePath();
    imgBasename = imgInfo.baseName();
    imgFilename = imgInfo.fileName();
    //    imgXmlFilename = QString("%1.xml").arg(imgBasename);

    QString data = QString("%1%2.xml").arg(fPathSlashCheck(imgDir)).arg(imgBasename);
    QFile fileData(data);
    QFile fileForm(sys_testDataForm);
    if (!fileForm.open(QFile::ReadOnly) || !fileData.open(QFile::ReadOnly)) {
        return -1;
    }

    QDomDocument doc;
    if (!doc.setContent(&fileData)) {
        fileData.close();
        return 0;
    }

    while(!fileForm.atEnd()) {
        QString line = fileForm.readLine(255).simplified();
        QDomNodeList nodes = doc.elementsByTagName(line);
        if (!nodes.size()) return -2;

        QString value = nodes.at(0).toElement().text();
        int x, y;
        if(value.compare("null") == 0) {
            x = y = -1;
        } else {
            QStringList poseDataStrs;
            poseDataStrs = value.simplified().split(",");
            int size = poseDataStrs.size();
            if(size != 2) return -3;
            x = poseDataStrs.at(0).toInt();
            y = poseDataStrs.at(1).toInt(); //value: (x, y)
        }      
        locDetail.insert(pair<string, Point>(line.toStdString(), Point(x, y)));
    }
    fileForm.close();
    fileData.close();

    return 1;
}

int MyLocation::loadPosition(QString& fpath, QString& xmlpath)
{
    QFileInfo imgInfo(fpath);
    imgDir = imgInfo.absolutePath();
    imgBasename = imgInfo.baseName();
    imgFilename = imgInfo.fileName();
    //    imgXmlFilename = xmlpath;

    QString data = xmlpath;
    QFile fileData(data);
    QFile fileForm(sys_testDataForm);
    if (!fileForm.open(QFile::ReadOnly) || !fileData.open(QFile::ReadOnly)) {
        return -1;
    }

    QDomDocument doc;
    if (!doc.setContent(&fileData)) {
        fileData.close();
        return 0;
    }

    while(!fileForm.atEnd()) {
        QString line = fileForm.readLine(255).simplified();
        QDomNodeList nodes = doc.elementsByTagName(line);
        if (!nodes.size()) return -2;

        QString value = nodes.at(0).toElement().text();
        int x, y;
        if(value.compare("null") == 0) {
            x = y = -1;
        } else {
            QStringList poseDataStrs;
            poseDataStrs = value.simplified().split(",");
            int size = poseDataStrs.size();
            if(size != 2) return -3;
            x = poseDataStrs.at(0).toInt();
            y = poseDataStrs.at(1).toInt(); //value: (x, y)
        }
        locDetail.insert(pair<string, Point>(line.toStdString(), Point(x, y)));
    }
    fileForm.close();
    fileData.close();

    return 1;
}

/**
 * @brief MyLocation::loadPosition
 *  read pos data from fpath file, the file layout should consistent with: sys_testDataForm;
 * @param fpath
 * @return
 */
int MyLocation::loadPosition(string fpath)
{    
    QFile fileData(fpath.c_str());
    QFile fileForm(sys_testDataForm);
    if(!fileData.open(QFile::ReadOnly) || !fileForm.open(QFile::ReadOnly)) {
        return -1;
    }
    while(!fileData.atEnd()) {
        QString line = fileForm.readLine(255).simplified();

        QString value = fileData.readLine(255).simplified();
        int x, y;
        if(value.compare("null") == 0) {
            x = y = -1;
        } else {
            QStringList poseDataStrs;
            poseDataStrs = value.simplified().split(",");
            int size = poseDataStrs.size();
            if(size!=2) return -3;
            x = poseDataStrs.at(0).toInt();
            y = poseDataStrs.at(1).toInt();
        }
        locDetail.insert(pair<string, Point>(line.toStdString(), Point(x, y)));
    }
    fileData.close();
    fileForm.close();

    return 1;
}


Point MyLocation::getPointByName(const char *name)
{
    QString tmp(name);
    map<string, Point>::iterator it;
    it = locDetail.find(tmp.toStdString());
    if (it != locDetail.end()) {
        return locDetail[tmp.toStdString()];
    } else {
        return Point(-1,-1);
    }
}

QString& MyLocation::fPathSlashCheck(QString& fpath)
{
    int strLength = fpath.length();
    int slashPos = fpath.lastIndexOf("/");
    if(slashPos+1 != strLength) {
        fpath.append("/");
    }
    return fpath;
}
