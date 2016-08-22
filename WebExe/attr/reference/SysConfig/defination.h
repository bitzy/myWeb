#ifndef DEFINATION_H
#define DEFINATION_H

#include "poseconfigdata.h"       //pose config base data
#include "attrconfigdata.h"       //attr config base data
#include "../AttrRecognize/attrRecognize.h"    //attr's recognition function
#include "../AttrRecognize/mylocation.h"       //attr's coresponding xml data

//some globel limit:
#define PATH_LEN_LIMIT      255
#define FILE_LINE_LIMIT     255
#define BASEDATA_NAME_LIMIT 30
#define POSE_NAME_LIMIT     30
#define ATTR_NAME_LIMIT     30

//some path definition:
//#define sys_baseConfigFile ":/config/0baseData.config"
//#define sys_dealImagePath   "../dealImagePath"
//#define sys_storeImagePath  "../LABELresult/"
// /home/sharon/Videos
#define SYS_LABELPATH_DEFAULT    "/home/sharon/Videos/500Test"
#define SYS_BULKPATH_DEFAULT     "/home/sharon/Videos/500Test"
//#define SYS_IMGXMLPATH  "/home/sharon/Documents/code/qt_project/AttrAnalyze/wrongSleeve"    //"/home/sharon/Videos/500Test/"
#define SYS_DELPATH     "../delImg/"
#define BULKPATH_DATA_FNAME "data.config"


//===============================================================
//  system basedata:
//===============================================================
//BASE DATATYPE:
#define SYSTEM_BASEDATA_TYPES 4

enum FLAG_SHAPE {
    POINT, LINE, RECT, COLORVALUE
};

struct System_BaseData {
    char name[BASEDATA_NAME_LIMIT];
    int detail;
};
extern System_BaseData sysBaseData[SYSTEM_BASEDATA_TYPES];


//===============================================================
//  global struct variable:
//===============================================================
//define in mainwindow.cpp
extern AttrConfigData *attrconfig;
extern PoseConfigData *poseconfig;
extern QStringList imageTypefilter;


//===============================================================
//  global function definition:
//===============================================================
typedef int(*ATTR_RECOGNITION_FUNC)(const char*, MyLocation*);

struct FUNC_ITEM{
    const char* func_name;
    ATTR_RECOGNITION_FUNC func_address;
};
extern FUNC_ITEM func_list[][5];


//===============================================================
//  global functions class:
//===============================================================
#define FUNC_KIND 5

class ATTRWAYS{
    QList<QString> funcForAttrName;
    QList< QList<FUNC_ITEM> > funcs;
public:
    ATTRWAYS(){
        funcForAttrName << "collar" //0
                        << "sleeve" //1
                        << "up_length"  //2
                        << "texture";   //3
        QList<FUNC_ITEM> tmp;
        //collar:
        FUNC_ITEM a1 = {"base way", baseWayCollar};
        FUNC_ITEM a2 = {"Test Collar", testWayCollar};
        tmp << a1 << a2;
        funcs.push_back(tmp);
        //sleeve:
        tmp.clear();
        FUNC_ITEM b1 = {"base way", baseWaySleeve};
        FUNC_ITEM b2 = {"Test Sleeve", testWaySleeve};
        FUNC_ITEM b3 = {"New Sleeve", myNewWaySleeve};
        FUNC_ITEM b4 = {"Skin Sleeve", addSkinSleeve};
        tmp << b1 << b2 << b3 << b4;
        funcs.push_back(tmp);
        //uplength:
        tmp.clear();
        FUNC_ITEM d1 = {"base way", upLengAnalyze};
        tmp << d1;
        funcs.push_back(tmp);
        //texture:
        tmp.clear();
        FUNC_ITEM e1 = {"base way", textureAnalyze};
        tmp << e1;
        funcs.push_back(tmp);
    }
    QStringList getWays(QString name) {
        int size = funcForAttrName.size();
        int pos = -1;
        for(int i = 0; i < size; i++) {
            if(!name.compare(funcForAttrName.at(i))) {
                pos = i;
                break;
            }
        }
        QStringList res;
        if (pos == -1) return res;

        int waysNum = funcs.at(pos).size();
        for(int i = 0; i < waysNum; i++) {
            res << funcs.at(pos).at(i).func_name;
        }
        return res;
    }
    ATTR_RECOGNITION_FUNC getAttrFuncAddress(QString attrName, int j) {
        int size = funcForAttrName.size();
        int pos = -1;
        for(int i = 0; i < size; i++) {
            if(!attrName.compare(funcForAttrName.at(i))) {
                pos = i;
                break;
            }
        }
        if (pos == -1) exit(20);
        return funcs.at(pos).at(j).func_address;
    }
};
extern ATTRWAYS *sysAttrWays;


#endif // DEFINATION_H
