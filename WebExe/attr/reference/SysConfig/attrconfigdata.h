#ifndef ATTRCONFIGDATA_H
#define ATTRCONFIGDATA_H

#include <QFile>
#include <QStringList>

#define sys_attrConfigFile ":/config/0attr.config"
class AttrConfigData
{
private:
    int maxAlternative;         //used to build show table;
    QList<QString> attrName;    //recognize attribution;
    QList<QString> labelAttrName;   //used to store label value
    QList<QStringList> attrValues;  //attribution's value;

public:
    AttrConfigData() {
        this->maxAlternative = -1;
        QFile file(sys_attrConfigFile);
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

            QStringList tmpValues;
            tmpValues = tmpStr.at(1).split(";");
            int tmpValueSize = tmpValues.size();
            if(tmpValueSize > maxAlternative)
                maxAlternative = tmpValueSize;

            if(tmpValueSize == 1 && tmpValues.at(0).isEmpty()) {
                labelAttrName.push_back(tmpStr.at(0));
            } else {
                attrName.push_back(tmpStr.at(0));
                attrValues.push_back(tmpValues);
            }
        }
        file.close();
    }


    int getMaxAlternative() { return this->maxAlternative; }

    QStringList getAttrs() { return this->attrName; }
    QStringList getAllAttrs() {
        QStringList res;
        res << attrName << labelAttrName;
        return res;
    }
    int attrKind() { return attrName.size();}
    int getAllKind() { return attrName.size() + labelAttrName.size();}
    int getIndexByName(QString name) {//used for analyzer construction matrics;
        int size = attrName.size();
        for(int i = 0; i < size; i++) {
            if(!attrName.at(i).compare(name))
                return i;
        }
        size = labelAttrName.size();
        for(int i = 0; i < size; i++) {
            if(!labelAttrName.at(i).compare(name))
                return i+attrName.size();
        }
        return -1;
    }
    QString getAttrNameByIndex(int index) {
        if(index < attrName.size())
            return attrName.at(index);
        return labelAttrName.at(index - attrName.size());
    }


    QStringList getAttrValuesByattrName(QString name) {
        int size = attrName.size();
        for(int i = 0; i < size; i++) {
            if(!attrName.at(i).compare(name))
                return attrValues.at(i);
        }
        return QStringList();
    }
    QStringList getAttrValuesByIndex(int index){
        if(index >= attrValues.size()) return QStringList();
        return attrValues.at(index);
    }
    int getIndexByAttrValue(int AttrIndex, QString value) {
        int size = attrValues.at(AttrIndex).size();
        for (int i = 0; i < size; i++) {
            if(attrValues.at(AttrIndex).at(i).compare(value) == 0) {
                return i;
            }
        }
        return -1;
    }
    QString getAttrValue(int kind, int index)
    {
        QStringList tmp = this->attrValues.at(kind);
        return tmp.at(index);
    }
};

#endif // ATTRCONFIGDATA_H
