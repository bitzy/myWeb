#ifndef POSECONFIGDATA_H
#define POSECONFIGDATA_H

#include <QFile>
#include <QStringList>
//#include "POSE/mypose.h"

#define sys_poseConfigFile ":/config/0pose.config"
class PoseConfigData
{    
private:
    int poseCounter;            //pose data count
    QList<QString> poseName;    //pose data name
    QList<int> poseTypeIndex;   //pose data type Index

public:
    PoseConfigData(){
        QFile file(sys_poseConfigFile);
        if(!file.open(QFile::ReadOnly)) {
            exit(10);
        }
        while(!file.atEnd()) {
            QString line = file.readLine(255);
            QStringList tmpStr;
            tmpStr = line.simplified().split(":");
            if(tmpStr.size() != 2) {
                continue;
            }

            poseName << tmpStr.at(0);
            poseTypeIndex << tmpStr.at(1).toInt();
        }
        file.close();
        poseCounter = poseName.size();
    }

    int getPoseCounter() {
        return this->poseCounter;
    }

    QStringList getPoseName() {
        return this->poseName;
    }
    int getPoseIndexByName(QString name) {
        for(int i = 0; i < poseName.size(); i++){
            if(!poseName.at(i).compare(name)) return i;
        }
        return -1;
    }

    QString getPoseNameByIndex(int index) {
        if(index >= this->poseCounter) return QString("error");
        return  this->poseName.at(index);
    }
    int getPoseTypeIndexByIndex(int index){
        if(index >= this->poseCounter) return 0;
        return this->poseTypeIndex.at(index);
    }
};

#endif // POSECONFIGDATA_H
