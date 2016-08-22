#include <QDebug>
#include "attrRecognize.h"
#include "sleeveHeader.h"

#define POSE_NECK "neck"
#define POSE_LSHOULDER "left_shoulder"
#define POSE_RSHOULDER "right_shoulder"
#define POSE_LHIP "left_hip"
#define POSE_RHIP "right_hip"

#define HLEVELSIZE 16
#define SLEVELSIZE 16
#define VLEVELSIZE 16
#define HISTHIGHT 500

int histlen = HLEVELSIZE * SLEVELSIZE * VLEVELSIZE;
int histAll[HISTHIGHT][HLEVELSIZE*SLEVELSIZE*VLEVELSIZE]={0};
QString UplengthTestPath = SYSTEST_DIR;/*QString("%1Uplength/").arg(SYSTEST_DIR)*/

void getColorHistAll(cv::Mat src, cv::Point start, const int rectwidth,int &histAllSize);
void getpartialhist(const int h1, const int h2, int histTmp[]);
int compForClothlen(const void*a, const void*b);
double calHistDistByXiangguandu(int a1[], int a2[]);
void saveImage(cv::Mat& img, const char* fNoExt, const char* fExt);
void selectPoint(Point twentyPRes[20], const int ylenThre, vector<Point> &res);
void getMeanColor(ColorFeat2 colFeature[5], const int size, int rgbRes[]);
void getRigionColorForClothlen(Mat colorRigion,ColorFeat2 colfeature[]);
//////////////////////////////////////////////////////////
/// \brief upLength
/// \param fpath
/// \param loc
/// \return
///
int upLengAnalyze(const char * fpath, MyLocation* loc) {

    //string fpathStr(fpath);
    //Mat src = imread(fpathStr);
    Mat src = preProcessing(fpath, loc);
    Mat src2;

    //Point neck = loc->getPointByName(POSE_NECK);
    //Point lshoulder = loc->getPointByName(POSE_LSHOULDER);
    //Point rshoulder = loc->getPointByName(POSE_RSHOULDER);
    //Point lhip = loc->getPointByName(POSE_LHIP);
    //Point rhip = loc->getPointByName(POSE_RHIP);

    //get 20 lines & max diff in the src image;
    Point twentyPRes[20];
    //getTwentyPoint(src, loc, savefPathNoExt, twentyPRes);
    // function -- getTwentyPoint
    {
        //get rectwidth:
        double shoulderwidth = getEuclideanDist(lshoulder,rshoulder);
        int rectwidth = shoulderwidth*2/3;

        //get start point:
        Point belly((lhip.x+rhip.x)/2,(lhip.y+rhip.y)/2);
        Point start((lshoulder.x*2+neck.x)/3,(neck.y*2+belly.y)/3);

        //get color hist all:
        int histallsize = 0;
        getColorHistAll(src, start, rectwidth, histallsize);

        //get twenty point:
        nodeForClothlen record[500];
        for(int j=10; j < histallsize-10; j++) {
            int uperhist[HLEVELSIZE*SLEVELSIZE*VLEVELSIZE]={0};
            int downhist[HLEVELSIZE*SLEVELSIZE*VLEVELSIZE]={0};
            getpartialhist(j-10, j, uperhist);
            getpartialhist(j, j+10, downhist);
            double tmpdist = calHistDistByXiangguandu(uperhist,downhist);
            record[j].num = j;
            record[j].dist = tmpdist;
        }
        qsort(record, histallsize, sizeof(record[0]), compForClothlen);
        Point end[20];
        for (int i = 0; i < 20; i++) {
            end[i].x = start.x;
            end[i].y = start.y + record[i].num*5;
            twentyPRes[i] = end[i];
        }

        /*
#ifdef SYS_OUTPUT_MIDRES
            //output the 20 lines as image:
            Mat src2;
            src.copyTo(src2);
            rectangle(src2, start, cv::Point(start.x + rectwidth, src2.rows), cv::Scalar(0,0,0), 2);
            for(int k = 0; k < 20; k++) {
                cv::line(src2, cv::Point(end[k].x,end[k].y), cv::Point(end[k].x + rectwidth, end[k].y), cv::Scalar(255,255,255), 2);
            }
            saveImage(src2, "timg", "clothlen_20lines");

            //output the max image:
            cv::line(src2, cv::Point(end[0].x,end[0].y), cv::Point(end[0].x+rectwidth, end[0].y), cv::Scalar(0, 0, 0),2);
            char text[100];
            sprintf(text,"%lf",record[0].dist);
            putText(src2, text, end[0], CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));//show value on src2
            rectangle(src2,cv::Point(end[0].x,end[0].y-5*8),cv::Point(end[0].x+rectwidth,end[0].y+5*8),cv::Scalar(0,0,0),2);
            saveImage(src2, "timg", "clothlen_maxdiff");
#endif
*/
    }// function over

    //get selectPoint:
    int ylenthre = ( (lhip.y + rhip.y)/2 - neck.y )/5;
    vector<Point> selectPointRes;
    selectPoint(twentyPRes, ylenthre, selectPointRes);

    int pointResCount = selectPointRes.size();
    /*
#ifdef SYS_OUTPUT_MIDRES
        src.copyTo(src2);//output;
        for(int k=0; k < pointResCount; k++) {
            cv::line(src2, selectPointRes[k],
                     cv::Point(src2.cols,selectPointRes[k].y),
                     cv::Scalar(255, 255, 255),2);
        }
        saveImage(src2, "timg", "selectLine");
#endif
*/

    //get clothmaincolor:
    ColorFeat2 clothMainColor[5];
    getClothesMainColor(src, loc, clothMainColor);
    //get main color:
    int clothesMainColorMean[3];
    getMeanColor(clothMainColor, 5, clothesMainColorMean);

    //judge selectPoint:
    vector<int> lenresUp;
    vector<int> lenresDown;
    for(int i = 0; i < pointResCount; i++) {//each line;
        myRectangle rectup,rectdown;
        //getJudgeRigion(src, loc, selectPointRes[i].y, rectup, rectdown);
        // function -- getJudgeRegion
        {
            int rectwidth = (rhip.x - lhip.x)*4/5;
            int rectXcenter = (rhip.x + lhip.x)/2;
            int rectheight = ( (lhip.y + rhip.y)/2 - neck.y )/4;

            rectup.start.x = rectXcenter-rectwidth/2;
            rectup.start.y = selectPointRes[i].y - rectheight;
            rectup.end.x = rectup.start.x +rectwidth;
            rectup.end.y = selectPointRes[i].y;
            if(src.rows - selectPointRes[i].y > rectheight) {
                rectdown.start.x = rectup.start.x;
                rectdown.start.y = selectPointRes[i].y;
                rectdown.end.x = rectup.end.x;
                rectdown.end.y = selectPointRes[i].y+rectheight;
            }
        }//function over

        //rectup area color:
        ColorFeat2 colFeatureUp[5];
        //reverse point-byzy:
        int startx = rectup.start.x;
        int starty = rectup.start.y;
        int endx = rectup.end.x;
        int endy = rectup.end.y;
        if(starty > endy) {
            endy = rectup.start.y;
            starty = rectup.end.y;
        }
        if(startx > endx) {
            startx = rectup.end.x;
            endx = rectup.start.x;
        }

        //Mat upColorRigion = src(Range(rectup.start.y,rectup.end.y),Range(rectup.start.x,rectup.end.x));
        Mat upColorRigion = src(Range(starty, endy),Range(startx, endx));
        getRigionColorForClothlen(upColorRigion,colFeatureUp);

        //rectup area color dist with main color:
        int upColorMean[3];
        getMeanColor(colFeatureUp, 5, upColorMean);
        double colorFeatDistUp = colorDist(upColorMean, clothesMainColorMean);
        lenresUp.push_back(colorFeatDistUp);

        //rectdown area color:
        if(rectdown.end.x > 0 && rectdown.end.y > 0) {
            ColorFeat2 colFeatureDown[5];

            //reverse point-byzy:
            int startx = rectdown.start.x;
            int starty = rectdown.start.y;
            int endx = rectdown.end.x;
            int endy = rectdown.end.y;
            if(starty > endy) {
                endy = rectdown.start.y;
                starty = rectdown.end.y;
            }
            if(startx > endx) {
                startx = rectdown.end.x;
                endx = rectdown.start.x;
            }

            Mat downColorRigion = src(Range(starty, endy),Range(startx, endx));
            getRigionColorForClothlen(downColorRigion, colFeatureDown);

            int downColorMean[3];
            getMeanColor(colFeatureDown, 5, downColorMean);
            double colorFeatDistDown = colorDist(downColorMean, clothesMainColorMean);
            lenresDown.push_back(colorFeatDistDown);
        } else {
            lenresDown.push_back(-1);
        }

        /*
#ifdef SYS_OUTPUT_MIDRES
            //output:
            src.copyTo(src2);
            rectangle(src2, rectup.start, rectup.end, cv::Scalar(0,0,0), 2);
            char text[30];
            sprintf(text, "%d", lenresUp[i]);
            putText(src2, text, rectup.start, CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255) );
            rectangle(src2, rectdown.start, rectdown.end, cv::Scalar(0,0,0),2);
            sprintf(text, "%d", lenresDown[i]);
            putText(src2, text, rectdown.end, CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255) );
            sprintf(text, "selectLine%d", i);
            saveImage(src2, "timg", text);
#endif
*/
    }

    //choose the point P:
    Point P;
    int mark = 0;
    int tmpLenresUpSize = lenresUp.size();
    for(int i=0; i < tmpLenresUpSize; i++) {
        if(-1 < lenresDown[i]) {
            if( (lenresDown[i] > 10000 && lenresUp[i] < 8000) ||
                    (lenresUp[i] > 0 && lenresDown[i] > 5000 && lenresDown[i] < 10000 && lenresDown[i]/lenresUp[i]>5) ||
                    (lenresUp[i] > 0 && lenresDown[i] > 3000 && lenresDown[i]/lenresUp[i]>15) ||
                    (lenresUp[i] > 0 && lenresDown[i]/lenresUp[i] > 20)) {
                P = selectPointRes[i];
                mark=1;
                break;
            }
        }
    }
    if(0 == mark) {
        P = selectPointRes[0];
    }

    //calculate the cloth len:
    /*
    Point clothBottom;
    clothBottom.x = (lhip.x + rhip.x)/2;
    clothBottom.y = P.y;
    int flagLength = P.y;
    Point hip = (lhip+rhip)*0.5;
    int clothlenres = int(getEuclideanDist(neck, clothBottom)/getEuclideanDist(neck, hip)*100);
    */
    int flagLenth = P.y;
    double res;
    if(lknee.x == -1) {
        Point hip = (lhip+rhip)*0.5;
        res = (flagLenth - neck.y)*400/getEuclideanDist(neck, hip);
    } else if(lankle.x == -1) {
        Point knee = (lknee+rknee)*0.5;
        res = (flagLenth - neck.y)*700/getEuclideanDist(neck, knee);
    } else {
        Point ankle = (lankle+rankle)*0.5;
        res = (flagLenth - neck.y)*1000/getEuclideanDist(neck, ankle);
    }
    int clothlenres = (int)res;
    if(clothlenres > 1000) clothlenres = 1000;
    /*
#ifdef SYS_OUTPUT_MIDRES
        //output res:
        src.copyTo(src2);
        cv::line(src2, cv::Point(0,P.y), cv::Point(src2.cols,P.y), cv::Scalar(0,0,0), 2);
        saveImage(src2, "timg", "res");
#endif

    if(clothlenres < 5) {
        return 0; //short;
    } else if(clothlenres < 45) {
        return 1;//normal;
    } else if(clothlenres < 85) {
        return 2;//mid_long;
    } else {
        return 3;//long;
    }*/

    return clothlenres;
}

