#ifndef HIST2DTOOL_H
#define HIST2DTOOL_H

#include "../AttrBase/cvTool.h"

class HistoGram2D
{
private:
    int histSize[2];//H & S
    float hranges[2][2];
    const float * ranges[2];
    int channels[2];

public:
    HistoGram2D() {
        histSize[0] = 60;//H:0~180 ==> rows
        histSize[1] = 64;//S:0~256 ==> cols

        hranges[0][0] = 0;
        hranges[0][1] = 180;
        hranges[1][0] = 0;
        hranges[1][1] = 256;

        ranges[0] = hranges[0];
        ranges[1] = hranges[1];

        channels[0] = 0;
        channels[1] = 1;
    }

    void getHistogramMaxHS(const Mat &image, Point& res) {
        MatND hist;
        calcHist(&image, 1, channels, Mat(),
                 hist, 2, histSize, ranges);
        double minVal = 0, maxVal = 0;
        Point maxLoc;
        minMaxLoc(hist, &minVal, &maxVal, NULL, &maxLoc);

        res.x = maxLoc.y*3;//H
        res.y = maxLoc.x*4;//S
    }

    void getHistogram1DMax(const Mat& image, int &res){
        MatND hist;

        int vHistSize[1] = {64};//V:0~256
        float vHranges[2] = {0, 256};
        const float * vRanges[1] = {vHranges};
        int vChannels[1] = {2};
        calcHist(&image, 1, vChannels, Mat(),
                 hist, 1, vHistSize, vRanges);
        double minValue, maxValue;
        Point maxLoc;
        minMaxLoc(hist, &minValue, &maxValue, NULL, &maxLoc);
        res = maxLoc.y*4;
    }

//    Mat getHistogramImage(MatND &histo) {
//        double maxVal = 0;
//        double minVal = 0;
//        minMaxLoc(histo, &minVal, &maxVal, 0, 0);

//        Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(255));
//        int hpt = static_cast<int>(0.9*histSize[0]);
//        for(int h = 0; h < histSize[0]; h++) {
//            float binVal = histo.at<float>(h);
//            int intensity = static_cast<int>(binVal*hpt/maxVal);
//            line(histImg, Point(h, histSize[0]),
//                    Point(h, histSize[0]-intensity), Scalar::all(0));
//        }
//        return histImg;
//    }
};
#endif // HIST2DTOOL_H
