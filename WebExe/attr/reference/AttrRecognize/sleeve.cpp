#include <QDebug>
#include "attrRecognize.h"

QString SleeveTestPath = SYSTEST_DIR;/*QString("%1Sleeve/").arg(SYSTEST_DIR);*/

void addPointToVec(const cv::Point start, const cv::Point end,
                   const unsigned int n, std::vector<cv::Point> &points);
int colorIsSkin(int red,int green,int blue);
void getSkinMat1(cv::Mat src, cv::Mat &dst);
void getSkinMat2(cv::Mat src, cv::Mat &dst);
int checkRigion(cv::Mat &skinMat,const cv::Point p, const float pointPercentThre);

extern bool system_debug;
////////////////////////////////////////////////////
/// \brief sleeveAnalyze
/// \param fpath: dealImage full path;
/// \param loc: pose data;
/// \return
///
int baseWaySleeve(const char * fpath, MyLocation* loc)
{
    string fpathStr(fpath);
    Mat src = imread(fpathStr);

    std::vector<cv::Point> lsamplePoint,rsamplePoint;
    std::vector<int >ltag,rtag;
    const unsigned int sampleN = 20;

    lsamplePoint.reserve(sampleN*2);
    rsamplePoint.reserve(sampleN*2);
    ltag.reserve(sampleN*2);
    rtag.reserve(sampleN*2);

    loadPoseData(loc);

    //sample point to lsamplePoint:
    addPointToVec(lshoulder, lelbow, sampleN, lsamplePoint);
    addPointToVec(lelbow, lhand, sampleN, lsamplePoint);
    //sample point to rsamplePoint:
    addPointToVec(rshoulder, relbow, sampleN, rsamplePoint);
    addPointToVec(relbow, rhand, sampleN, rsamplePoint);

    //get skin color:
    cv::Mat skin1;
    cv::Mat skin2;
    getSkinMat1(src, skin1);
    getSkinMat2(src, skin2);
    cv::Mat skinMat(src.size(),CV_8UC1);
    for(int i=0; i < skinMat.rows; i++) {
        uchar *ptr1 =  (uchar *)skin1.ptr<uchar>(i);
        uchar *ptr2 =  (uchar *)skin2.ptr<uchar>(i);
        uchar *ptr =  (uchar *)skinMat.ptr<uchar>(i);
        for(int j=0; j < skinMat.cols; j++) {
            ptr[j] = ptr1[j]&ptr2[j];
        }
    }
    QString saveFileName;
    if(system_debug) {
        saveFileName = QString("%1timg_sleeveFeature_skin1.jpg").arg(SleeveTestPath);
        imwrite(saveFileName.toStdString().c_str(), skin1);

        saveFileName = QString("%1timg_sleeveFeature_skin2.jpg").arg(SleeveTestPath);
        imwrite(saveFileName.toStdString().c_str(), skin2);

        saveFileName = QString("%1timg_sleeveFeature_skin.jpg").arg(SleeveTestPath);
        imwrite(saveFileName.toStdString().c_str(), skinMat);
    }


    //get sample dots tag:
    int skinPointPercentThre = 0.6;
    for(unsigned int i=0; i < sampleN*2; i++) {
        ltag.push_back(checkRigion(skinMat, lsamplePoint[i], skinPointPercentThre));
        rtag.push_back(checkRigion(skinMat, rsamplePoint[i], skinPointPercentThre));
    }

    //skin tag revise ==> 1-20
    for(unsigned int i=0; i < sampleN; i++) {
        //left arm:
        int sumtmp = 0;
        if(1 == ltag[i]) {
            for(unsigned int j=i; j < i+15; j++) {
                sumtmp += ltag[j];
            }
            if(sumtmp < 8) ltag[i] = 0;
        }
        //right arm:
        sumtmp = 0;
        if(1 == rtag[i]) {
            for(unsigned int j=i; j < i+15; j++) {
                sumtmp += rtag[j];
            }
            if(sumtmp < 8) rtag[i] = 0;
        }
    }
    //skin tag revise ==> 21-40
    for(unsigned int i = sampleN; i < 2*sampleN; i++) {
        //left bottom arm:
        int sumtmp = 0;
        if(0 == ltag[i]) {
            for(unsigned int j=i; j > i-10; j--) {
                sumtmp+=ltag[j];
            }
            if(sumtmp > 7) ltag[i]=1;
        }
        //right bottom arm:
        sumtmp = 0;
        if(0 == rtag[i]) {
            for(unsigned int j=i; j > i-10; j--) {
                sumtmp+=rtag[j];
            }
            if(sumtmp > 7) rtag[i]=1;
        }
    }

    if(system_debug) {
        // output tag result:
        Mat src2;
        src.copyTo(src2);
        for(unsigned int i=0; i<ltag.size(); i++) {
            if(1 == ltag[i]) {
                circle(src2,lsamplePoint[i],2,Scalar(0,0,0),2);
            } else {
                circle(src2,lsamplePoint[i],2,Scalar(0,0,255),2);
            }
        }
        for(unsigned int i=0; i<rtag.size(); i++) {
            if(1 == rtag[i]) {
                circle(src2,rsamplePoint[i],2,Scalar(0,0,0),2);
            } else {
                circle(src2,rsamplePoint[i],2,Scalar(0,0,255),2);
            }
        }
        //    sprintf(savefilepath,"timg_sleeveFeature_result.jpg");
        saveFileName.clear();
        saveFileName = QString("%1timg_sleeveFeature_result.jpg").arg(SleeveTestPath);
        imwrite(saveFileName.toStdString().c_str(), src2);
    }

    //calculate the sleeve data number:
    int lper = 0, rper = 0;
    for(std::vector<int>::iterator iter = ltag.begin(); iter != ltag.end(); iter++) {
        if(*iter != 1)
            lper++;
        else break;
    }
    for(std::vector<int>::iterator iter =rtag.begin(); iter != rtag.end(); iter++) {
        if(*iter != 1)
            rper++;
        else break;
    }

    //analyze res & return the final judgement:
    int sleeve = (lper + rper)*10/8;
    if(sleeve < 5) {//s_no;
        return 0;
    } else if(sleeve < 33) {
        return 1;//s_short;
    } else if(sleeve < 66) {
        return 2;//s_half;
    } else {
        return 3;//s_long;
    }
}

