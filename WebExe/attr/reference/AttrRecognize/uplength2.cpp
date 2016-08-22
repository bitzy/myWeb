#include "sleeveHeader.h"
#include <cstring>

const int HIST_LENTH = 16*16*16;
typedef struct hsvHistData {
    int hist[HIST_LENTH];
}HSVDATA;

void getImageHist(const Mat& src, const Point& p, int w, vector<HSVDATA>& res);
double cov(int v1[], int v2[]);
//void getSelectPoints(const Mat& src, vector<Point>& res);
void calRectColorHist(const Mat& src, Point p, int w, int h, int rectHist[]);
void calPeriodHistValue(int front, int back, vector<HSVDATA>& hist, int res[]);
double calHistDistByXiangGuanDu(int hist1[], int hist2[], int dim);

bool smallerCompare(const nodeForClothlen& obj1, const nodeForClothlen& obj2) {
    return obj1.dist > obj2.dist;
}

/**
 * @brief upLengAnalyze2
 * @param fpath
 * @param loc
 * @return
 */
int upLengAnalyze2( const char *fpath, MyLocation *loc)
{
    //pre process:
    Mat src = preProcessing(fpath, loc);

    //src hist sum get:
    vector<HSVDATA> srcHist;
    double rectwidth = getEuclideanDist(lshoulder, rshoulder)*2/3;
    Point belly = (lhip + rhip) * 0.5;
    Point start((lshoulder.x*2 + neck.x)/3, (neck.y*2 + belly.y)/3);
    getImageHist(src, start, rectwidth, srcHist);

    //get partial point ==> partialPoints
    vector<nodeForClothlen> nodes;
    int size = srcHist.size();
    for(int i = 10; i < size - 10; i++) {
        int upHist[HIST_LENTH], downHist[HIST_LENTH];
        calPeriodHistValue(i-10, i, srcHist, upHist);
        calPeriodHistValue(i, i+10, srcHist, downHist);
        double relationRate = calHistDistByXiangGuanDu(upHist, downHist, HIST_LENTH);
        nodeForClothlen tmp;
        tmp.num = i;
        tmp.dist = relationRate;
        nodes.push_back(tmp);
    }
    sort(nodes.begin(), nodes.end(), smallerCompare);
    Point partialPoints[20];
    for(int i = 0; i < 20; i++) {
        partialPoints[i].x = start.x;
        partialPoints[i].y = start.y + nodes.at(i).num*5;
    }

    //get select Point
    vector<Point> splitPoint;
    splitPoint.push_back(partialPoints[0]);
    int limit = (int)(((lhip.y + rhip.y)/2.0 - neck.y)/5.0);
    for(int i = 1; i < 20; i++) {
        int mark = 0;
        int size = splitPoint.size();
        for(int j = 0; j < size; j++) {
            if(abs(splitPoint[j].y - partialPoints[i].y) < limit) {
                mark = 1;
                break;
            }
        }
        if(mark == 0) splitPoint.push_back(partialPoints[i]);
    }

    //get color:
    return 0;
}

void getImageHist(const Mat& src, const Point& p, int w, vector<HSVDATA> &res)
{
    int num = (src.rows - p.y)/5;
    assert(num > 0);

    HSVDATA histValue;
    calRectColorHist(src, p, w, 5, histValue.hist);

    Point start = p;
    int tmpHist[HIST_LENTH];
    for(int i = 1; i < num; i++) {
        start.y = start.y + 5;
        calRectColorHist(src, start, w, 5, tmpHist);
        for(int j = 0; j < HIST_LENTH; j++)
            histValue.hist[j] = tmpHist[j] + res.at(i-1).hist[j];
        res.push_back(histValue);
    }

    //in case res.size < 20:
    int size = res.size();
    for(int i = size; i < 20; i++) {
        for(int j = 0; j < HIST_LENTH; j++)
            histValue.hist[j] = tmpHist[j] + res.at(i-1).hist[j];
        res.push_back(histValue);
    }
}

//void getSelectPoints(const Mat &src, vector<Point>& res)
//{
//    const int pNum = 20;
//    Point tmpPoint[pNum];

//    double shoulderWid = getEuclideanDist(lshoulder, rshoulder);
//    int rectWidth = shoulderWid * 2 / 3;
//    calRectColorHist(src, start, rectWidth, 5, histAll[0]);


//    //get color hist for src:
//}

void calRectColorHist(const Mat& src, Point p, int w, int h, int rectHist[])
{
    //RGB ==> HSV
    Mat source = src(Range(p.y, p.y+h), Range(p.x, p.x+w));
    Mat hsvSrc;
    cvtColor(source, hsvSrc, CV_BGR2HSV);

    Mat layers[3];
    split(hsvSrc, layers);

    int hist[16][16][16];
    memset(hist, 0, sizeof(hist));
    int rows = hsvSrc.rows;
    int cols = hsvSrc.cols;
    for(int i = 0; i < rows; i++) {
        uchar* hlayer = layers[0].ptr<uchar>(i);
        uchar* slayer = layers[1].ptr<uchar>(i);
        uchar* vlayer = layers[2].ptr<uchar>(i);
        for(int j = 0; j < cols; j++) {
            int k1 = (hlayer[j] >> 4);
            int k2 = (slayer[j] >> 4);
            int k3 = (vlayer[j] >> 4);
            hist[k1][k2][k3]++;
        }
    }

    int count = 0;
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            for(int k = 0; k < 16; k++)
                rectHist[count++] = hist[i][j][k];
        }
    }
}

void calPeriodHistValue(int front, int back, vector<HSVDATA>& hist, int res[])
{
    for(int i = 0; i < HIST_LENTH; i++) {
        res[i] = hist.at(back).hist[i] - hist.at(front).hist[i];
    }
}

double calHistDistByXiangGuanDu(int hist1[], int hist2[], int dim)
{
    double res = cov(hist1, hist2, dim) /
            sqrt(cov(hist1, hist1, dim) * cov(hist2, hist2, dim));
    return 1 - res;
}
