#include "sleeveHeader.h"

extern "C" {
#include <vl/slic.h>
#include <vl/mathop.h>
}
#include "imagesp.h"

// -------------------------- function -------------------------------------------
void useGrabCut(const Mat &src, Mat &resForeground);
void useGrabCut(const Mat &src, Mat &mask, Mat &resForeground);
void useSegment(const Mat &src, Mat &resSeg);
void getSegResImage(const Mat &seg, int w, int h, const char* outName);
// -------------------------- over ----------------------------------------------

int myNewWaySleeve(const char * fpath, MyLocation* loc)
{
    int HSTYLE;
    bool leftFlag;
    Mat curImage = sleeveProcessing(fpath, loc, HSTYLE, leftFlag);

    //get segment image;
    Mat seg2D;//CV_32SC1
    useSegment(curImage, seg2D);
    //    if(system_debug) { getSegResImage(seg1D, scaledImg.cols, scaledImg.rows, "countour.jpg");}

    //get image sp's feature:
    SPIMAGE imgSPfeat(curImage, seg2D);
    imgSPfeat.setColorByHistomForEachSp();
    //    imgSPfeat.setColorByCenterForEachSp();
    //      if(system_debug) imgSPfeat.getSpColorImage("spColor.jpg");

    double stdPercent;
    getStdPercent(fpath, stdPercent);
    stdPercent *= 10;

    int lTercent, rTercent;
    double lconf = imgSPfeat.getSleeveTercent(lshoulder, lelbow, lhand, lTercent, "left");
    double rconf = imgSPfeat.getSleeveTercent(rshoulder, relbow, rhand, rTercent, "right");
    //test percent , test confidence , standard percent.
    cout << lTercent << "," << lconf << "," << stdPercent << endl;
    cout << rTercent << "," << rconf << "," << stdPercent << endl;

    if(rconf > lconf) {
        return rTercent;
    } else {
        return lTercent;
    }
}



void useGrabCut(const Mat &src, Mat &resForeground)
{
    Rect area(0,0, src.cols-1, src.rows-1);

    Mat resMask, bgModel, fgModel;
    grabCut(src, resMask, area, bgModel, fgModel, 1, cv::GC_INIT_WITH_RECT);
    resMask = resMask&1;
    src.copyTo(resForeground, resMask);//just copy the element in resMask value is 1
}

void useGrabCut(const Mat &src, Mat &mask, Mat &resForeground)
{
    Rect area(0, 0, src.cols-1, src.rows-1);

    Mat bgModel, fgModel;
    grabCut(src, mask, area, bgModel, fgModel, 1, cv::GC_INIT_WITH_MASK);
    mask = mask&1;
    src.copyTo(resForeground, mask);
}

float* _matToLine(const Mat& src)
{
    int width = src.cols;
    int height = src.rows;
    int channel = src.channels();
    float * image = new float[width * height * src.channels()];
    if(channel == 1) {
        //gray img;
        for(int i = 0, pos = 0; i < height; i++ ){
            for(int j = 0; j < width; j++) {
                image[pos] = (float)src.at<uchar>(i,j);
                pos++;
            }
        }
    } else {
        //color img;
        int offsetB = 0;
        int offsetG = height * width;
        int offsetR = offsetG << 1;
        for(int i = 0, pos = 0; i < height; i++ ){
            for(int j = 0; j < width; j++) {
                image[offsetB + pos] = (float)src.at<Vec3b>(i,j)[0];
                image[offsetG + pos] = (float)src.at<Vec3b>(i,j)[1];
                image[offsetR + pos] = (float)src.at<Vec3b>(i,j)[2];
                pos++;
            }
        }
    }
    return image;
}

void useSegment(const Mat &src, Mat &resSeg)
{
    float * imageLine = _matToLine(src);
    int w = src.cols;
    int h = src.rows;
    int size = w*h;

    vl_uint32* segmentation_res = new vl_uint32[size];
    vl_size vl_height = h;
    vl_size vl_width = w;
    vl_size vl_channels = 3;

    vl_size region = 30;    //nominal size of the regions.
    float regularization = 1000;//trade-off between appearance and spatial terms.
    vl_size minRegion = 20; //minimum size of a segment.
    vl_slic_segment(segmentation_res, imageLine, vl_width, vl_height, vl_channels,
                    region, //regionSize
                    regularization, //regularization
                    minRegion); //minRegionSize

    delete(imageLine);

    resSeg.create(h, w, CV_32SC1);
    int count = 0;
    for(int i = 0; i < h; i++) {
        int* ptr = resSeg.ptr<int>(i);
        for(int j = 0; j < w; j++) {
            ptr[j] = int(segmentation_res[count++]);
        }
    }
}


void getSegResImage(const Mat &seg, int w, int h, const char *outName)
{
    int** labels = new int*[h];
    for (int i = 0; i < h; ++i) {
        labels[i] = new int[w];
        for (int j = 0; j < w; ++j) {
            labels[i][j] = seg.at<ushort>(j + w*i);
        }
    }

    //get result figure:
    int label = 0;
    int labelTop = -1;
    int labelBottom = -1;
    int labelLeft = -1;
    int labelRight = -1;
    Mat srcRes(Size(w, h), CV_8UC3, Scalar::all(0));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            label = labels[i][j];
            labelTop = (i > 0? labels[i-1][j]:label);
            labelBottom = (i < h-1? labels[i+1][j]:label);
            labelLeft = (j > 0? labels[i][j-1]:label);
            labelRight = (j < w-1? labels[i][j+1]:label);
            if (label != labelTop || label != labelBottom ||
                    label!= labelLeft || label != labelRight) {
                srcRes.at<cv::Vec3b>(i, j)[0] = 0;
                srcRes.at<cv::Vec3b>(i, j)[1] = 0;
                srcRes.at<cv::Vec3b>(i, j)[2] = 255;
            }
        }
    }
    for(int i = 0; i < h; i++) {
        delete[] labels[i];
    }
    delete[] labels;
    imwrite(outName, srcRes);
}
