/***************************************************************
 * the file is used to recognize collar.
 * must provide the below follow list:
 * -Model: store the model files;
 * -test.xml: test.jpg's corresponding xml data;
 * -testDataForm.txt: the program need the pose data from the xml;
 *
 * *************************************************************/
#include <QDebug>
#include "attrRecognize.h"
#include "../AttrBase/svm.h"
#include "../AttrBase/svmpredict.h"

QString CollarTestPath = SYSTEST_DIR;/*QString("%1Collar/").arg(SYSTEST_DIR);*/

struct svm_model* modelForR[ROUND_MODEL];
struct svm_model* modelForV[VSTYLE_MODEL];
struct svm_model *modelForFclose[FCLOSE_MODEL];
struct svm_model *modelForFopen[FOPEN_MODEL];

char modelFileForCollarRes[300] = {"./Model/ModelForRes/collarRes2.txt.dat.model"};
char modelFileForCollarRange[300] = {"./Model/ModelForRes/collarRes2.txt.dat.range"};

void getRoundCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* points);
void getVstyleCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* points);
void getFcloseCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* points);
void getFopenCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* points);

void analysys(float confidenceres[4][6], int res[4],int &result) {
    float threshold[4] = {4.0, 3.6, 0.65, 2.3} ;//r,v,fclose,fopen

    float sumconf;
    for(int i=0; i < 4; i++) res[i]=0;

    for(int i=0; i < 4; i++) {
        sumconf = 0;
        for(int j = 0; j < 5; j++)
            sumconf += confidenceres[i][j];
        confidenceres[i][5] = sumconf;
        if(sumconf > threshold[i]) res[i]=1;
    }

    for(int i=0; i < 4; i++) {
        if(res[i] ==1) {
            result = i;
            break;
        }
    }
}