void getColorHist(cv::Mat src,cv::Point p,const int rectwidth,const int rectheight,int resHist[]);
void clearHist(int hist[HLEVELSIZE][SLEVELSIZE][VLEVELSIZE]);
void clearHist(int hist[]);
void getColorHistAll(cv::Mat src, cv::Point start, const int rectwidth,int &histAllSize) {

    // the first rect:
    getColorHist(src, start, rectwidth, 5, histAll[0]);

    histAllSize = (src.rows - start.y)/5;
    Point iter = start;
    int histTmp[HLEVELSIZE * SLEVELSIZE * VLEVELSIZE]={0};
    for(int i=1; i < histAllSize; i++) {
        iter.y = iter.y + 5;
        getColorHist(src, iter, rectwidth, 5, histTmp);
        for(int j=0; j < histlen; j++) {
            histAll[i][j] = histTmp[j] + histAll[i-1][j];
        }
    }

    for(int i = histAllSize; i < 20; i++) {//??? why is < 20
        for(int j=0; j < histlen; j++) {
            histAll[i][j] = histTmp[j] + histAll[i-1][j];
        }
    }
}

void getColorHist(cv::Mat src,cv::Point p,const int rectwidth,const int rectheight,int resHist[]) {
    //get h/s/v channel:
    cv::Mat source= src(cv::Range(p.y, p.y+rectheight),cv::Range(p.x, p.x+rectwidth));
    cv::Mat hsv;
    cvtColor(source,hsv,CV_BGR2HSV);
    cv::Mat layers[3];
    cv::split(hsv,layers);

    int hist[HLEVELSIZE][SLEVELSIZE][VLEVELSIZE]={0};
    clearHist(hist);
    int tmpRows = hsv.rows;
    int tmpCols = hsv.cols;
    for(int i=0; i < tmpRows; i++) {
        uchar *hlayer = layers[0].ptr<uchar>(i);
        uchar *slayer = layers[1].ptr<uchar>(i);
        uchar *vlayer = layers[2].ptr<uchar>(i);
        for(int j=0; j < tmpCols; j++) {
            int k1 = hlayer[j]/16;//int k1 = hlayer[j]/12;
            int k2 = slayer[j]/16;
            int k3 = vlayer[j]/16;
            hist[k1][k2][k3]++;
        }
    }

    int count = 0;
    clearHist(resHist);
    for(int i=0; i < HLEVELSIZE; i++) {
        for(int j=0; j < SLEVELSIZE; j++) {
            for(int k=0; k < VLEVELSIZE; k++) {
                resHist[count++] = hist[i][j][k];
            }
        }
    }
}

