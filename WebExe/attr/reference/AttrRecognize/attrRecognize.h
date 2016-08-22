#ifndef ATTRRECOGNIZE_H
#define ATTRRECOGNIZE_H

#include "../AttrBase/cvTool.h"
#include "mylocation.h"
#include "../AttrBase/planimetry_tools.h"

#define rChannel 2
#define gChannel 1
#define bChannel 0

#define SYSTEST_DIR "../singleExeDir/"

/**********************************************************************
 * inner interface
**********************************************************************/
//truely definition in sleeveWay2.cpp
#define COLLAR_TYPE 4
#define ROUND_MODEL 5
#define VSTYLE_MODEL 5
#define FCLOSE_MODEL 1
#define FOPEN_MODEL 4

extern struct svm_model* modelForR[ROUND_MODEL];
extern struct svm_model* modelForV[VSTYLE_MODEL];
extern struct svm_model *modelForFclose[FCLOSE_MODEL];
extern struct svm_model *modelForFopen[FOPEN_MODEL];

extern Point head, neck, lhip, rhip;
extern Point lshoulder, rshoulder, lelbow, relbow, lhand, rhand;
extern Point lknee, rknee, lankle, rankle;

void loadPoseData(MyLocation * loc);
bool sortDescNodeByConf(const node &v1, const node &v2);
bool sortPairDescNodeByConf(const pairNode &v1, const pairNode &v2);
int descNodeByConf(const void * a, const void * b);
int descPairNodeByConf(const void * a, const void * b);
void getMatPixelBGR(const Mat& src, const Point& p, Vec3b& res);


/************************************************************************
 * output interface:
 *
 * each attribute interface:
 *      -fpath: dealImage;
 *      -loc: the corresponding pose file;
 *      -output path: store data to the file;
 *
 * return value: return the recognize index;
 *              or nothing;
 * **********************************************************************/
//debug variables:
extern bool system_debug;
void openMidRes(bool debugSwitch);
int getMinimum7(int a, int b, int c=-1, int d=-1, int e=-1, int f=-1, int g=-1);
int getMaximum7(int a, int b, int c=-1, int d=-1, int e=-1, int f=-1, int g=-1);
int getTopLine();
int getLeftLine();
int getRightLine();
int getBottomLine();


//layout:
int layoutAnalyze(  const char * fpath, MyLocation* loc);
int layoutAnalyze2( const char * fpath, MyLocation* loc);


//collar:
int baseWayCollar(  const char * fpath, MyLocation* loc);
int testWayCollar(  const char * fpath, MyLocation* loc);


//sleeve:
int baseWaySleeve( const char * fpath, MyLocation* loc);
int testWaySleeve( const char * fpath, MyLocation* loc);
int myNewWaySleeve(const char * fpath, MyLocation* loc);
int addSkinSleeve(  const char * fpath, MyLocation* loc);


//uplength:
int upLengAnalyze(  const char * fpath, MyLocation* loc);
int upLengAnalyze2( const char * fpath, MyLocation *loc);

//textrue:
int textureAnalyze( const char * fpath, MyLocation* loc);

//color:
void getColor(const char* fpath, MyLocation* loc, char* output);

//extra method:
void getClothesMainColor(Mat src, MyLocation* loc, ColorFeat2 colorFeatureRes[]);
void getClothColorFeatureByCount(
        Mat colorRigion[], const int colorRigionNum,
        ColorFeat colorFeatureRes[], double threshold);



#endif // ATTRRECOGNIZE_H
