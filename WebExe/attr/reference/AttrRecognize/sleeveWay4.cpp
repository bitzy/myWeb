#include "sleeveHeader.h"
#include "opencv2/ximgproc.hpp"
#include "imagesp.h"
#include "skincoloranalyze.h"

#include <QDebug>
using namespace cv::ximgproc;

Rect getHeadRect(const Mat& src);
Rect getBodyRect(const Mat& src);

int addSkinSleeve(  const char * fpath, MyLocation* loc)
{
    int HSTYLE;
    bool leftFlag = true; //default: left; false:right;
    Mat curImage = sleeveProcessing(fpath, loc, HSTYLE, leftFlag);

    Ptr<SuperpixelSLIC> slic = createSuperpixelSLIC(curImage, SLICO);
    slic->iterate(25);

    Mat labels;
    slic->getLabels(labels);

    SPIMAGE imgSPfeat(curImage, labels);
    imgSPfeat.setColorByHistomForEachSp();
    //-----------------------------------------------------------------
        if(system_debug) imgSPfeat.getSpColorImage("spColor.jpg");
    //-----------------------------------------------------------------

    Rect headRect = getHeadRect(curImage);
    //-----------------------------------------------------------------
    //    Mat face = imgSPfeat.getRectColorImage(headRect);
    //    imwrite("face.jpg", face);
    //-----------------------------------------------------------------
    SkinColorAnalyze skinTool(curImage, headRect);
    //Vec3b colorSkin = skinTool.getSkinColor();
    //-----------------------------------------------------------------
    skinTool.getSkinColorImg("headskin.jpg");
    //-----------------------------------------------------------------


    //Rect bodyRect = getBodyRect(curImage);
    int lTercent;
    double lconf = imgSPfeat.getSleeveTercent(lshoulder, lelbow, lhand, lTercent, "left");
    int rTercent;
    double rconf = imgSPfeat.getSleeveTercent(rshoulder, relbow, rhand, rTercent, "right");
    //-----------------------------------------------------------------
    //    double stdPercent;
    //    getStdPercent(fpath, stdPercent); stdPercent*=10;
    //    cout << lTercent << "," << lconf << "," << stdPercent << endl;
    //    cout << rTercent << "," << rconf << "," << stdPercent << endl;
    //-----------------------------------------------------------------
    if(leftFlag) return lTercent;
    else return rTercent;
}

Rect getHeadRect(const Mat& /*src*/)
{
    Rect res;
    double bodyWidth = getEuclideanDist(lshoulder, rshoulder);
    float width = int(bodyWidth/3);

    Point bodyUpCenter = (lshoulder+rshoulder)*0.5;
    Point bodyDownCenter = (lhip + rhip)*0.5;
    double bodyHeight = getEuclideanDist(bodyUpCenter, bodyDownCenter);
    float height = int(bodyHeight/3);

    Point2f center(neck.x, neck.y);
    center.y -= height/2;
    if(center.y < 1e-6) center.y = 0;

    float angle = 0;
    if(head.x - neck.x > 20) {
        angle = getAngle180ByThreeP(neck, head, Point(neck.x, 0));
    } else if(head.x - neck.x < -10) {
        angle = getAngle180ByThreeP(neck, head, Point(neck.x, 0));
        angle = -angle;
    }
    RotatedRect rect = RotatedRect(center, Size2f(width, height), -angle);
    res = rect.boundingRect();
    //-----------------------------------------------------------------
    //    Mat src2;
    //    src.copyTo(src2);

    //    Point2f vertices[4];
    //    rect.points(vertices);
    //    for(int i = 0; i < 4; i++) {
    //        line(src2, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));
    //    }
    //    rectangle(src2, res, Scalar(0,0,255));
    //    imwrite("head.jpg", src2);
    //-----------------------------------------------------------------
    return res;
}

Rect getBodyRect(const Mat& /*src*/)
{
    //
}
