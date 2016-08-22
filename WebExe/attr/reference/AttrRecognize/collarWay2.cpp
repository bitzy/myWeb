#include "attrRecognize.h"
#define RSEARCH_LINE 5

/*********************************************************************************************
 * Collar recognize program.
 * Revise from Hanzhitian program.By zhangyan 2016.5.20.
 *
 * This program can recognize 4 kinds of collar:
 *  Round/Vstyle/Fclose/Fopen.
 *
 * Part-based way to recognize collar.
 *
* ********************************************************************************************/
void loadModelData();

void RoundCollar_collarW2( const Mat &src, float &roundCollarConf);
void VstyleCollar_collarW2(const Mat &src, float &vstyleCollarConf);
void FcloseCollar_collarW2(const Mat &src, float &fcloseCollarConf);
void FstyleCollar_collarW2(const Mat &src, float &fstyleCollarConf);
int SVM_function(float* confidence);

float getPointConf_Round( const Mat &src, int rectsize, Point p, int mark);
float getPointConf_Vstyle(const Mat &src, int rectsize, Point p, int mark);
float getPointConf_Fstyle(const Mat &src, int rectsize, Point p, int mark);
void expandFromP_Round(const Mat &src, Point p, float startAngle, float endAngle,
                       float minWid, float maxWid, int rectsize, node& res, int modelMask);
QString saveFile_collarWay2;
///
/// \brief testWayCollar
/// \param fpath ==> deal image path;
/// \param loc ==> pose data of image;
/// \return ==> int value denote the collar category.
///
int testWayCollar(const char * fpath, MyLocation* loc){
    printf("testWayCollar start:\n\tDealing %s\n\n", fpath);

    Mat srcImg = imread(fpath);
    loadPoseData(loc);
    loadModelData();

    float confidence[COLLAR_TYPE];
    if(system_debug) cout << "getting confidence for each collar..." << endl;

    RoundCollar_collarW2( srcImg, confidence[0]);    
    if(system_debug)
        cout << "RoundCollar confidence get." << confidence[0] << endl;
    VstyleCollar_collarW2(srcImg, confidence[1]);
    if(system_debug)
        cout << "VstyleCollar confidence get."<< confidence[1] << endl;
    FcloseCollar_collarW2(srcImg, confidence[2]);
    if(system_debug)
        cout << "FcloseCollar confidence get."<< confidence[2] << endl;
    FstyleCollar_collarW2(srcImg, confidence[3]);
    if(system_debug)
        cout << "FopenCollar confidence get." << confidence[3] << endl;

    int resIndex = SVM_function(confidence);
    printf("testWayCollar end:\n\tDeal over.%s...OK!\n\n", fpath);    
    cout << resIndex << endl;
    return resIndex;
}

int SVM_function(float* confidence)
{
    int resIndex;

    //resize confidence:
    float afterZoom[COLLAR_TYPE];
    char modelFileForCollarRange[300] = {"./Model/ModelForRes/collarRes2.txt.dat.range"};
    FILE *fp = fopen(modelFileForCollarRange,"r");
    char tmp[20];
    fgets(tmp,20,fp);
    int left, right;
    fscanf(fp,"%d %d",&left,&right);

    float min[COLLAR_TYPE],max[COLLAR_TYPE];
    for(int i=0; i < COLLAR_TYPE; i++) {
        fscanf(fp,"%*d %f %f", &min[i],&max[i]);
    }
    fclose(fp);
    for(int i=0; i < COLLAR_TYPE; i++) {
        afterZoom[i]=left + (confidence[i]-min[i])*2.0/(max[i]-min[i]);
    }


    //tmp.txt: 0 1:x1 2:x2 3:x4
    fp = fopen("tmp.txt", "w+");
    fprintf(fp, "0 ");
    for(int i = 0; i < COLLAR_TYPE; i++) {
        fprintf(fp, "%d:%f ", i+1, afterZoom[i]);
    }
    fclose(fp);

    char cmd[300];
    char modelFileForCollarRes[300] = {"./Model/ModelForRes/collarRes2.txt.dat.model"};
    sprintf(cmd, "./svm-predict-linux tmp.txt %s tmp.txt.res > tempToThrow.txt",
            modelFileForCollarRes);
    system(cmd);

    fp = fopen("tmp.txt.res", "r");
    fscanf(fp, "%d", &resIndex);
    fclose(fp);

    return resIndex;
}