void resizeConfDataForCollar(float origional[],float res[]) {
    FILE *fp = fopen(modelFileForCollarRange,"r");
    char tmp[20]; fgets(tmp,20,fp);

    int left, right;
    fscanf(fp,"%d %d",&left,&right);

    float min[4],max[4];
    for(int j=0; j < 4; j++) {
        fscanf(fp,"%*d %f %f", &min[j],&max[j]);
    }
    fclose(fp);

    for(int i=0; i < 4; i++) {
        res[i]=left + (origional[i]-min[i])*2.0/(max[i]-min[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief collarAnalyze
///     Give a file path of one image, and get it's collar value;
///     return the collar value;
/// \return
///     return the type index;
///
int baseWayCollar(const char * fpath, MyLocation* loc) {
    //target image:
    string fpathStr(fpath);
    Mat srcImg = imread(fpathStr);
    loadPoseData(loc);

    //collar: r/v/fclose/fopen
    const char* modelfilePath[COLLAR_TYPE][5] = {
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
    float confidence[COLLAR_TYPE][6] = {0};
    Point points[COLLAR_TYPE][5];
    getRoundCollar(srcImg, loc, modelfilePath[0], confidence[0], points[0]);
    getVstyleCollar(srcImg, loc, modelfilePath[1], confidence[1], points[1]);
    getFcloseCollar(srcImg, loc, modelfilePath[2], confidence[2], points[2]);
    getFopenCollar(srcImg, loc, modelfilePath[3], confidence[3], points[3]);

    int resttt[4] = {0};
    int result = -1;
    analysys(confidence, resttt, result);

    //prepare file 'tmp.txt';
    float origional[4], afterZoom[4];
    for(int j = 0; j < 4; j++) {
        origional[j] = confidence[j][5];
    }
    resizeConfDataForCollar(origional, afterZoom);
    FILE *tmpCollarData = fopen("tmp.txt","w+");
    fprintf(tmpCollarData, "0 ");
    for(int i = 0; i < 4; i++) {
        fprintf(tmpCollarData,"%d:%f ",i+1, afterZoom[i]);
    }
    fclose(tmpCollarData);

    //cmd: the final result stored in 'tmp.txt.res';
    char cmd[500];
    sprintf(cmd,"./svm-predict-linux tmp.txt %s tmp.txt.res > tempToThrow.txt",
            modelFileForCollarRes);
    system(cmd);

    int collarResult;
    FILE *resfp = fopen("tmp.txt.res","r");
    fscanf(resfp, "%d", &collarResult);
    fclose(resfp);

    //0-Round;
    //1-V-collar;
    //2-F-collar;
    //3-otherCollar;H-collar;O-collar
    return collarResult;
}

void getFistNMiddlePointConf(Mat &src, Point start, Point end, int rectsize, Point points[20], float confidence[20], int n);
void getFirstPointConf(Mat &src,Point center,float startAngle,float endAngle1,
                       float startR,float endR,int rectsize, Point points[],
                       float confidence[],int mark, int headY, int topbodyY, int lshoulderX, int rshoulderX);
void getMax2(Point resLeftP[3][20],Point resRightP[3][20],float resLeftC[3][20],float resRightC[3][20],int n,
Point points[5],float confidence[5]);
/**
 * @brief getRoundCollar
 * @param src
 * @param loc
 * @param mFile: model file path;
 * @param conf: confidende[6];
 * @param points: points[5];
 */
void getRoundCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* ppoints){

    //load model file:
    for(int i = 0; i < ROUND_MODEL; i++) {
        if((modelForR[i] = svm_load_model(mFile[i])) == 0) {
            qDebug() << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }

    //find region in search space:
    //    Point head = loc->getPointByName(POSE_HEAD);
    //Point neck = loc->getPointByName(POSE_NECK);
    //Point lshoulder = loc->getPointByName(POSE_LSHOULDER);
    //Point rshoulder = loc->getPointByName(POSE_RSHOULDER);
    //Point lhip = loc->getPointByName(POSE_LHIP);
    //Point rhip = loc->getPointByName(POSE_RHIP);

    int rectsize = (rshoulder.x - lshoulder.x) / 5;
    Point bottomBody((rhip.x+lhip.x) >> 1,(rhip.y+lhip.y) >> 1);
    float shoulderlen = getEuclideanDist(lshoulder, rshoulder);
    int midbodystarty = (lshoulder.y < rshoulder.y? lshoulder.y : rshoulder.y);
    midbodystarty -= (shoulderlen / 8);

    Point midBodyStart(neck.x, midbodystarty);
    Point midBodyEnd;
    midBodyEnd.x = (5*bottomBody.x + 11*neck.x)/16;
    midBodyEnd.y = (5*bottomBody.y + 11*neck.y)/16;
    //    char saveFilePath[200];
    //    sprintf(saveFilePath, "timg_R_midsearchline.jpg");
    QString saveFileName;
    if(system_debug) {
        saveFileName = QString("%1timg_R_midsearchline.jpg").arg(CollarTestPath);
        showSingleLine(src, midBodyStart, midBodyEnd, saveFileName.toStdString().c_str());
    }

    int n1 = 5;
    int n2 = 3;
    Point points[20];
    float confidence[20];
    getFistNMiddlePointConf(src, midBodyStart, midBodyEnd, rectsize, points, confidence, n1);
    //    sprintf(saveFilePath,"timg_R_middle%d_rect.jpg", n1);

    if(system_debug) {
        saveFileName.clear();
        saveFileName = QString("%1timg_R_middle5_rect.jpg").arg(CollarTestPath);
        showRectInImage2(src, points, 5, rectsize, confidence, saveFileName.toStdString().c_str());
    }

    Point resLeftP[3][20], resRightP[3][20];
    float resLeftC[3][20] = {0}, resRightC[3][20] = {0};
    midBodyStart.y = midBodyStart.y-(neck.y-head.y)*1.0/2;

    //function -- 0.getOtherPoint:
    // midpoints = points
    // midconfidence = confidence
    {
        float angle1, angle2, angleSpecial, angle4, angle5;
        angleSpecial = getAngleByFourP(Point(0,0), Point(5,0), midBodyStart, midBodyEnd);
        angle2 = angleSpecial + CV_PI/9;
        angle1 = angle2 + CV_PI/9;
        angle4 = angleSpecial - CV_PI/9;
        angle5 = angle4 - CV_PI/9;
        float angleOffset = 8.0/180*CV_PI;

        float startAngle = CV_PI;
        float endAngle = CV_PI * 5/4;
        Point po[5], p[5];
        float co[5] = {0}, c[5] = {0};
        double endR, startR;

        for(int i = 0; i < n2; i++) {
            resLeftP[0][i] = points[i];
            resLeftC[0][i] = confidence[i];

            Point midBodyStartTmp = midBodyStart;
            if((midBodyStartTmp.y > points[i].y) || (points[i].y-midBodyStartTmp.y < shoulderlen / 8)) {
                midBodyStartTmp.y = points[i].y-shoulderlen/8;
            }
            startAngle = CV_PI;//-10.0/180*CV_PI;
            endAngle = CV_PI*7/6;
            startR = getDistFromP2L(points[i], midBodyStartTmp, angle2-angleOffset);
            endR = getDistFromP2L(points[i], midBodyStartTmp, angle2+angleOffset);

            Point center = points[i];
            getFirstPointConf(src, center, startAngle, endAngle, startR, endR, rectsize, po, co, 1, head.y, neck.y, lshoulder.x, rshoulder.x);
            resLeftC[1][i] = co[0];
            resLeftP[1][i] = po[0];

            startAngle = getAngleByFourP(Point(0,0),Point(5,0),points[i],po[0]);
            startAngle = CV_PI*2-startAngle;//-10.0/180*CV_PI;
            startR = getDistFromP2L(po[0],midBodyStartTmp,angle1 - angleOffset);
            endR = getDistFromP2L(po[0],midBodyStartTmp,angle1+angleOffset);
            endAngle = startAngle+CV_PI/6;
            if(endAngle>CV_PI*3/2)endAngle = CV_PI*3/2;
            center = po[0];
            getFirstPointConf(src, center, startAngle, endAngle, startR, endR, rectsize, p, c, 0, head.y, neck.y, lshoulder.x, rshoulder.x);
            resLeftC[2][i] = c[0];
            resLeftP[2][i] = p[0];
        }

        for(int i = 0; i < n2; i++) {
            resRightC[0][i] = confidence[i];
            resRightP[0][i] = points[i];


            Point midBodyStartTmp = midBodyStart;
            if(midBodyStartTmp.y > points[i].y || points[i].y-midBodyStartTmp.y < shoulderlen/8) {
                midBodyStartTmp.y = points[i].y-shoulderlen/8;
            }
            endAngle = 0;//+10.0/180*CV_PI;
            startAngle = 0-CV_PI/6.0;
            startR = getDistFromP2L(points[i], midBodyStartTmp, angle4+angleOffset);
            endR = getDistFromP2L(points[i], midBodyStartTmp, angle4-angleOffset);

            Point center = points[i];
            getFirstPointConf(src,center,startAngle,endAngle,startR,endR,rectsize,po,co,3, head.y, neck.y, lshoulder.x, rshoulder.x);
            resRightC[1][i] = co[0];
            resRightP[1][i] = po[0];

            endAngle = 0 - getAngleByFourP(Point(0,0),Point(5,0),points[i],po[0]);
            startAngle = endAngle - CV_PI/6;
            endAngle = endAngle;//+10.0/180*CV_PI;
            startR = getDistFromP2L(po[0],midBodyStartTmp,angle5+angleOffset);
            endR = getDistFromP2L(po[0],midBodyStartTmp,angle5-angleOffset);

            if(startAngle<-CV_PI/2) startAngle = -CV_PI/2;
            center = po[0];
            getFirstPointConf(src,center,startAngle,endAngle,startR,endR,rectsize,p,c, 4, head.y, neck.y, lshoulder.x, rshoulder.x);
            resRightC[2][i] = c[0];
            resRightP[2][i] = p[0];
        }
    } // function over

    //getMax2(resLeftP, resRightP, resLeftC, resRightC, n2, ppoints, conf);
    //confidence = conf
    {
        float lconfidence[20]={0},rconfidence[20] = {0};
        for(int i = 0; i < n2; i++) {
            float sum = 0;
            sum= resLeftC[0][i] + resLeftC[1][i] + resLeftC[2][i];
            if(sum > lconfidence[i]) {
                lconfidence[i] = sum;
            }
            sum = resRightC[0][i]+resRightC[1][i]+resRightC[2][i];
            if(sum>rconfidence[i]) {
                rconfidence[i] = sum;
            }
        }
        int mk = -1;
        float maxconf = 0;
        for(int i = 0; i < n2; i++) {
            if(lconfidence[i] + rconfidence[i] > maxconf) {
                maxconf = lconfidence[i] + rconfidence[i];
                mk = i;
            }
        }
        ppoints[0] = resLeftP[2][mk];
        conf[0] = resLeftC[2][mk];
        ppoints[1] = resLeftP[1][mk];
        conf[1] = resLeftC[1][mk];
        ppoints[2] = resLeftP[0][mk];
        conf[2] = resLeftC[0][mk];
        ppoints[3] = resRightP[1][mk];
        conf[3] = resRightC[1][mk];
        ppoints[4] = resRightP[2][mk];
        conf[4] = resRightC[2][mk];
    }
    float sum = 0;
    for(int i = 0; i < 5; i++) {
        sum += conf[i];
    }
    //    sprintf(saveFilePath, "timg_R_res%f.jpg", sum);
    if(system_debug) {
        saveFileName.clear();
        saveFileName = QString("%1timg_R_res%2.jpg").arg(CollarTestPath).arg(sum);
        showRectInImage2(src, ppoints, 5, rectsize, conf, saveFileName.toStdString().c_str());
    }
}

float calConfByPForV(Mat &src,int rectsize,Point p,int i);
void getFistNMiddlePointConfForV(Mat &src,Point start,Point end,int rectsize,Point points[20], float confidence[20],int n);
/**
 * @brief getVstyleCollar
 * @param src
 * @param loc
 * @param mFile
 * @param conf
 * @param points
 */
void getVstyleCollar(Mat& src, MyLocation *loc, const char** mFile, float* conf, Point* ppoints){

    //load model file:
    for(int i = 0; i < VSTYLE_MODEL; i++) {
        if((modelForV[i]=svm_load_model(mFile[i])) == 0) {
            qDebug() << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }

    // find region in search space:
    //    Point neck = loc->getPointByName(POSE_NECK);
    //    Point lshoulder = loc->getPointByName(POSE_LSHOULDER);
    //    Point rshoulder = loc->getPointByName(POSE_RSHOULDER);
    //    Point lhip = loc->getPointByName(POSE_LHIP);
    //    Point rhip = loc->getPointByName(POSE_RHIP);

    int rectsize = (rshoulder.x - lshoulder.x)/5;
    Point midBodyStart, midBodyEnd;
    {
        Point bottomBody((rhip.x+lhip.x) >> 1,(rhip.y+lhip.y) >> 1);
        midBodyStart.x = (neck.x*5+bottomBody.x)/6;
        midBodyStart.y = (neck.y*5+bottomBody.y)/6;
        midBodyEnd.x = (neck.x + 2*bottomBody.x)/3;
        midBodyEnd.y = (neck.y + 2*bottomBody.y)/3;
    }

    int n = 2;
    Point points[20];
    float confidence[20];
    //    char saveFilePath[200];
    //sprintf(saveFilePath,"%s_midRect.jpg",fullFilePathWithoutExten);

    getFistNMiddlePointConfForV(src,midBodyStart,midBodyEnd,rectsize, points, confidence,n);
    //    sprintf(saveFilePath, "timg_V_middle%d.jpg", n);
    QString saveFileName;
    if(system_debug) {
        saveFileName = QString("%1timg_V_middle2.jpg").arg(CollarTestPath);
        showRectInImage2(src, points, n, rectsize, confidence, saveFileName.toStdString().c_str());
        //        Mat src2;
        //        src.copyTo(src2);
        //        line(src2, midBodyStart, midBodyEnd, Scalar(0,255,255));
        //        imwrite("test.jpg", src2);
    }

    node res[5];
    // function -- getOtherPointForV
    {
        Point p1, p2, iterp[4];
        float offset = rectsize / 10.0;
        float len = (getEuclideanDist(lshoulder,rshoulder)/4.0);

        float midlineangle, langle, rangle;
        node resTmp[5][50][4];
        int count = 0;
        Mat src2;
        //        char saveFilePath[200];
        for(int i = 0; i < n; i++) {
            src.copyTo(src2);
            line(src2,points[i], neck, Scalar(rand()%256,rand()%256,rand()%256), 2);
            p1.x = (points[i].x+neck.x)/2;
            p1.y = (points[i].y+neck.y)/2;
            p2.x = (points[i].x+3*neck.x)/4;
            p2.y = (points[i].y+3*neck.y)/4;
            midlineangle = 2*CV_PI - getAngleByFourP(points[i], neck,Point(0,0),Point(5,0));
            langle = midlineangle-CV_PI/2;
            rangle = midlineangle+CV_PI/2;
            count = 0;
            for(float r = 1;r<len;r=r+offset) {
                iterp[0].x = p1.x+r*cos(langle);
                iterp[0].y = p1.y+r*sin(langle);
                iterp[1].x = p2.x+1.45*r*cos(langle);
                iterp[1].y = p2.y+1.45*r*sin(langle);

                iterp[2].x = p1.x+r*cos(rangle);
                iterp[2].y = p1.y+r*sin(rangle);
                iterp[3].x = p2.x+1.45*r*cos(rangle);
                iterp[3].y = p2.y+1.45*r*sin(rangle);

                line(src2,iterp[0],iterp[2],Scalar(rand()%256,rand()%256,rand()%256),2);
                line(src2,iterp[1],iterp[3],Scalar(rand()%256,rand()%256,rand()%256),2);
                for(int k = 0;k<4;k++) {
                    resTmp[i][count][k].confidence = calConfByPForV(src,rectsize,iterp[k],k);
                    resTmp[i][count][k].point = iterp[k];
                }
                count++;
            }
            //            sprintf(saveFilePath,"timg_V_searchLine%d.jpg", i);
            if(system_debug) {
                saveFileName.clear();
                saveFileName = QString("%1collarV_searchLine%2.jpg").arg(CollarTestPath).arg(i);
                imwrite(saveFileName.toStdString().c_str(), src2);
            }
        }

        int tmpi = -1, tmpj = -1;
        float maxconfidence = 0, sum = 0;
        for(int i = 0;i<n;i++) {
            for(int j=0;j<count;j++) {
                sum = 0;
                for(int k=0;k<4;k++) {
                    sum+=resTmp[i][j][k].confidence;
                }
                sum += confidence[i];
                if(sum > maxconfidence) {
                    maxconfidence = sum;
                    tmpi = i;
                    tmpj = j;
                }
            }
        }
        res[0].confidence = resTmp[tmpi][tmpj][1].confidence;
        res[0].point = resTmp[tmpi][tmpj][1].point;

        res[1].confidence = resTmp[tmpi][tmpj][0].confidence;
        res[1].point = resTmp[tmpi][tmpj][0].point;

        res[2].confidence = confidence[tmpi];
        res[2].point = points[tmpi];

        res[3].confidence = resTmp[tmpi][tmpj][2].confidence;
        res[3].point = resTmp[tmpi][tmpj][2].point;

        res[4].confidence = resTmp[tmpi][tmpj][3].confidence;
        res[4].point = resTmp[tmpi][tmpj][3].point;
    }

    float sum = 0;
    for(int i = 0; i < 5; i++) {
        conf[i] = res[i].confidence;
        ppoints[i] = res[i].point;
        sum += conf[i];
    }
    //    sprintf(saveFilePath, "timg_V_res%f.jpg", sum);
    if(system_debug) {
        saveFileName.clear();
        saveFileName = QString("%1timg_V_res%2.jpg").arg(CollarTestPath).arg(sum);
        showRectInImage2(src, ppoints, 5, rectsize, conf, saveFileName.toStdString().c_str());
    }
}

void getFcloseCollar(Mat& src, MyLocation *loc, const char** mFile, float* pconf, Point* ppoints){

    if((modelForFclose[0] = svm_load_model(mFile[0]))==0) {
        qDebug() << "Cann't open model for Round collar:" << mFile[0] << endl;
        exit(1);
    }

    //Point neck = loc->getPointByName(POSE_NECK);
    //Point lshoulder = loc->getPointByName(POSE_LSHOULDER);
    //Point rshoulder = loc->getPointByName(POSE_RSHOULDER);

    int rectsize = (rshoulder.x - lshoulder.x)/3;
    //float conf = calConfByPForFclose(src, rectsize, topbody, 0);
    float conf;
    {
        Mat pointRect = src(Range(neck.y-rectsize/2, neck.y+rectsize/2),
                            Range(neck.x-rectsize/2, neck.x+rectsize/2));
        struct inputPredict inputData;
        inputData.data = calHog2(pointRect);
        inputData.n = 1;
        struct outputPredict outputData;
        svm_predict2b(inputData,outputData,modelForFclose,0);
        conf = outputData.conf1;
    }
    ppoints[0] = neck;
    pconf[0] = conf;
    //    char savefilePath[300];
    //    sprintf(savefilePath,"timg_Fclose_res_%f.jpg", conf);
    if(system_debug) {
        QString saveFileName = QString("%1timg_Fclose_res_%2.jpg").arg(CollarTestPath).arg(conf);
        showRectInImage2(src, ppoints, 1, rectsize, pconf, saveFileName.toStdString().c_str());
    }
}


float calConfByPForFopen(Mat &src,int rectsize, Point p, int i);
float maxconfidenceForFopen(int centeri,int centerj,int nstep,node rrigion[][100],int nwidth,int nheight,Point &p);
int compForFopenRes(const void*a,const void*b);
/**
 * @brief getFopenCollar
 * @param src
 * @param loc
 * @param mFile
 * @param pconf
 * @param ppoints
 */
void getFopenCollar(Mat& src, MyLocation *loc, const char** mFile, float* pconf, Point* ppoints){

    // load model file:
    for(int i = 0; i < FOPEN_MODEL; i++) {
        if((modelForFopen[i]=svm_load_model(mFile[i]))==0) {
            qDebug() << "Cann't open model for Round collar:" << mFile[i] << endl;
            exit(1);
        }
    }

    //Point neck = loc->getPointByName(POSE_NECK);
    //Point lshoulder = loc->getPointByName(POSE_LSHOULDER);
    //Point rshoulder = loc->getPointByName(POSE_RSHOULDER);

    int rectsize = (rshoulder.x - lshoulder.x)/8;

    // find region in search space:
    pairNode res2and3[3], res1and4[3];
    //find2and3rigionForFopen(src,loc,rectsize,rectsize*1.2,res2and3,1.0);
    // function -- find2and3rigionForFopen:
    {
        float shoulderWidth = getEuclideanDist(lshoulder, rshoulder);
        float rectwidth = shoulderWidth * 3.0 / 5;
        float rectheight = shoulderWidth * 1.0 / 5;

        Point lstartPoint, lpoint, rpoint;
        lstartPoint.x= neck.x - rectwidth/2;
        lstartPoint.y = neck.y - rectheight*3.0/5;


        node lrigion[100][100],rrigion[100][100];
        float steplen = rectsize * 1.0 / 3;
        for(int i=0;i*steplen<=rectheight;i++) {
            lpoint.y = lstartPoint.y+i*steplen;
            rpoint.y = lpoint.y;
            for(int j=0;j*steplen<=rectwidth/2.0;j++) {
                lpoint.x = lstartPoint.x+j*steplen;
                rpoint.x = 2*neck.x-lpoint.x;

                float conf = calConfByPForFopen(src,rectsize,lpoint,1);
                lrigion[i][j].confidence=conf;
                lrigion[i][j].point = lpoint;

                conf = calConfByPForFopen(src,rectsize,rpoint,2);
                rrigion[i][j].confidence=conf;
                rrigion[i][j].point = rpoint;
            }
        }
        float offsetthreshold = rectwidth/2.0;
        int nwidth = offsetthreshold/steplen+1;
        int nheight = rectheight/steplen+1;

        pairNode tmpres[200];
        int k = 0;
        int searchWidthNum = (int)(rectsize*1.2/steplen);
        for(int i = 0; i < nheight; i++) {
            for(int j = 0; j < nwidth; j++) {
                Point rp;
                float sumconf = lrigion[i][j].confidence;
                float rmaxconf = maxconfidenceForFopen(i, j, searchWidthNum, rrigion, nwidth, nheight, rp);
                sumconf += rmaxconf;

                //if(sumconf > 1.0) {
                tmpres[k].confidence1 = lrigion[i][j].confidence;
                tmpres[k].confidence2 = rmaxconf;
                tmpres[k].confidencesum=sumconf;
                tmpres[k].pl = lrigion[i][j].point;
                tmpres[k].pr = rp;
                k++;
                //}
            }
        }
        qsort(tmpres, k+1, sizeof(tmpres[0]), compForFopenRes);
        for(int i=0; i < 3; i++) {
            res2and3[i] = tmpres[i];
            //cout << tmpres[i].pl << ":"<<tmpres[i].pr << endl;
        }
    }// function over

    //find1and4rigionForFopen(src,loc,rectsize,res2and3, 3, res1and4);
    // function -- find1and4rigionForFopen:
    {
        float steplen = rectsize*1.0/3;
        float rectwidth = rectsize*2;
        float rectheight = rectsize*2;
        Point lstartPoint,lpoint,rpoint,rstartPoint;
        float confidence[5]={0};
        Point points[5];
        for(int k = 0; k < 3; k++) {
            //if(res2and3[k].confidence1 > 0.3 && res2and3[k].confidence2 > 0.3) {
            lstartPoint.x=res2and3[k].pl.x-rectwidth;
            lstartPoint.y=res2and3[k].pl.y-rectheight/5.0;
            rstartPoint.x = res2and3[k].pr.x+rectwidth;
            rstartPoint.y = res2and3[k].pr.y-rectheight/5.0;

            Point maxConfPL, maxConfPR;
            float maxConfL=0, maxConfR = 0;
            float maxconf = 0;
            node lrigion[100][100],rrigion[100][100];
            for(int i=0;i*steplen<=rectheight;i++) {
                lpoint.y = lstartPoint.y+i*steplen;
                rpoint.y = rstartPoint.y+i*steplen;
                for(int j=0;j*steplen<=rectwidth;j++) {
                    lpoint.x = lstartPoint.x+j*steplen;
                    rpoint.x = rstartPoint.x-j*steplen;
                    if(lpoint.x>lshoulder.x&&rpoint.x<rshoulder.x) {
                        float confl = calConfByPForFopen(src,rectsize,lpoint,0);
                        float confr = calConfByPForFopen(src,rectsize,rpoint,3);
                        lrigion[i][j].confidence=confl;
                        lrigion[i][j].point = lpoint;
                        rrigion[i][j].confidence=confr;
                        rrigion[i][j].point = rpoint;
                    }
                }
            }
            int nheight = rectheight/steplen;
            int nwidth = rectwidth/steplen;

            maxconf = 0;
            for(int i=0;i<=nheight;i++) {
                for(int j=0;j<=nwidth;j++) {
                    lpoint =  lrigion[i][j].point;
                    float confl = lrigion[i][j].confidence;
                    float maxRconf = maxconfidenceForFopen(i,j,4,rrigion,nwidth,nheight,rpoint);

                    if(confl+maxRconf>maxconf) {
                        maxConfPL = lpoint;
                        maxConfPR = rpoint;
                        maxConfL = confl;
                        maxConfR = maxRconf;
                        maxconf = confl+maxRconf;
                    }
                }
            }

            res1and4[k].confidence1 = maxConfL;
            res1and4[k].confidence2 = maxConfR;
            res1and4[k].pl = maxConfPL;
            res1and4[k].pr = maxConfPR;
            res1and4[k].confidencesum = maxConfL+maxConfR;
            confidence[0]= maxConfL;
            confidence[1] = res2and3[k].confidence1;
            confidence[2] = res2and3[k].confidence2;
            confidence[3] = maxConfR;
            points[0] = maxConfPL;
            points[1] = res2and3[k].pl;
            points[2] = res2and3[k].pr;
            points[3] = maxConfPR;
            float sumconf = 0;
            for(int iter=  0;iter<4;iter++)sumconf+=confidence[iter];
        }
        //}
    } // function over

    int mark =  0;
    float maxconf = 0;
    for(int i=0; i < 3; i++) {
        //cout << res1and4[i].pl << ":" << res1and4[i].pr << endl;
        if(res2and3[i].confidencesum + res1and4[i].confidencesum>maxconf) {
            mark = i;
            maxconf = res2and3[i].confidencesum + res1and4[i].confidencesum;
        }
    }
    float sumconf=0;
    pconf[0]= res1and4[mark].confidence1;
    pconf[1] = res2and3[mark].confidence1;
    pconf[2] = res2and3[mark].confidence2;
    pconf[3] = res1and4[mark].confidence2;

    ppoints[0] = res1and4[mark].pl;
    ppoints[1] = res2and3[mark].pl;
    ppoints[2] = res2and3[mark].pr;
    ppoints[3] = res1and4[mark].pr;

    for(int i=0; i < 4; i++) {
        sumconf += pconf[i];
    }

    //    char savefilePath[300];
    //    sprintf(savefilePath,"timg_Fopen_result_%f.jpg", sumconf);
    if(system_debug) {
        QString saveFileName = QString("%1timg_Fopen_result_%2.jpg").arg(CollarTestPath).arg(sumconf);
        showRectInImage2(src, ppoints, 4, rectsize, pconf, saveFileName.toStdString().c_str());
    }
}

float calConfByP(Mat &src,int rectsize,Point p,int i);
int compForR(const void*a,const void*b);
void getFistNMiddlePointConf(Mat &src,Point start,Point end,int rectsize,Point points[20],float confidence[20],int n)
{

    float len = getEuclideanDist(start,end);
    int steps = rectsize/4;
    int num = len/steps;
    Point p;
    node confTmp[100];
    for(int i=0;i<=num;i++)
    {
        p.x = (start.x*i+(num-i)*end.x)/num;
        p.y = (start.y*i+(num-i)*end.y)/num;
        confTmp[i].confidence = calConfByP(src,rectsize,p,2);
        confTmp[i].point = p;
    }
    qsort(confTmp,num+2,sizeof(confTmp[0]),compForR);
    for(int i=0;i<n;i++)
    {
        confidence[i] = confTmp[i].confidence;
        points[i] = confTmp[i].point;
    }
}

float calConfByP(Mat &src,int rectsize,Point p,int i)
{
    Mat pointRect = src(Range(p.y-rectsize/2, p.y+rectsize/2),cv::Range(p.x-rectsize/2, p.x+rectsize/2));
    struct inputPredict inputData;
    inputData.data = calHog(pointRect);
    inputData.n = i+1;
    struct outputPredict outputData;
    svm_predict2b(inputData, outputData, modelForR, i);
    return outputData.conf1;
}

int compForR(const void*a,const void*b) {
    return (*(node *)b).confidence > (*(node *)a).confidence ? 1 : -1;

}

void getFirstPointConf( Mat &src, Point center, float startAngle, float endAngle1, float startR, float endR,
                        int rectsize, Point points[], float confidence[], int mark,
                        int headY, int topbodyY, int lshoulderX, int rshoulderX)
{
    float rstep = (endR-startR)/4;
    float anglestep = (endAngle1 - startAngle)/5;

    node confTmp[100];
    Point p;
    int i=0;
    float endAngle = endAngle1+0.05*CV_PI;
    for(float angle=startAngle;angle<=endAngle;angle+=anglestep)
    {
        for(float rlen = startR;rlen<=endR;rlen+=rstep)
        {

            p.x = center.x+rlen*cos(angle);
            p.y = center.y+rlen*sin(angle);
            int yThreshold = (headY*3+topbodyY*5)/8;
            int xLthre = lshoulderX;
            int xRthre = rshoulderX;
            if(p.x<xLthre||p.x>xRthre) continue;
            if(p.y<yThreshold) continue;
            confTmp[i].confidence = calConfByP(src,rectsize,p,mark);
            confTmp[i].point = p;
            i++;
        }
    }
    qsort(confTmp,i,sizeof(confTmp[0]),compForR);

    confidence[0] = confTmp[0].confidence;
    points[0] = confTmp[0].point;
}

int compForV(const void*a,const void*b);
//getFistNMiddlePointConfForV(src,midBodyStart,midBodyEnd,rectsize, points, confidence,n);
void getFistNMiddlePointConfForV(Mat &src,Point start,Point end,
                                 int rectsize,Point points[20], float confidence[20],int n)
{
    int xstart = 0-rectsize,xend = rectsize;
    float len = getEuclideanDist(start,end);
    int steps = rectsize/4;
    int num = len/steps;
    Point p;
    Point iterstart,iterend;
    iterstart.y = start.y;
    iterend.y = end.y;

    node confTmp[500];
    int count=0;
    for(int iter = xstart;iter<=xend;iter+=steps)
    {
        iterstart.x = start.x+iter;
        iterend.x = end.x+iter;
        for(int i=0;i<=num;i++)
        {
            p.x = (iterstart.x*i+(num-i)*iterend.x)/num;
            p.y = (iterstart.y*i+(num-i)*iterend.y)/num;
            confTmp[count].confidence = calConfByPForV(src,rectsize,p,4);
            confTmp[count].point = p;
            count++;
        }
    }
    qsort(confTmp,count,sizeof(confTmp[0]),compForV);
    for(int i=0;i<n;i++)
    {
        confidence[i] = confTmp[i].confidence;
        points[i] = confTmp[i].point;
    }
}

float calConfByPForV(Mat &src,int rectsize,Point p,int i)
{
    Mat pointRect = src(Range(p.y-rectsize/2, p.y+rectsize/2),cv::Range(p.x-rectsize/2, p.x+rectsize/2));
    struct inputPredict inputData;
    inputData.data = calHog(pointRect);
    inputData.n = i+1;
    struct outputPredict outputData;
    svm_predict2b(inputData,outputData,modelForV,i);
    return outputData.conf1;
}

int compForV(const void*a,const void*b) {
    return (*(node *)b).confidence > (*(node *)a).confidence ? 1 : -1;
}


float calConfByPForFopen(Mat &src,int rectsize, Point p, int i)
{
    Mat pointRect = src(Range(p.y-rectsize/2, p.y+rectsize/2),cv::Range(p.x-rectsize/2, p.x+rectsize/2));
    struct inputPredict inputData;
    inputData.data = calHog2(pointRect);
    inputData.n = i+1;
    struct outputPredict outputData;
    svm_predict2b(inputData,outputData,modelForFopen,i);
    return outputData.conf1;
}

float maxconfidenceForFopen(int centeri,int centerj,int nstep,node rrigion[][100],int nwidth,int nheight,Point &p)
{
    float max = 0;
    int marki = -1,markj=-1;
    for(int i=centeri-nstep;i<centeri+nstep;i++) {
        if(i <= nheight && i >= 0) {
            for(int j = centerj-nstep; j < centerj+nstep; j++) {
                if(j<=nwidth&&j>=0) {
                    if(rrigion[i][j].confidence>max) {
                        marki = i;
                        markj = j;
                        max = rrigion[i][j].confidence;
                    }
                }
            }
        }
    }
    p = rrigion[marki][markj].point;
    return rrigion[marki][markj].confidence;
}

int compForFopenRes(const void*a,const void*b) {
    return (*(pairNode *)b).confidencesum > (*(pairNode *)a).confidencesum ? 1 : -1;
}
