#include "sleeveHeader.h"
#include "ScaleTool.h"

void getBoundingBox(const Mat& src, Rect &focusArea);
int scalePoint(SCALETOOL& scaleTool);

Mat preProcessing(const char* fpath, MyLocation* loc) {
    string fpathStr(fpath);
    Mat curImage = imread(fpathStr);
    loadPoseData(loc);

    Rect focusArea;
    getBoundingBox(curImage, focusArea);

    SCALETOOL scaleTool;
    scaleTool.scaleToFixHeight(curImage, focusArea);
    scalePoint(scaleTool);

    return curImage;
}

Mat sleeveProcessing(const char * fpath, MyLocation* loc, int& HSTYLE, bool& leftFlag) {
    string fpathStr(fpath);
    Mat curImage = imread(fpathStr);//CV_8UC3
    loadPoseData(loc);

    //get bounding box;
    Rect focusArea;
    getBoundingBox(curImage, focusArea);

    //scale image;
    SCALETOOL scaleTool;
    scaleTool.scaleToFixHeight(curImage, focusArea);
    //    if(system_debug) { imwrite("scale.jpg", scaledImg);}
    HSTYLE = scalePoint(scaleTool);

    //analyze choose left or right arm as result;
    double errorDist = 0.5;
    double lTopArmLen = getEuclideanDist(lshoulder, lelbow);
    double lDowArmLen = getEuclideanDist(lelbow, lhand);
    double leftDist = (lTopArmLen > lDowArmLen?
                           lTopArmLen/lDowArmLen : lDowArmLen/lTopArmLen) - 1;

    double rTopArmLen = getEuclideanDist(rshoulder, relbow);
    double rDowArmLen = getEuclideanDist(relbow, rhand);
    double rightDist = (rTopArmLen > rDowArmLen?
                            rTopArmLen/rDowArmLen : rDowArmLen/rTopArmLen) - 1;
    if(leftDist - rightDist > errorDist) {//two arm distance is too big
        leftFlag = (leftDist < rightDist? true:false);//choose the smaller
    }

    return curImage;
}

void getBoundingBox(const Mat &src, Rect& focusArea)
{
    Mat src2;

    int top = getTopLine();
    int right = getRightLine();
    int bottom = getBottomLine();
    int left = getLeftLine();

    Point topLeft = Point(left, top);
    Point bottomRight = Point(right, bottom);

    //    if(system_debug) {
    //        curImage.copyTo(src2);
    //        rectangle(src2, topLeft, bottomRight, Scalar(0,0,255));
    //        imwrite("sleeve3_boundBox.jpg", src2);
    //    }

    //calculate
    int H_height = bottom - top;
    int H_width = right - left;

    int marginVertical = H_height / 5;
    int marginHorinzol = H_width / 5;

    //get outer rect:
    int imgWidth = src.cols-1;
    int imgHeight = src.rows-1;

    int outTop = topLeft.y - marginVertical;
    outTop = (outTop < 0? 0:outTop);
    int outRight = bottomRight.x + marginHorinzol;
    outRight = (outRight > imgWidth? imgWidth:outRight);
    int outBottom = bottomRight.y + marginVertical;
    outBottom = (outBottom > imgHeight? imgHeight:outBottom);
    int outLeft = topLeft.x - marginHorinzol;
    outLeft = (outLeft < 0? 0:outLeft);

    focusArea = Rect(Point(outLeft, outTop), Point(outRight, outBottom));
    //    if(system_debug) {
    //        curImage.copyTo(src2);
    //        rectangle(src2, focusArea, Scalar(0,255,0));
    //        imwrite("sleeve3_boundBox2.jpg", src2);
    //    }
}

int scalePoint(SCALETOOL& scaleTool)
{
    //1- full body; 2-half body; 3-up body;
    int style;

    if(lknee.x == -1 || rknee.x == 2) style = 3;
    else if(lankle.x == -1 || rankle.x == -1) style = 2;

    //pose scale
    scaleTool.scalePoint(head);
    scaleTool.scalePoint(neck);
    scaleTool.scalePoint(lshoulder);
    scaleTool.scalePoint(lelbow);
    scaleTool.scalePoint(lhand);
    scaleTool.scalePoint(rshoulder);
    scaleTool.scalePoint(relbow);
    scaleTool.scalePoint(rhand);
    scaleTool.scalePoint(lhip);
    scaleTool.scalePoint(rhip);
    if(style != 3) {
        scaleTool.scalePoint(lknee);
        scaleTool.scalePoint(rknee);
        if(style != 2) {
            scaleTool.scalePoint(lankle);
            scaleTool.scalePoint(rankle);
        }
    }
    return style;
}

double cov(const int hist1[], const int hist2[], int dim)
{
    double ret = 0.0;

    double total1 = 0.0, total2 = 0.0;
    for (int i = 0; i < dim; i++) {
        total1 += hist1[i];
        total2 += hist2[i];
    }
    total1 /= dim;
    total2 /= dim;
    for(int i = 0; i < dim; i++) {
        ret += ((hist1[i]*1.0 - total1) * (hist2[i]*1.0) - total2);
    }
    return ret / (dim - 1);
}
