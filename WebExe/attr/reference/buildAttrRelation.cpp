/*************************************************************************
	> File Name: buildAttrRelation.cpp
	> Author: zhangyan
	> Mail: 1534589721@qq.com 
	> Created Time: Wed 10 Aug 2016 11:06:31 AM CST
 ************************************************************************/

#include "SysConfig/defination.h"
#include <QFile>
#include "buildAttrRelation.h"
#define	sys_attrCorrespond ":/config/0attrCorrespond.config"
AttrConfigData *attrconfig;

AttrRelation::AttrRelation() {
	attrconfig = new AttrConfigData;
	buildAttrRelation();
}

void AttrRelation::buildAttrRelation() {
	QFile configFile(sys_attrCorrespond);
	if(configFile.open(QFile::ReadOnly)) {
		while(!configFile.atEnd()) {
			QString line = configFile.readLine(255);
			QStringList tmpStr;
			tmpStr = line.simplified().split(";");
			if(tmpStr.size() != 2) continue;
			attrRelation.insert(pair<QString, QString>(tmpStr.at(0), tmpStr.at(1)));
		}
	} else {
		cout << "load file error:" << sys_attrCorrespond << endl;
		exit(0);
	}
	configFile.close();
}

/**
 * attrName ==> recognize attrName
 * idx ==> recognize rtn value
 *
 * function:
 * replace the attrName by correspond subtitute attr
 * and return the idx of subtitute attr value
 */
string AttrRelation::getRelateValue(string attrName, int idx)
{
	QString res;

	//subtitute attr
    QString attrNameSub = attrRelation[QString(attrName.c_str())];
	int subAttrIdx = attrconfig->getIndexByName(attrNameSub);
    if(attrconfig->getAttrValuesByIndex(subAttrIdx).isEmpty()) {
		//the idx is a percent int value;
		double resD = idx / 10.0;
		res = QString::number(resD, 'f', 1);
	} else {
		//the idx is a real index;		
		res = attrconfig->getAttrValue(subAttrIdx, idx);
	}    
    return res.toStdString();
}