///
/// \brief getRoundCollar:
///     get src's Round Collar confidence data.
/// \param src
/// \param conf: float[SEARCH_LINE];
///
void RoundCollar_collarW2(const Mat& src, float& roundCollarConf)
{
    Mat src2;

    //get area:
    int rectsize = (rshoulder.x - lshoulder.x)/5; cout << "rectsize:" << rectsize << endl;
    Point pStart, pEnd;
    {
        pStart = neck;
        float pStarty;
        {
            float len = getEuclideanDist(lshoulder, rshoulder);
            pStarty = (lshoulder.y < rshoulder.y ? lshoulder.y:rshoulder.y);
            pStarty -= (len/8);
        }
        pStart.y = pStarty;
        Point hipCenterDot = (lhip + rhip)*0.5;
        pEnd = hipCenterDot*(5.0/16) + neck*(11.0/16);
    }
    if(system_debug){
        src.copyTo(src2);
        line(src2, pStart, pEnd, Scalar(0,255,255));
        saveFile_collarWay2 = QString("RoundCollar_1searchLine.jpg");
        imwrite(saveFile_collarWay2.toStdString(), src2);
    }

    //get Topconfidence in the (pStart, poEnd) direction:
    const int TOPNum = 6;
    node topPNode[TOPNum];
    {
        //===save:
        if(system_debug) {
            src.copyTo(src2);
        }

        float ppDist = getEuclideanDist(pStart, pEnd);
        int ppSteps = rectsize / 4;
        int ppNum = ppDist / ppSteps;

        //===save:
        if(system_debug){
            line(src2, pStart, pEnd, Scalar(0,255,255));
        }

        Point p;
        vector<node> confTmp;
        for(int i = 0; i <= ppNum; i++) {
            p = pStart*(i*1.0/ppNum) + pEnd*((ppNum-i)*1.0/ppNum);
            node tmp;
            tmp.confidence = getPointConf_Round(src, rectsize, p, 2);
            tmp.point = p;
            confTmp.push_back(tmp);
        }
        sort(confTmp.begin(), confTmp.end(), sortDescNodeByConf);
        for(int i = 0; i < TOPNum; i++) {
            topPNode[i] = confTmp[i];
            //==save:
            if(system_debug) {
                circle(src2, topPNode[i].point, 1, Scalar(255,0,0));
                char text[10];
                sprintf(text, "p%d", i+1);
                putText(src2, text, topPNode[i].point, CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,255,0));
            }
        }
        //==save:
        if(system_debug){
            saveFile_collarWay2 = QString("RoundCollar_2Topnode.jpg");
            imwrite(saveFile_collarWay2.toStdString(), src2);
        }
    }


    //get TopAngle confidence on each angle:
    node resConf[TOPNum][RSEARCH_LINE];
    {
        float angle = getAngleByFourP(Point(0,0), Point(5, 0), pStart, pEnd);
        float angle2 = angle + CV_PI/9;
        float angle1 = angle2+ CV_PI/9;
        float angle4 = angle - CV_PI/9;
        float angle5 = angle4- CV_PI/9;
        float angleOffset = 8*CV_PI/180;

        for(int i = 0; i < TOPNum; i++) {//for each point center:
            Point centerP = topPNode[i].point;
            Point focus = pStart;
            int tmpMinDistThresh = getEuclideanDist(lshoulder, rshoulder)/8;
            if(focus.y > centerP.y - tmpMinDistThresh) {
                focus.y = centerP.y - tmpMinDistThresh;
            }

            //==save:
            char text[10];
            if(system_debug) {
                src.copyTo(src2);
                line(src2, pStart, pEnd, Scalar(0,0,255));
            }

            resConf[i][2] = topPNode[i];
            //==save:
            if(system_debug) {
                circle(src2, resConf[i][2].point, 2, Scalar(0,255,0));
                circle(src2, focus, 2, Scalar(255,255,255));
                sprintf(text, "%.2f", resConf[i][2].confidence);
                putText(src2, text, resConf[i][2].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
            }

            node res;
            //left: [1]            
            float minimumDist = getDistFromP2L(resConf[i][2].point, focus, angle2 - angleOffset);
            float maximumDist = getDistFromP2L(resConf[i][2].point, focus, angle2 + angleOffset);
            expandFromP_Round(src, resConf[i][2].point, CV_PI, CV_PI*7/6, minimumDist,
                    maximumDist, rectsize, res, 1);
            resConf[i][1] = res;
            //==save:
            if(system_debug) {
                line(src2, focus, resConf[i][1].point, Scalar(0,255,255));
                circle(src2, resConf[i][1].point, 2, Scalar(0,255,0));
                sprintf(text, "%.2f", resConf[i][1].confidence);
                putText(src2, text, resConf[i][1].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
            }

            //left: [0]            
            minimumDist = getDistFromP2L(resConf[i][1].point, focus, angle1 - angleOffset);
            maximumDist = getDistFromP2L(resConf[i][1].point, focus, angle1 + angleOffset);
            float startAngle = CV_PI*2 - getAngleByFourP(
                        Point(0,0), Point(5,0),resConf[i][2].point, resConf[i][1].point);
            float endAngle = startAngle + CV_PI/6;            
            expandFromP_Round(src, resConf[i][1].point, startAngle, endAngle, minimumDist,
                    maximumDist, rectsize, res, 0);
            resConf[i][0] = res;
            //==save:
            if(system_debug) {
                line(src2, focus, resConf[i][0].point, Scalar(0,255,255));
                circle(src2, resConf[i][0].point, 2, Scalar(0,255,0));
                sprintf(text, "%.2f", resConf[i][0].confidence);
                putText(src2, text, resConf[i][0].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
            }

            //right: [3]            
            minimumDist = getDistFromP2L(resConf[i][2].point, focus, angle4 + angleOffset);
            maximumDist = getDistFromP2L(resConf[i][2].point, focus, angle4 - angleOffset);
            expandFromP_Round(src, resConf[i][2].point, -CV_PI/6, 0, minimumDist,
                    maximumDist, rectsize, res, 3);
            resConf[i][3] = res;
            //==save:
            if(system_debug) {
                line(src2, focus, resConf[i][3].point, Scalar(0,255,255));
                circle(src2, resConf[i][3].point, 2, Scalar(0,255,0));
                sprintf(text, "%.2f", resConf[i][3].confidence);
                putText(src2, text, resConf[i][3].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
            }

            //right: [4]            
            minimumDist = getDistFromP2L(resConf[i][3].point, focus, angle5 + angleOffset);
            maximumDist = getDistFromP2L(resConf[i][3].point, focus, angle5 - angleOffset);
            endAngle = -getAngleByFourP(
                        Point(0,0), Point(5, 0), resConf[i][2].point, resConf[i][3].point);
            startAngle = endAngle - CV_PI/6;
            //if(startAngle < -CV_PI/2) startAngle = -CV_PI/2;
            expandFromP_Round(src, resConf[i][3].point, startAngle, endAngle, minimumDist,
                    maximumDist, rectsize, res, 4);
            resConf[i][4] = res;
            //==save:
            if(system_debug) {
                line(src2, focus, resConf[i][4].point, Scalar(0,255,255));
                circle(src2, resConf[i][4].point, 2, Scalar(0,255,0));
                sprintf(text, "%.2f", resConf[i][4].confidence);
                putText(src2, text, resConf[i][4].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
                saveFile_collarWay2 = QString("RoundCollar_3expandDot%1.jpg").arg(QString::number(i+1));
                imwrite(saveFile_collarWay2.toStdString(), src2);
            }

        }// point Center;
    }

    //get max confidence:
    float maxconf = 0;
    int maxconfIndx = 0;
    {
        float sumConfTmp;
        for(int i = 0; i < TOPNum; i++) {//each center Point
            sumConfTmp = 0;
            for(int j = 0; j < RSEARCH_LINE; j++) { //each expand
                sumConfTmp += resConf[i][j].confidence;
            }
            if(sumConfTmp > maxconf) {
                maxconf = sumConfTmp;
                maxconfIndx = i;
            }
        }
    }

    roundCollarConf = maxconf;
    //==save:
    if(system_debug) {
        src.copyTo(src2);
        circle(src2, resConf[maxconfIndx][0].point, 2, Scalar(255,0,0));
        for(int j = 1; j < RSEARCH_LINE; j++) {
            circle(src2, resConf[maxconfIndx][j].point, 2, Scalar(255,0,0));
            line(src2, resConf[maxconfIndx][j-1].point, resConf[maxconfIndx][j].point, Scalar(0,255,255));
        }
        saveFile_collarWay2 = QString("RoundCollar_0res%1.jpg").arg(QString::number(maxconfIndx+1));
        imwrite(saveFile_collarWay2.toStdString(), src2);
    }
}

void VstyleCollar_collarW2(const Mat &src, float& vstyleCollarConf)
{
    int rectsize = (rshoulder.x - lshoulder.x)/5;
    Point pStart, pEnd;
    {
        Point hipCenterDot = (lhip + rhip)*0.5;
        pStart = neck*(5.0/6) + hipCenterDot*(1.0/6);
        pEnd = neck*(1.0/3) + hipCenterDot*(2.0/3);
    }
    //===save:
    Mat src2;

    const int TOPNum = 6;
    const int EXPAND_DotsNum = 4;
    node topPNode[TOPNum];
    {
        //===save:
        if(system_debug) {
            src.copyTo(src2);
            line(src2, pStart, pEnd, Scalar(255, 0, 0));
        }

        float ppDist = getEuclideanDist(pStart, pEnd);
        int ppSteps = rectsize / 4;
        int ppNum = ppDist / ppSteps;

        Point p;
        vector<node> confTmp;
        for(int iter = -rectsize; iter <= rectsize; iter += ppSteps) {
            Point pStartTmp(pStart.x+iter, pStart.y);
            Point pEndTmp(pEnd.x+iter, pEnd.y);
            //==save:
            if(system_debug) {
                line(src2, pStartTmp, pEndTmp, Scalar(0,255,255));
            }

            for(int i = 0; i <= ppNum; i++) {
                p = pStartTmp*(i*1.0/ppNum) + pEndTmp*((ppNum-i)*1.0/ppNum);
                node tmp;
                tmp.confidence = getPointConf_Vstyle(src, rectsize, p, 4);
                tmp.point = p;
                confTmp.push_back(tmp);
            }
        }
        sort(confTmp.begin(), confTmp.end(), sortDescNodeByConf);
        for(int i = 0; i < TOPNum; i++) {
            topPNode[i] = confTmp[i];
            //==save:
            if(system_debug) {
                circle(src2, topPNode[i].point, 1, Scalar(255,0,0));
                char text[10];
                sprintf(text, "p%d", i+1);
                putText(src2, text, topPNode[i].point, CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,255,0));
            }
        }
        //==save:
        if(system_debug) {
            saveFile_collarWay2 = QString("VstyleCollar_2Topnode.jpg");
            imwrite(saveFile_collarWay2.toStdString(), src2);
        }
    }

    node resConf[TOPNum][EXPAND_DotsNum];
    {
        float offset = rectsize / 10.0;

        for (int i = 0; i < TOPNum; i++) {//each point;
            Point p = topPNode[i].point;

            float angle = getAngleByFourP(neck, p, Point(0,0), Point(5,0));
            float startAngle = angle - CV_PI/2;
            float endAngle = angle + CV_PI/2;

            Point p1 = (p + neck)*0.5;
            Point p2 = p*0.25 + neck * 0.75;

            float maxConf_centerP = 0;
            for (int step = offset; step < rectsize; step += offset) {//expand step:
                node expandDots[EXPAND_DotsNum];
                expandDots[0].point = Point(p1.x + step*cos(startAngle), p1.y + step*sin(startAngle));
                expandDots[1].point = Point(p2.x + step*cos(startAngle), p2.y + step*sin(startAngle));
                expandDots[2].point = Point(p1.x + step*cos(endAngle), p1.y + step*sin(endAngle));
                expandDots[3].point = Point(p2.x + step*cos(endAngle), p2.y + step*sin(endAngle));

                float tmpTotal = 0;
                for(int k = 0; k < EXPAND_DotsNum; k++) {
                    expandDots[k].confidence = getPointConf_Vstyle(src, rectsize, expandDots[k].point, k);
                    tmpTotal += expandDots[k].confidence;
                }
                if(tmpTotal <= maxConf_centerP) continue;
                maxConf_centerP = tmpTotal;
                for(int k = 0; k < EXPAND_DotsNum; k++)
                    resConf[i][k] = expandDots[k];
            }

            //==save:
            char text[10];
            if(system_debug) {
                src.copyTo(src2);
                line(src2, p, neck, Scalar(255,0,0));
                circle(src2, p1, 5, Scalar(255,255,255));
                circle(src2, p2, 3, Scalar(255,255,255));

                Point expandline1 = Point(p1.x+rectsize*cos(startAngle), p1.y+rectsize*sin(startAngle));
                Point expandline2 = Point(p2.x+rectsize*cos(startAngle), p2.y+rectsize*sin(startAngle));
                Point expandline3 = Point(p1.x+rectsize*cos(endAngle), p1.y+rectsize*sin(endAngle));
                Point expandline4 = Point(p2.x+rectsize*cos(endAngle), p2.y+rectsize*sin(endAngle));
                line(src2, p1, expandline1, Scalar(0,255,255));
                line(src2, p2, expandline2, Scalar(0,255,255));
                line(src2, p1, expandline3, Scalar(0,255,255));
                line(src2, p2, expandline4, Scalar(0,255,255));

                for(int k = 0; k < EXPAND_DotsNum; k++) {
                    circle(src2, resConf[i][k].point, 5, Scalar(0,255,0));
                    sprintf(text, "%.2f", resConf[i][k].confidence);
                    putText(src2, text, resConf[i][k].point+Point(0,5), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255,255,0));
                }
                saveFile_collarWay2 = QString("VstyleCollar_3expandDot%1.jpg").arg(QString::number(i+1));
                imwrite(saveFile_collarWay2.toStdString(), src2);
            }
        }
    }

    float max_resConf = 0;
    int maxIndex = -1;
    {
        for(int i = 0; i < TOPNum; i++) {
            float tmpTotal = topPNode[i].confidence;
            for(int k = 0; k < EXPAND_DotsNum; k++) {
                tmpTotal += resConf[i][k].confidence;
            }
            if(max_resConf < tmpTotal) {
                max_resConf = tmpTotal;
                maxIndex = i;
            }
        }
    }

    vstyleCollarConf = max_resConf;
    //==save:
    if(system_debug) {
        src.copyTo(src2);
        circle(src2, topPNode[maxIndex].point, 2, Scalar(0,0,255));
        for(int k = 0; k < EXPAND_DotsNum; k++) {
            circle(src2, resConf[maxIndex][k].point, 2, Scalar(255,0,0));
        }
        line(src2, topPNode[maxIndex].point, resConf[maxIndex][1].point, Scalar(0,255,255));
        line(src2, topPNode[maxIndex].point, resConf[maxIndex][3].point, Scalar(0,255,255));
        line(src2, resConf[maxIndex][1].point, resConf[maxIndex][0].point, Scalar(0,255,255));
        line(src2, resConf[maxIndex][3].point, resConf[maxIndex][2].point, Scalar(0,255,255));
        saveFile_collarWay2 = QString("VstyleCollar_0res%1.jpg").arg(QString::number(maxIndex+1));
        imwrite(saveFile_collarWay2.toStdString(), src2);
    }
}

void FcloseCollar_collarW2(const Mat &src, float& fcloseCollarConf)
{
    int rectsize = (rshoulder.x - lshoulder.x)/3;
    Mat pointRect = src(Range(neck.y - rectsize/2, neck.y + rectsize/2),
                        Range(neck.x - rectsize/2, neck.x + rectsize/2));
    struct inputPredict inputData;
    inputData.data = calHog2(pointRect);
    inputData.n = 1;

    struct outputPredict outputData;
    svm_predict2b(inputData, outputData, modelForFclose, 0);
    fcloseCollarConf = outputData.conf1;
}

void FstyleCollar_collarW2(const Mat &src, float &fstyleCollarConf)
{
    Mat src2, src3;
    int rectsize = (rshoulder.x - lshoulder.x)/8;

    const int TOPNum = 3;
    pairNode topPairNode2and3[TOPNum];
    {
        //area:
        float shoulderWidth = getEuclideanDist(lshoulder, rshoulder);
        float rectheight = shoulderWidth * 1.0 / 5;
        float rectwidth = shoulderWidth * 3.0 / 5;
        float offsetthreshold = rectwidth / 2.0;

        Point startPoint = Point(neck.x - rectwidth/2, neck.y - rectheight*3.0/5);
        Point lpoint, rpoint;
        src.copyTo(src2);
        src.copyTo(src3);

        //get node:
        float steplen = rectsize / 3.0;
        node lnode[100][100], rnode[100][100];
        for(int i = 0; i*steplen <= rectheight; i++) { //height
            rpoint.y = lpoint.y = startPoint.y + i*steplen;
            for(int j = 0; j*steplen <= offsetthreshold; j++) { //width
                lpoint.x = startPoint.x + j*steplen;
                rpoint.x = 2*neck.x - lpoint.x;

                //==save:
                if(system_debug) {
                    circle(src2, lpoint, 2, Scalar(255,0,0));
                    circle(src3, rpoint, 2, Scalar(255,0,0));
                }

                float conf = getPointConf_Fstyle(src, rectsize, lpoint, 1);
                lnode[i][j].confidence = conf;
                lnode[i][j].point = lpoint;

                conf = getPointConf_Fstyle(src, rectsize, rpoint, 2);
                rnode[i][j].confidence = conf;
                rnode[i][j].point = rpoint;
            }
        }
        if(system_debug) {
            saveFile_collarWay2 = QString("FstyleCollar_2leftTopnode.jpg");
            imwrite(saveFile_collarWay2.toStdString(), src2);
            saveFile_collarWay2 = QString("FstyleCollar_2rightTopnode.jpg");
            imwrite(saveFile_collarWay2.toStdString(), src3);
        }


        //get pairnode:
        //pairNode tmpres[200];
        //int count = 0;
        vector<pairNode> tmpres;
        int nheight = rectheight / steplen + 1;
        int nwidth = offsetthreshold / steplen + 1;
        for(int i = 0; i < nheight; i++) {
            for(int j = 0; j < nwidth; j++) {
                float sumConf = 0;
                sumConf += lnode[i][j].confidence;
                float rmaxconf;
                Point rp;
                {
                    rmaxconf = 0;
                    int searchWidthNum = int(rectsize*1.2/steplen);
                    for(int ii = i-searchWidthNum; ii < i+searchWidthNum; ii++) {
                        if(ii >= nheight || ii < 0) continue;
                        for(int jj = j-searchWidthNum; jj < j+searchWidthNum; jj++) {
                            if(jj < 0 || jj >= nwidth) continue;
                            if(rnode[ii][jj].confidence > rmaxconf) {
                                rmaxconf = rnode[ii][jj].confidence;
                                rp = rnode[ii][jj].point;
                            }
                        }
                    }
                }
                sumConf += rmaxconf;

                pairNode tmp;
                tmp.pl = lnode[i][j].point;
                tmp.confidence1 = lnode[i][j].confidence;
                tmp.pr = rp;
                tmp.confidence2 = rmaxconf;
                tmp.confidencesum = sumConf;
                tmpres.push_back(tmp);
            }
        }
        sort(tmpres.begin(), tmpres.end(), sortPairDescNodeByConf);
        //==save:
        char text[10];
        if(system_debug) {
            src.copyTo(src2);
        }
        for(int i = 0; i < TOPNum; i++) {
            topPairNode2and3[i] = tmpres[i];
            if(system_debug) {
                //cout << tmpres[i].pl << ":"<<tmpres[i].pr << endl;
                line(src2, topPairNode2and3[i].pl, topPairNode2and3[i].pr, Scalar(0,255,255));
                //sprintf(text, "%.2f", topPairNode2and3[i].confidence1);
                //putText(src2, text, topPairNode2and3[i].pl+Point(0,5),CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,255,0));
                //sprintf(text, "%.2f", topPairNode2and3[i].confidence2);
                //putText(src2, text, topPairNode2and3[i].pr+Point(0,5),CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,255,0));
            }
        }
        if(system_debug) {
            saveFile_collarWay2 = QString("FstyleCollar_2TopPairNode.jpg");
            imwrite(saveFile_collarWay2.toStdString(), src2);
        }
    }

    //expand search 1&4 based on 2&3:
    //1 based 2, 4 based 3;
    pairNode res1and4[TOPNum];
    {
        //area:
        float rectheight = rectsize * 2;
        float rectwidth = rectsize * 2;
        float steplen = rectsize / 3.0;

        //get pairnode 1&4:
        for(int k = 0; k < TOPNum; k++) {
            //expand center:
            Point lstartPoint = Point(topPairNode2and3[k].pl.x - rectwidth,
                                      topPairNode2and3[k].pl.y - rectheight/5.0);
            Point rstartPoint = Point(topPairNode2and3[k].pr.x + rectwidth,
                                      topPairNode2and3[k].pr.y - rectheight/5.0);

            if(system_debug) {
                src.copyTo(src2);
                src.copyTo(src3);
            }
            //get alternative 1&4 node:
            node lnode[100][100], rnode[100][100];
            for(int i = 0; i * steplen <= rectheight; i++) {
                Point lpoint, rpoint;
                lpoint.y = lstartPoint.y + i*steplen;
                rpoint.y = rstartPoint.y + i*steplen;
                for(int j = 0; j * steplen <= rectwidth; j++) {
                    lpoint.x = lstartPoint.x + j*steplen;
                    rpoint.x = rstartPoint.x - j*steplen;
                    if(lpoint.x <= lshoulder.x || rpoint.x >= rshoulder.x) continue;

                    float confl = getPointConf_Fstyle(src, rectsize, lpoint, 0);
                    lnode[i][j].point = lpoint;
                    lnode[i][j].confidence = confl;
                    if(system_debug) {
                        circle(src2, lnode[i][j].point, 2, Scalar(255,0,0));
                    }

                    float confr = getPointConf_Fstyle(src, rectsize, rpoint, 3);
                    rnode[i][j].point = rpoint;
                    rnode[i][j].confidence = confr;
                    if(system_debug) {
                        circle(src3, rnode[i][j].point, 2, Scalar(255,0,0));
                    }
                }
            }
            if(system_debug) {
                saveFile_collarWay2 = QString("FstyleCollar_3leftExpandDot%1.jpg").arg(QString::number(k+1));
                imwrite(saveFile_collarWay2.toStdString(), src2);
                saveFile_collarWay2 = QString("FstyleCollar_3rightExpandDot%1.jpg").arg(QString::number(k+1));
                imwrite(saveFile_collarWay2.toStdString(), src3);
            }

            //get max pairnode 1&4:
            float tmpMaxconf1and4 = 0;
            int nheight = rectheight / steplen + 1;
            int nwidth = rectwidth / steplen + 1;
            for(int i = 0; i < nheight; i++) {
                for (int j = 0; j < nwidth; j++) {
                    float sumConf = 0;
                    sumConf += lnode[i][j].confidence;

                    float rmaxconf;
                    Point rp;
                    {
                        rmaxconf = 0;
                        int nstep = 4;
                        for(int ii = i-nstep; ii < i+nstep; ii++) {
                            if(ii >= nheight || ii < 0) continue;
                            for(int jj = j-nstep; jj < j+nstep; jj++) {
                                if(jj < 0 || jj >= nwidth) continue;
                                if(rnode[ii][jj].confidence > rmaxconf) {
                                    rmaxconf = rnode[ii][jj].confidence;
                                    rp = rnode[ii][jj].point;
                                }
                            }
                        }
                    }

                    sumConf += rmaxconf;
                    if(tmpMaxconf1and4 < sumConf) {
                        res1and4[k].confidence1 = lnode[i][j].confidence;
                        res1and4[k].confidence2 = rmaxconf;
                        res1and4[k].pl = lnode[i][j].point;
                        res1and4[k].pr = rp;
                        res1and4[k].confidencesum = sumConf;
                    }
                }
            }//get max pairnode 1&4;
            //cout << res1and4[k].pl << ":" << res1and4[k].pr << endl;

            //==save:
            if(system_debug) {
                src.copyTo(src2);
                circle(src2, topPairNode2and3[k].pl, 2, Scalar(0,0,255));
                circle(src2, topPairNode2and3[k].pr, 2, Scalar(0,0,255));
                line(src2, topPairNode2and3[k].pl, topPairNode2and3[k].pr, Scalar(0,255,255));
                circle(src2, res1and4[k].pl, 2, Scalar(255,0,0));
                line(src2, topPairNode2and3[k].pl, res1and4[k].pl, Scalar(0,255,255));
                circle(src2, res1and4[k].pr, 2, Scalar(255,0,0));
                line(src2, topPairNode2and3[k].pr, res1and4[k].pr, Scalar(0,255,255));
                saveFile_collarWay2 = QString("FstyleCollar_3expandDot%1.jpg").arg(QString::number(k+1));
                imwrite(saveFile_collarWay2.toStdString(), src2);
            }
        }
    }

    float maxconf = 0;
    int maxIndex = -1;
    for(int i = 0; i < TOPNum; i++) {
        float fopenConfTmp = topPairNode2and3[i].confidencesum + res1and4[i].confidencesum;
        if(fopenConfTmp > maxconf) {
            maxconf = fopenConfTmp;
            maxIndex = i;
        }
    }

    fstyleCollarConf = maxconf;
    //==save:
    if(system_debug) {
        src.copyTo(src2);
        circle(src2, topPairNode2and3[maxIndex].pl, 2, Scalar(0,0,255));
        circle(src2, topPairNode2and3[maxIndex].pr, 2, Scalar(0,0,255));
        circle(src2, res1and4[maxIndex].pl, 2, Scalar(0,0,255));
        circle(src2, res1and4[maxIndex].pr, 2, Scalar(0,0,255));
        saveFile_collarWay2 = QString("FstyleCollar_0res%1.jpg").arg(QString::number(maxIndex+1));
        imwrite(saveFile_collarWay2.toStdString(), src2);
    }
}

/**
 * @brief calConfByP: calculate conficende let p as center && window lenth is rectsize.
 * @param src
 * @param rectsize
 * @param p
 * @param i
 * @return
 */
float getPointConf_Round(const Mat &src, int rectsize, Point p, int mark)
{
    Mat pointRect = src(Range(p.y-rectsize/2, p.y+rectsize/2),
                        Range(p.x-rectsize/2, p.x+rectsize/2));

    struct inputPredict inputData;
    inputData.data = calHog(pointRect);
    inputData.n = mark + 1;

    struct outputPredict outputData;
    svm_predict2b(inputData, outputData, modelForR, mark);//mask is the model index.
    return outputData.conf1;
}

float getPointConf_Vstyle(const Mat &src, int rectsize, Point p, int mark)
{
    Mat pointRect = src(Range(p.y - rectsize/2, p.y + rectsize/2),
                        Range(p.x - rectsize/2, p.x + rectsize/2));

    struct inputPredict inputData;
    inputData.data = calHog(pointRect);
    inputData.n = mark + 1;

    struct outputPredict outputData;
    svm_predict2b(inputData, outputData, modelForV, mark);
    return outputData.conf1;
}

float getPointConf_Fstyle(const Mat &src, int rectsize, Point p, int i)
{
    Mat pointRect = src(Range(p.y-rectsize/2, p.y+rectsize/2),
                        Range(p.x-rectsize/2, p.x+rectsize/2));
    struct inputPredict inputData;
    inputData.data = calHog2(pointRect);
    inputData.n = i+1;

    struct outputPredict outputData;
    svm_predict2b(inputData,outputData,modelForFopen,i);
    return outputData.conf1;
}

void loadModelData()
{
    const char* mFile[COLLAR_TYPE][5] = {
        {
            "./Model/ModelForRound/trainDataSet1.dat.model",
            "./Model/ModelForRound/trainDataSet2.dat.model",
            "./Model/ModelForRound/trainDataSet3.dat.model",
            "./Model/ModelForRound/trainDataSet4.dat.model",
            "./Model/ModelForRound/trainDataSet5.dat.model",
        }, {
            "./Model/ModelForV/trainDataSet2.dat.model",
            "./Model/ModelForV/trainDataSet1.dat.model",
            "./Model/ModelForV/trainDataSet4.dat.model",
            "./Model/ModelForV/trainDataSet5.dat.model",
            "./Model/ModelForV/trainDataSet3.dat.model",
        }, {
            "./Model/ModelForF/ModelForFclose/trainDataSet.dat.model",
        }, {
            "./Model/ModelForF/ModelForFopen/trainDataSet1.dat.model",
            "./Model/ModelForF/ModelForFopen/trainDataSet2.dat.model",
            "./Model/ModelForF/ModelForFopen/trainDataSet3.dat.model",
            "./Model/ModelForF/ModelForFopen/trainDataSet4.dat.model"
        }
    };

    for(int i = 0; i < ROUND_MODEL; i++) {
        if((modelForR[i] = svm_load_model(mFile[0][i])) == 0) {
            cout << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }
    for(int i = 0; i < VSTYLE_MODEL; i++) {
        if((modelForV[i]=svm_load_model(mFile[1][i])) == 0) {
            cout << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }
    if((modelForFclose[0] = svm_load_model(mFile[2][0]))==0) {
        cout << "Cann't open model for Round collar:" << mFile[0] << endl;
        exit(1);
    }
    for(int i = 0; i < FOPEN_MODEL; i++) {
        if((modelForFopen[i]=svm_load_model(mFile[3][i]))==0) {
            cout << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }
}


////
/// \brief getFirstPointConf:
///     In image src, line L go through point p,
///     the angle of L from startAngle to endAngle,
///     the stepwidth from startR to endR.
///     get the max confidence point.
/// \param src
/// \param p
/// \param startAngle
/// \param endAngle1
/// \param startR
/// \param endR
/// \param rectsize
/// \param points
/// \param confidence
/// \param mark
///
void expandFromP_Round(const Mat &src, Point p,
                       float startAngle, float endAngle,
                       float minWid, float maxWid,
                       int rectsize, node& res, int modelMask)
{
    //int countConf = 0;
    //node confTmp[100];
    vector<node> confTmp;

    float anglestep = (endAngle - startAngle)/5;
    float step = (maxWid - minWid)/4;
    for(float i = startAngle; i <= endAngle; i += anglestep) {
        for(float len = minWid; len <= maxWid;len += step) {
            Point tmpp(p.x + len*cos(i), p.y + len*sin(i));

            if(tmpp.x < lshoulder.x || tmpp.x > rshoulder.x || tmpp.y < head.y)
                continue;

            node tmp;
            tmp.confidence = getPointConf_Round(src, rectsize, tmpp, modelMask);
            tmp.point = tmpp;
            confTmp.push_back(tmp);
        }
    }
    //qsort(confTmp, countConf, sizeof(confTmp[0]), descNodeByConf);
    sort(confTmp.begin(), confTmp.end(), sortDescNodeByConf);

    res = confTmp[0];
}