void clearHist(int hist[HLEVELSIZE][SLEVELSIZE][VLEVELSIZE]) {
    //three dimension:
    for(int i=0; i < HLEVELSIZE; i++) {
        for(int j=0; j < SLEVELSIZE; j++) {
            for(int k=0; k < VLEVELSIZE; k++) {
                hist[i][j][k]=0;
            }
        }
    }
}

void clearHist(int hist[]) {
    //single dimension:
    for(int i=0; i < HLEVELSIZE*SLEVELSIZE*VLEVELSIZE; i++) {
        hist[i] = 0;
    }
}

void getpartialhist(const int h1, const int h2, int histTmp[]) {
    clearHist(histTmp);
    for(int i=0; i< histlen; i++) {
        histTmp[i] = histAll[h2][i] - histAll[h1][i];
    }
}

double mean(int v[]);
double cov(int v1[], int v2[]);
double calHistDistByXiangguandu(int a1[], int a2[]) {
    double xiangguandu = cov(a1, a2) / sqrt(cov(a1, a1) * cov(a2, a2));
    return 1 - xiangguandu;
}

double cov(int v1[], int v2[]) {
    // assert(v1.size() == v2.size() && v1.size() > 1);
    double ret = 0.0;
    double v1a = mean(v1), v2a = mean(v2);

    for (int i = 0; i <histlen; i++) {
        ret += (v1[i]*1.0 - v1a) * (v2[i]*1.0 - v2a);
    }

    return ret / (histlen - 1);
}

