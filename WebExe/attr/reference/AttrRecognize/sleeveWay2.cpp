#include "attrRecognize.h"
#include "skinmodel.h"

skinModel * bayesSkinModel;
bool skinModelFlag = false;

void loadSkinModel()
{
    if(!skinModelFlag) {
        bayesSkinModel = new skinModel();
        skinModelFlag = true;
    }
}

/*********************************************************************************************
 * Sleeve recognize program.
 * Revise from Hanzhitian program.By zhangyan 2016.5.20.
 *
 * This program get sleeve attribute value:
 *  0: no sleeve;
 *  1: short sleeve;
 *  2: half sleeve;
 *  3: long sleeve;
 *
* ********************************************************************************************/

//function:
void getSamplePoints(vector<Point>& samplePoints, const int sampleN);
void _getPoints(const Point start, const Point end, const int n, vector<Point>& vec);
void getMatSkinMask(const Mat& src, Mat& skinMask, skinModel * bayesSkinModel);
void getPointTag(const Mat& src, const vector<Point>& samplePoints,
                 skinModel* bayesSkinModel, vector<bool>& pointTag);

int testWaySleeve(const char * fpath, MyLocation* loc)
{
    //load position;
    loadPoseData(loc);

    //sample data:
    vector<Point> samplePoints;
    int sampleN = 20;//each part sample 20 dots
    getSamplePoints(samplePoints, sampleN);

    //skin color ==> Bayes model.
    loadSkinModel();
    if(system_debug)
        cout << "loading model ok!" << endl;

    //load deal image;
    string fpathStr(fpath);
    Mat src = imread(fpathStr);

    //get skin judgement result mat:
    Mat skinMask;
    getMatSkinMask(src, skinMask, bayesSkinModel);

    //bayes model skin judge:
    vector<bool> pointTag;
    getPointTag(src, samplePoints, bayesSkinModel, pointTag);

    return 1;
}

void getSamplePoints(vector<Point> &samplePoints, const int sampleN)
{
    samplePoints.reserve(sampleN * 4);
    _getPoints(lshoulder, lelbow, sampleN, samplePoints);
    _getPoints(lelbow, lhand, sampleN, samplePoints);
    _getPoints(rshoulder, relbow, sampleN, samplePoints);
    _getPoints(relbow, rhand, sampleN, samplePoints);
}

void _getPoints(const Point start, const Point end, const int n, vector<Point>& vec) {
    assert(n > 1);
    double fraction = n-1;

    for(int i = 0; i < n; i++) {
        Point dot = i/fraction*end + (fraction-i)/fraction*start;
        vec.push_back(dot);
    }
}

void getMatSkinMask(const Mat& src, Mat& skinMask, skinModel * bayesSkinModel)
{
    skinMask = Mat::zeros(src.size(), CV_8UC1);

    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++){
            Vec3b bgr;
            getMatPixelBGR(src, Point(i, j), bgr);

            bool res = bayesSkinModel->judgePixelSkin(
                        bgr[rChannel], bgr[gChannel], bgr[bChannel]);
//            bool res = _colorIsSkin(bgr[rChannel], bgr[gChannel],
//                                    bgr[bChannel]);
            if(res)
                skinMask.at<uchar>(i,j) = 255;
            else skinMask.at<uchar>(i,j)= 0;
        }
    }
    imwrite("skinRes.jpg", skinMask);
}

void getPointTag(const Mat& src, const vector<Point>& samplePoints,
                 skinModel * bayesSkinModel, vector<bool> &pointTag)
{
    int size = samplePoints.size();
    pointTag.reserve(size);

    Mat_<Vec3b> srcCopy = src;
    for(int i = 0; i < size; i++) {
        Vec3b bgr;
        getMatPixelBGR(src, samplePoints.at(i), bgr);
        bool skinFlag = bayesSkinModel->judgePixelSkin(bgr[rChannel], bgr[gChannel], bgr[bChannel]);
        pointTag.push_back(skinFlag);
        if(skinFlag)
            circle(srcCopy, samplePoints.at(i), 3, Scalar(255,0,0));
        else
            circle(srcCopy, samplePoints.at(i), 3, Scalar(0,0,0));
    }
    imwrite("skinJudge.jpg", srcCopy);
}

