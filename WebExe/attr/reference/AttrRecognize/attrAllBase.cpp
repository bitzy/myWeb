#include "attrRecognize.h"

#define POSE_HEAD "head"
#define POSE_NECK "neck"
#define POSE_LSHOULDER "left_shoulder"
#define POSE_RSHOULDER "right_shoulder"
#define POSE_LELBOW "left_elbow"
#define POSE_RELBOW "right_elbow"
#define POSE_LHAND "left_hand"
#define POSE_RHAND "right_hand"
#define POSE_LHIP "left_hip"
#define POSE_RHIP "right_hip"
#define POSE_LKNEE "left_knee"
#define POSE_RKNEE "right_knee"
#define POSE_LANKLE "left_ankle"
#define POSE_RANKLE "right_ankle"

bool system_debug;
//definition: all ways use this.
Point head, neck, lshoulder, rshoulder;
Point lelbow, relbow, lhand, rhand, lhip, rhip;
Point lknee, rknee, lankle, rankle;

void loadPoseData(MyLocation * loc)
{
    head = loc->getPointByName(POSE_HEAD);    
    neck = loc->getPointByName(POSE_NECK);
    lshoulder = loc->getPointByName(POSE_LSHOULDER);
    rshoulder = loc->getPointByName(POSE_RSHOULDER);
    lelbow = loc->getPointByName(POSE_LELBOW);
    relbow = loc->getPointByName(POSE_RELBOW);
    lhand = loc->getPointByName(POSE_LHAND);
    rhand = loc->getPointByName(POSE_RHAND);
    lhip = loc->getPointByName(POSE_LHIP);
    rhip = loc->getPointByName(POSE_RHIP);
    lknee = loc->getPointByName(POSE_LKNEE);
    rknee = loc->getPointByName(POSE_RKNEE);
    lankle = loc->getPointByName(POSE_LANKLE);
    rankle = loc->getPointByName(POSE_RANKLE);
}

bool sortDescNodeByConf(const node &v1, const node &v2) {
    return v1.confidence > v2.confidence;
}

bool sortPairDescNodeByConf(const pairNode &v1, const pairNode &v2) {
    return v1.confidencesum > v2.confidencesum;
}

int descNodeByConf(const void * a, const void * b) {
    return (*(node *)b).confidence > (*(node *)a).confidence ? 1:-1;
}

int descPairNodeByConf(const void * a, const void * b) {
    return (*(pairNode *)b).confidencesum > (*(pairNode *)a).confidencesum ? 1 : -1;
}

void openMidRes(bool debugSwitch)
{
    if(debugSwitch) { system_debug = true;
    } else { system_debug = false; }
}

void getMatPixelBGR(const Mat& src, const Point& p, Vec3b& res)
{
    res = src.at<Vec3b>(p.x, p.y);
}

//compare 14 variable, get the minimum value;
int getMinimum7(int a, int b, int c, int d, int e, int f, int g)
{
    int res = (a < b? a: b);
    if(c == -1) return res;
    else res = (res < c ? res: c);
    if(d == -1) return res;
    else res = (res < d? res: d);
    if(e == -1) return res;
    else res = (res < e ? res: e);
    if(f == -1) return res;
    else res = (res < f ? res: f);
    if(g == -1) return res;
    else res = (res < g ? res: g);

    return res;
}

int getMaximum7(int a, int b, int c, int d, int e, int f, int g)
{
    int res = (a > b? a: b);
    if(c == -1) return res;
    else res = (res > c ? res: c);
    if(d == -1) return res;
    else res = (res > d? res: d);
    if(e == -1) return res;
    else res = (res > e ? res: e);
    if(f == -1) return res;
    else res = (res > f ? res: f);
    if(g == -1) return res;
    else res = (res > g ? res: g);

    return res;
}

/**
 * @brief getTopLine
 * @return
 */
int getTopLine()
{              
    return getMinimum7(head.y, lelbow.y, relbow.y, lhand.y, rhand.y);
}

int getLeftLine()
{
    int res = getMinimum7(lshoulder.x, lelbow.x, lhand.x, relbow.x, rhand.x, lhip.x);
    if(lknee.x == -1 || rknee.x == -1) {
        return res;
    } else if(lankle.x == -1 || rankle.x == -1) {
        res = getMinimum7(res, lknee.x, rknee.x);
    } else {
        res = getMinimum7(res, lknee.x, rknee.x, lankle.x, rankle.x);
    }
    return res;
}

int getRightLine()
{
    int res = getMaximum7(rshoulder.x, relbow.x, rhand.x, lelbow.x, lhand.x, rhip.x);
    if(lknee.x == -1 || rknee.x == -1) {
        return res;
    } else if(lankle.x == -1 || rankle.x == -1) {
        res = getMaximum7(res, lknee.x, rknee.x);
    } else {
        res = getMaximum7(res, lknee.x, rknee.x, lankle.x, rankle.x);
    }
    return res;
}

int getBottomLine()
{
    int res = getMaximum7(lelbow.y, lhand.y, relbow.y, rhand.y, lhip.y, rhip.y);
    if(lknee.x == -1 || rknee.x == -1) {
        return res;
    } else if(lankle.x == -1 || rankle.x == -1){
        res = getMaximum7(res, lknee.y, rknee.y);
    } else {
        res = getMaximum7(res, lknee.y, rknee.y, lankle.y, rankle.y);
    }
    return res;
}