void addPointToVec(
        const cv::Point start,
        const cv::Point end,
        const unsigned int n,
        std::vector<cv::Point> &points)
{
    assert(n > 1);
    const float N = n - 1;

    for(unsigned int i = 0; i <= n ; i++) {
        points.push_back((end * (i / N)) + (start * ((N - i) / N)));
    }
}

void getSkinMat1(cv::Mat src, cv::Mat &dst)
{    
    dst = Mat::zeros(src.size(), CV_8UC1);

    int rows = src.rows;
    int cols = src.cols;
    for(int i = 0; i < rows; i++) {
        uchar* pointer = src.ptr<uchar>(i);
        uchar* skinP = dst.ptr<uchar>(i);

        for(int j = 0; j < cols; j++) {
            int blue = *pointer, green = *(pointer + 1), red = *(pointer + 2);
            if(colorIsSkin(red, green, blue)) *skinP = 255;
            pointer += 3;
            skinP ++;
        }
    }
}

void getSkinMat2(cv::Mat src, cv::Mat &dst)
{
    Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
    ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0,
            Scalar(255, 255, 255), -1);

    Mat ycrcb_image;
    cvtColor(src, ycrcb_image, CV_BGR2YCrCb);

    dst = Mat::zeros(src.size(), CV_8UC1);
    for(int i = 0; i < src.rows; i++) {
        Vec3b *ycrcb = (Vec3b*) ycrcb_image.ptr<Vec3b>(i);
        uchar *p = (uchar *) dst.ptr<uchar>(i);
        for(int j = 0; j < src.cols; j++) {
            if(skinCrCbHist.at<uchar>(ycrcb[j][1], ycrcb[j][2]) > 0)
                p[j] = 255;
        }
    }
}

int colorIsSkin(int red,int green,int blue)
{    
    if(red > 95 && green > 40 && blue > 20
            && red > blue && red > green
            && abs(red - green) > 15) {
        int max = red;
        int min = (green > blue? blue: green);
        if(max - min > 15) return 1;
    }
    return 0;
}

int checkRigion(cv::Mat &skinMat,const cv::Point p, const float pointPercentThre)
{
    int pointSum = 0;
    int skinPointSum = 0;
    for(int i=-2; i<=2; i++) {
        int row = p.y + i;
        if( (row >= 0) && (row < skinMat.rows)) {
            uchar *ptr = skinMat.ptr<uchar>(row);
            for(int j=-2; j<=2; j++) {
                int col = p.x + j;
                if( (col >= 0) && (col < skinMat.cols)) {
                    pointSum++;
                    if(ptr[col] > 128) skinPointSum++;
                }
            }
        }
    }
    if((skinPointSum*1.0 / pointSum) > pointPercentThre) return 1;
    return 0;
}
