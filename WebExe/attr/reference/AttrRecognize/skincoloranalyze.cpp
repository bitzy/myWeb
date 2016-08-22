#include "skincoloranalyze.h"
#include "hist2DTool.h"

SkinColorAnalyze::SkinColorAnalyze(const Mat &img, const Rect &area)
{
    Rect newArea = area;
    //undo!

//    Vec3b resColor;
//    while(1) {
//        Mat imgROI = img(newArea);
//        Mat src;
//        imgROI.copyTo(src);
//        //-----------------------------------------------------------------
//        imwrite("head.jpg", src);
//        //-----------------------------------------------------------------
//        Mat srcHSV;
//        cvtColor(src, srcHSV, CV_BGR2HSV);
//        //-----------------------------------------------------------------
//        imwrite("hsv.jpg", srcHSV);
//        //-----------------------------------------------------------------
//        HistoGram2D histTool;
//        Point res;
//        histTool.getHistogramMaxHS(srcHSV, res);
//        int V_num;
//        histTool.getHistogram1DMax(srcHSV, V_num);

//        Mat tmp;
//        tmp.create(Size(1,1), CV_8UC3);
//        tmp.at<Vec3b>(0,0) = Vec3b(res.x, res.y, V_num);
//        cvtColor(tmp, tmp, CV_HSV2BGR);
//        resColor = tmp.at<Vec3b>(0,0);
//        if(judgeSkinByPixel(resColor)) {
//            cout << "get skin" << endl;
//            break;
//        } else {
//            cout << "isn't skin" << endl;
//            newArea.x += newArea.width/4;
//            newArea.width -= (newArea.width/2);
//        }
//        if(newArea.width < 20) {
//            cout << "no skin" << endl;
//            break;
//        }
//    }
//    color = resColor;
}

SkinColorAnalyze::SkinColorAnalyze(const Mat &/*img*/)
{
    //aggregatoin by color;
}

Vec3b SkinColorAnalyze::getSkinColor()
{
    return color;
}

void SkinColorAnalyze::getSkinColorImg(const char *outName)
{
    int size= 50;
    Mat res(size, size, CV_8UC3, color);
    imwrite(outName, res);
}

bool SkinColorAnalyze::judgeSkinByPixel(const Vec3b &color)
{
    int r = color[2];
    int g = color[1];
    int b = color[0];
    if(r > 95 && g > 40 && b > 20 &&
            r > b && r > g && abs(r - g) > 15) {
        int max = r;
        int min = (g > b ? b:g);
        if(max - min > 15) return true;
    }
    return false;
}
