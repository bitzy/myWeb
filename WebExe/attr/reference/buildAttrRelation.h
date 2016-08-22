/*************************************************************************
	> File Name: buildAttrRelation.h
	> Author: zhangyan
	> Mail: 1534589721@qq.com 
	> Created Time: Wed 10 Aug 2016 11:02:04 AM CST
 ************************************************************************/
#ifndef AttrRelation_H
#define AttrRelation_H

#include <map>
#include <iterator>
#include <QString>
using namespace std;

class AttrRelation
{
private:
	map<QString, QString> attrRelation;
	void buildAttrRelation();
public:
	AttrRelation();
	string getRelateValue(string attrName, int idx);
};

#endif