double mean(int v[]) {
    double ret = 0.0;
    for (int i = 0; i < histlen; i++) {
        ret += v[i];
    }
    return ret / histlen;
}


int compForClothlen(const void*a, const void*b) {
    return (*(nodeForClothlen *)b).dist > (*(nodeForClothlen *)a).dist ? 1 : -1;
}

void saveImage(cv::Mat& img, const char* fNoExt, const char* fExt) {
    QString saveFileName = QString("%1%2_%3.jpg").arg(UplengthTestPath).arg(fNoExt).arg(fExt);
    imwrite(saveFileName.toStdString().c_str(), img);
}

void selectPoint(Point twentyPRes[20], const int ylenThre, vector<Point> &res) {
    int mark;

    res.push_back(twentyPRes[0]);
    for(int i = 1; i < 20; i++) {
        mark = 0;
        int resSize = res.size();
        for(int j = 0; j < resSize; j++) {
            if(abs(res[j].y - twentyPRes[i].y) < ylenThre) {
                mark = 1;
                break;
            }
        }
        if(0 == mark) res.push_back(twentyPRes[i]);
    }
}

void getMeanColor(ColorFeat2 colFeature[5], const int size, int rgbRes[]) {
    int sumR=0, sumG=0, sumB=0, sum=0;
    for(int i=0; ((i < size) && (colFeature[i].per > 0)); i++) {
        sumR +=colFeature[i].rgb[0] * colFeature[i].per;
        sumG +=colFeature[i].rgb[1] * colFeature[i].per;
        sumB +=colFeature[i].rgb[2] * colFeature[i].per;
        sum+=colFeature[i].per;
    }
    rgbRes[0] = sumR/sum;
    rgbRes[1] = sumG/sum;
    rgbRes[2] = sumB/sum;
}

void getRigionColorForClothlen(Mat colorRigion,ColorFeat2 colfeature[]) {
    ColorFeat colorFeatureTmp[5];
    Mat colorRigionTmp[2] = {colorRigion};
    getClothColorFeatureByCount(colorRigionTmp, 1, colorFeatureTmp, 3000);

    for(int i=0; i<5; i++) {
        colfeature[i].per = colorFeatureTmp[i].per;
        unsigned int rgb = colorFeatureTmp[i].rgb;
        colfeature[i].rgb[2] = ( rgb& 0x003f) << 2;//b
        colfeature[i].rgb[1] = ((rgb >> 6) & 0x003f )<< 2;//g
        colfeature[i].rgb[0] = ((rgb>> 12) & 0x003f) << 2;//r
    }
}
