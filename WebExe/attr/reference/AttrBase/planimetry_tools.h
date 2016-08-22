#ifndef PLANIMETRY_TOOLS
#define PLANIMETRY_TOOLS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <cmath>
#include <iostream>
#include <cstdio>
#include <ctime>
#include "svmpredict.h"

typedef struct node {
	float confidence;
	cv::Point point;
    node() {
		confidence = 0;
		point.x = 0;
        point.y = 0;
	}
}node;

typedef struct pairNode
{
    cv::Point pl, pr;
    float confidence1,confidence2,confidencesum;
    pairNode() {
        pl.x = 0;
        pl.y = 0;
        pr.x = 0;
        pr.y = 0;
        confidence1 = 0;
        confidence2 = 0;
        confidencesum = 0;
    }
}pairNode;


typedef struct myRectangle {
	cv::Point start;
	cv::Point end;
    myRectangle() {
        start.x = 0;
		start.y = 0;
		end.x = 0;
        end.y = 0;
	}
} myRectangle;


typedef struct ColorFeat {
    unsigned int rgb;
    long long per;
	ColorFeat()	{
        rgb = 0;
        per = 0;
	}
} ColorFeat;


typedef struct ColorFeat2 {
    uchar rgb[3];
    long long per;
    ColorFeat2() {
		rgb[0] = 0;
        rgb[1] = 0;
        rgb[2] = 0;
        per = 0;
	}
} ColorFeat2;

double getEuclideanDist(double p1x, double p1y, double p2x, double p2y);
double getEuclideanDist(cv::Point p1, cv::Point p2);
double getEuclideanDist(cv::Point3d p1, cv::Point3d p2);
double getAngleByThreeP(cv::Point p1, cv::Point p2, cv::Point p3);
double getAngle180ByThreeP(cv::Point p1, cv::Point p2, cv::Point p3);
double getAngleByThreeP(double pointx[3], double pointy[3]);
double getAngleByThreP2(cv::Point p1, cv::Point p2, cv::Point p3);
double getVectorGrad(cv::Point p1, cv::Point p2);
double getVectorGrad(double p1x, double p1y, double p2x, double p2y);
double getDistFromP2L(cv::Point p1, cv::Point p2, cv::Point p3);
double getDistFromP2L(cv::Point p1, cv::Point p2, double angle);
double getDistFromP2L(double px, double py, double p1x, double p1y, double p2x, double p2y);
void getCircleFrom3P(cv::Point p1, cv::Point p2, cv::Point p3, double &r , cv::Point & cir_O);
void getCircleFrom3P(double *x, double *y, double &r, double &x0, double &y0);
double getTheta(cv::Point p1, cv::Point p2);
double getAngleL1toL2(cv::Point p1, cv::Point p2, cv::Point p3);
double getInclinationAngle(cv::Point p1, cv::Point p2);
double getVectMultiply(cv::Point p1,cv::Point p2,cv::Point p3);
double getVectp1p2Multip1p3(cv::Point p1,cv::Point p2,cv::Point p3);
cv::Point getP1Projection(cv::Point p1,cv::Point P2,cv::Point p3);
double getAngleByFourP(cv::Point p1,cv::Point p2,cv::Point p3,cv::Point p4);

std::vector<float> calHog(cv::Mat src);
std::vector<float> calHog2(cv::Mat src);
void getFileName(char fullFilePath[],char filename[]);
void showSingleLine(cv::Mat &src,cv::Point start,cv::Point end,const char saveFilePath[]);
void showRectInImage2(cv::Mat src,cv::Point points[],int pointsSize,
                      int rectsize,float confidence[], const char saveFilePath[]);
void getMaxByThreeValue(int a[3],int &k);
void getMaxByThreeValue(double a[3],int &k);
void getMaxByThreeValue(double a0,double a1,double a2,int &k);
void getMinByThreeValue(int a0,int a1,int a2,int &k);

int colorDist(int i, int j);
int colorDist(int rgb1[],int rgb2[]);
int colorDist( int r1,int r2,int g1,int g2,int b1,int b2);
int colorDist(uchar rgb1[],uchar rgb2[]);
int colorDist( uchar r1,uchar r2,uchar g1,uchar g2,uchar b1,uchar b2);

double getColorFeatureDist(ColorFeat2 feat1[],ColorFeat2 feat2[],const int featsize);

void imageRGB2HSI(const cv::Mat& image, cv::Mat &hsi);
int getLinePoint(const cv::Point& start, const cv::Point& end, vector<cv::Point> &res);
bool vectorHasExist(const vector<int>& vec, int x);
int vectorCountFindNxt(vector<int>& vec, int& idx, int& pos);
#endif
