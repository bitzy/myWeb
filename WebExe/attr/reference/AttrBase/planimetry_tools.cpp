#include "planimetry_tools.h"
#define EOPS (1E-10)
using namespace cv;

double getEuclideanDist(double p1x, double p1y, double p2x, double p2y) {
    return sqrt((p1x - p2x) * (p1x - p2x) + (p1y - p2y) * (p1y - p2y));
}

double getEuclideanDist(Point p1, Point p2) {
    return sqrt((double)(p1.x - p2.x) * (p1.x - p2.x) + (double)(p1.y - p2.y) * (p1.y - p2.y));
}

double getEuclideanDist(cv::Point3d p1, cv::Point3d p2)
{
    return sqrt((double)(p1.x-p2.x)*(p1.x-p2.x) +
                (double)(p1.y-p2.y)*(p1.y-p2.y) +
                (double)(p1.z-p2.z)*(p1.z-p2.z));
}

double getAngleByThreeP(cv::Point p1, cv::Point p2, cv::Point p3) {
    //get the angle Vec(p2p1) with Vec(p2p3)
    double a_b_x = p1.x - p2.x;
    double a_b_y = p1.y - p2.y;
    double c_b_x = p3.x - p2.x;
    double c_b_y = p3.y - p2.y;
    double ab_mul_cb = a_b_x * c_b_x + a_b_y * c_b_y;
    double dist_ab = sqrt(a_b_x * a_b_x + a_b_y * a_b_y);
    double dist_cd = sqrt(c_b_x * c_b_x + c_b_y * c_b_y);
    double cosValue = ab_mul_cb / (dist_ab * dist_cd);
    return acos(cosValue);
}



/*
 *  give three point, p1, p2, p3. return the angle between p1p2 and p2p3.
 *  the angle is 0~ 2 * pi.
 */
double getAngleByThreeP(double pointx[3], double pointy[3]) {
    double a_b_x = pointx[0] - pointx[1];
    double a_b_y = pointy[0] - pointy[1];
    double c_b_x = pointx[2] - pointx[1];
    double c_b_y = pointy[2] - pointy[1];
    double ab_mul_cb = a_b_x * c_b_x + a_b_y * c_b_y;
    double dist_ab = sqrt(a_b_x * a_b_x + a_b_y * a_b_y);
    double dist_cd = sqrt(c_b_x * c_b_x + c_b_y * c_b_y);
    double cosValue = ab_mul_cb / (dist_ab * dist_cd);
    return acos(cosValue);
}

double getAngleByThreP2(cv::Point p1, cv::Point p2, cv::Point p3) {
    //get angle p1p2 with p2p3
    double a_b_x = p2.x - p1.x;
    double a_b_y = p2.y - p1.y;
    double c_b_x = p3.x - p2.x;
    double c_b_y = p3.y - p2.y;
    double ab_mul_cb = a_b_x * c_b_x + a_b_y * c_b_y;
    double dist_ab = sqrt(a_b_x * a_b_x + a_b_y * a_b_y);
    double dist_cd = sqrt(c_b_x * c_b_x + c_b_y * c_b_y);
    double cosValue = ab_mul_cb / (dist_ab * dist_cd);
    return acos(cosValue);
}

double getAngleL1toL2(cv::Point p1, cv::Point p2, cv::Point p3) {
    double k1,k2,tanAngle,angle;
    if(p1.x==p2.x) {
        k1=10000;
    } else {
        k1 = (p2.y-p1.y)*1.0/(p2.x-p1.x);
    }

    if(p2.x==p3.x) {
        k2=10000;
    } else {
        k2 = (p3.y-p2.y)*1.0/(p3.x-p2.x);
    }
    tanAngle = (k2-k1)/(1+k1*k2);
    angle = atan(tanAngle);
    return angle;
}

double getVectorGrad(cv::Point p1, cv::Point p2) {
    if(abs(p2.x - p1.x) < EOPS) {
        if((p2.y - p1.y) < 0) {
            return 0 - CV_PI / 2.0;
        }
        return CV_PI / 2.0;
    }

    double alpha = atan((p2.y - p1.y)* 1.0 / ((p2.x - p1.x) ));
    if(p2.x - p1.x < 0) {
        alpha += CV_PI;
        if(alpha > CV_PI) {
            alpha = alpha - 2 * CV_PI;
        }
    }

    return alpha;
}

double getVectorGrad(double p1x, double p1y, double p2x, double p2y) {
    if(fabs(p2x - p1x) < EOPS) {
        if(p2y - p1y < 0) {
            return 0 - CV_PI / 2.0;
        }
        return CV_PI / 2.0;
    }

    double alpha = atan((p2y - p1y) / (p2x - p1x));
    if(p2x - p1x < 0) {
        alpha = alpha + CV_PI;
        if(alpha > CV_PI) {
            alpha = alpha - 2 * CV_PI;
        }
    }
    return alpha;
}
/* if(p1.y - p2.y == 0)
return CV_PI / 2.0;

return -atan((double)(p1.x - p2.x) / (double)(p1.y - p2.y));
*/

double getInclinationAngle(cv::Point p1, cv::Point p2) {
    if(abs(p2.x - p1.x) < EOPS) {
        if((p2.y - p1.y) < 0) {
            return 0 - CV_PI / 2.0;
        }
        return CV_PI / 2.0;
    }

    double alpha = atan((p2.y - p1.y)* 1.0 / ((p2.x - p1.x) ));
    return alpha;
}

double getDistFromP2L(cv::Point p1, cv::Point p2, cv::Point p3) {
    double y2_y1 = p3.y - p2.y;
    double x2_x1 = p3.x - p2.x;

    if(fabs(y2_y1) < EOPS && fabs(y2_y1) < EOPS) {
        return 0.0;
    }

    return abs(y2_y1 * (p1.x - p2.x) - x2_x1 * (p1.y - p2.y))*1.0
            / sqrt(y2_y1 * y2_y1 + x2_x1 * x2_x1);
}

double getDistFromP2L(cv::Point p1, cv::Point p2, double angle) {
    int len = 50;
    cv::Point p3(p2.x+len*cos(angle),p2.y+len*sin(angle));
    double y2_y1 = p3.y - p2.y;
    double x2_x1 = p3.x - p2.x;

    if(fabs(y2_y1) < EOPS && fabs(y2_y1) < EOPS) {
        return 0.0;
    }

    return abs(y2_y1 * (p1.x - p2.x) - x2_x1 * (p1.y - p2.y))*1.0
            / sqrt(y2_y1 * y2_y1 + x2_x1 * x2_x1);
}

double getDistFromP2L(
        double px, double py,
        double p1x, double p1y,
        double p2x, double p2y)
{
    double y2_y1 = p2y - p1y;
    double x2_x1 = p2x - p1x;

    if(fabs(y2_y1) < EOPS && fabs(y2_y1) < EOPS) {
        return 0.0;
    }

    return fabs(y2_y1 * (px - p1x) - x2_x1 * (py - p1y))
            / sqrt(y2_y1 * y2_y1 + x2_x1 * x2_x1);
}



void getCircleFrom3P(cv::Point p1, cv::Point p2, cv::Point p3, double &r , cv::Point &cir_O)
{
    double x1Px2 = (p1.x + p2.x) / 2.0;
    double y1Py2 = (p1.y + p2.y) / 2.0;
    double x2Px3 = (p2.x + p3.x) / 2.0;
    double y2Py3 = (p2.y + p3.y) / 2.0;
    double Kx1x2, Kx2x3;
    bool tag = true;

    if(p1.y - p2.y == 0) {
        cir_O.x = (p1.x + p2.x) / 2.0;
        Kx2x3 = -1 * (p2.x - p3.x) / (double)(p2.y - p3.y);
        cir_O.y = (cir_O.x - x2Px3) * Kx2x3 + y2Py3;
        tag = false;
    } else {
        Kx1x2 = -1 * (p1.x - p2.x) / (double)(p1.y - p2.y);
    }

    if(tag) {
        if(p2.y - p3.y == 0) {
            cir_O.x = (p2.x + p3.x) / 2.0;
            cir_O.y = (cir_O.x - x1Px2) * Kx1x2 + y1Py2;
        } else {
            Kx2x3 = -1 * (p2.x - p3.x) / (double)(p2.y - p3.y);
#ifdef DEBUG
            printf("%3lf %3lf\n", Kx1x2, Kx2x3);
            printf("%.3lf", fabs(Kx1x2 - Kx2x3));
            printf("assert kx1x2 - kx2x3 > 0");
#endif
            //   assert(Kx1x2 - Kx2x3 != 0);
            cir_O.x = (x1Px2 * Kx1x2 - Kx2x3 * x2Px3 + y2Py3 - y1Py2) / (double)(Kx1x2 - Kx2x3);
            cir_O.y = y1Py2 + Kx1x2 * (cir_O.x - x1Px2);
        }
    }

    r = sqrt((cir_O.x - p1.x) * (cir_O.x - p1.x) + (cir_O.y - p1.y) * (cir_O.y - p1.y));
    return;
}

void getCircleFrom3P(double *x, double *y, double &r, double &x0, double &y0)
{
    double x1Px2 = (x[0] + x[1]) / 2;
    double y1Py2 = (y[0] + y[1]) / 2;
    double x2Px3 = (x[1] + x[2]) / 2;
    double y2Py3 = (y[1] + y[2]) / 2;
    double Kx1x2, Kx2x3;
    bool tag = true;

    if(fabs(y[0] - y[1]) < EOPS) {
        x0 = (x[0] + x[1]) / 2;
        assert(fabs(y[1] - y[2]) > EOPS);
        Kx2x3 = -1 * (x[1] - x[2]) / (y[1] - y[2]);
        y0 = (x0 - x2Px3) * Kx2x3 + y2Py3;
        tag = false;
    } else {
        Kx1x2 = -1 * (x[0] - x[1]) / (y[0] - y[1]);
    }

    if(tag) {
        if((fabs(y[1] - y[2]) < EOPS)) {
            x0 = (x[1] + x[2]) / 2;
            y0 = (x0 - x1Px2) * Kx1x2 + y1Py2;
        } else {
            Kx2x3 = -1 * (x[1] - x[2]) / (y[1] - y[2]);
            printf("%3lf %3lf\n", Kx1x2, Kx2x3);
            printf("%.3lf", fabs(Kx1x2 - Kx2x3));
            printf("assert kx1x2 - kx2x3 > 0");
            assert(fabs(Kx1x2 - Kx2x3) > EOPS);
            x0 = (x1Px2 * Kx1x2 - Kx2x3 * x2Px3 + y2Py3 - y1Py2) / (Kx1x2 - Kx2x3);
            y0 = y1Py2 + Kx1x2 * (x0 - x1Px2);
        }
    }

    r = sqrt((x0 - x[0]) * (x0 - x[0]) + (y0 - y[0]) * (y0 - y[0]));
    return;
}

double getTheta(cv::Point p1, cv::Point p2)
{
    if(p1.y - p2.y == 0)
        return CV_PI / 2.0;

    return -atan((double)(p1.x - p2.x) / (double)(p1.y - p2.y));
}

double getVectMultiply(cv::Point p1,cv::Point p2,cv::Point p3)
{
    int ax = p2.x - p1.x,
            ay = p2.y - p1.y,
            bx = p3.x - p2.x,
            by = p3.y - p2.y;
    return ax*bx + ay*by;
}
double getVectp1p2Multip1p3(cv::Point p1,cv::Point p2,cv::Point p3)
{
    int ax = p2.x - p1.x,
            ay = p2.y - p1.y,
            bx = p3.x - p1.x,
            by = p3.y - p1.y;
    return ax*bx + ay*by;
}

cv::Point getP1Projection(cv::Point p1,cv::Point p2,cv::Point p3)
{
    cv::Point target;
    if(p2.x == p3.x)
    {
        return cv::Point(p2.x,p1.y);
    }
    else
    {
        double k = (p3.y-p2.y)*1.0/(p3.x-p2.x);
        target.x = (int)((k * p2.x + p1.x / k + p1.y - p2.y) / (1 / k + k));
        target.y = (int)(-1 / k * (target.x - p1.x) + p1.y);
        return target;
    }
}

double getAngleByFourP(cv::Point p1,cv::Point p2,cv::Point p3,cv::Point p4)
{
    double a_x = p2.x - p1.x;
    double a_y = p2.y - p1.y;
    double c_x = p4.x - p3.x;
    double c_y = p4.y - p3.y;
    double ab_mul_cb = a_x * c_x + a_y * c_y;
    double dist_ab = sqrt(a_x * a_x + a_y * a_y);
    double dist_cd = sqrt(c_x * c_x + c_y * c_y);
    double cosValue = ab_mul_cb / (dist_ab * dist_cd);
    return acos(cosValue);
}

std::vector<float> calHog(cv::Mat src)
{
    int n = 16;
    double xscale = n*1.0/(src.cols);
    double yscale = n*1.0/src.rows;
    cv::resize(src,src,cv::Size(0,0),xscale,yscale);
    HOGDescriptor *hog = new HOGDescriptor(cvSize(src.cols,src.rows),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);
    vector<float>descriptors;
    hog->compute(src, descriptors,Size(1,1), Size(0,0));
    return descriptors;
}

std::vector<float> calHog2(cv::Mat src)
{
    int n = 32;
    double xscale = n*1.0/(src.cols);
    double yscale = n*1.0/src.rows;
    cv::resize(src,src,cv::Size(0,0),xscale,yscale);
    HOGDescriptor *hog = new HOGDescriptor(cvSize(src.cols,src.rows),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);
    vector<float>descriptors;
    hog->compute(src, descriptors,Size(1,1), Size(0,0));
    return descriptors;
}

void getFileName(char fullFilePath[],char filename[])
{
    char tmpPath[200];
    strcpy(tmpPath,fullFilePath);
    int i;
    for(i = strlen(tmpPath)-1;tmpPath[i]!='.';i--);tmpPath[i] = 0;
    tmpPath[i]=0;
    i--;
    for(;tmpPath[i]!='\\';i--);
    strcpy(filename,&tmpPath[i+1]);
}

void showSingleLine(cv::Mat &src,cv::Point start,cv::Point end, const char saveFilePath[])
{
    cv::Mat src2;
    src.copyTo(src2);

    cv::line(src2,start,end,cv::Scalar(rand()%256,rand()%256,rand()%256),2);

    cv::imwrite(saveFilePath,src2);
}

void showRectInImage2(cv::Mat src, cv::Point points[], int pointsSize, int rectsize,
                      float confidence[], const char saveFilePath[])
{
    cv::Mat src2;
    src.copyTo(src2);
    for(int i = 0;i < pointsSize; i++) {
        char text[10];
        sprintf(text,"%d",i+1);
        rectangle(src2,cv::Point(points[i].x-rectsize/2,points[i].y-rectsize/2),
                  cv::Point(points[i].x+rectsize/2,points[i].y+rectsize/2),
                  cv::Scalar(rand()%256,rand()%256,rand()%256),2);
        putText(src2,text,points[i],CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 1);
        sprintf(text,"%.2f",confidence[i]);
        putText(src2,text,cv::Point(points[i].x+rectsize/2,points[i].y+rectsize/2),CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(rand()%256,rand()%256,rand()%256), 1);
    }
    cv::imwrite(saveFilePath,src2);
}

void getMaxByThreeValue(int a[3],int &k)
{
    int mark=-1;
    if(a[0]>a[1]) {
        mark = a[0];
        k=0;
    } else {
        mark = a[1];
        k=1;
    }
    if(mark<a[2]) {
        mark = a[2];
        k=2;
    }
}

void getMaxByThreeValue(double a[3],int &k) {
    double mark=-1;
    if(a[0]>a[1]) {
        mark = a[0];
        k=0;
    } else {
        mark = a[1];
        k=1;
    }
    if(mark<a[2]) {
        mark = a[2];
        k=2;
    }
}

void getMaxByThreeValue(double a0,double a1,double a2,int &k)
{
    double mark=-1;
    if(a0>a1) {
        mark = a0;
        k=0;
    } else {
        mark = a1;
        k=1;
    }
    if(mark<a2) {
        mark = a2;
        k=2;
    }
}

void getMinByThreeValue(int a0,int a1,int a2,int &k)
{
    int mark=-1;
    if(a0>a1)
    {
        mark = a1;
        k=1;
    }
    else
    {
        mark = a0;
        k=0;
    }
    if(mark>a2)
    {
        mark = a2;
        k=2;
    }
}

int colorDist(int i, int j)
{
    int r1, r2, g1, g2, b1, b2;
    int rmean,r,g,b;

    r1 = (i & 0x003f) << 2;
    r2 = (j & 0x003f) << 2;
    g1 = ((i >> 6) & 0x003f) << 2;
    g2 = ((j >> 6) & 0x003f) << 2;
    b1 = ((i >> 12) & 0x003f) << 2;
    b2 = ((j >> 12) & 0x003f) << 2;

    rmean = (r1 + r2) / 2;
    r = r1 - r2;
    g = g1 - g2;
    b = b1 - b2;
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}

int colorDist(int rgb1[],int rgb2[])
{
    int rmean,r,g,b;
    rmean = (rgb1[0] + rgb2[0]) / 2;
    r = rgb1[0] - rgb2[0];
    g = rgb1[1] - rgb2[1];
    b = rgb1[2] - rgb2[2];
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}

int colorDist( int r1,int r2,int g1,int g2,int b1,int b2)
{
    int rmean,r,g,b;
    rmean = (r1 + r2) / 2;
    r = r1 - r2;
    g = g1 - g2;
    b = b1 - b2;
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}


int colorDist(uchar rgb1[],uchar rgb2[])
{
    int rmean,r,g,b;
    rmean = (rgb1[0] + rgb2[0]) / 2;
    r = rgb1[0] - rgb2[0];
    g = rgb1[1] - rgb2[1];
    b = rgb1[2] - rgb2[2];
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}

int colorDist( uchar r1,uchar r2,uchar g1,uchar g2,uchar b1,uchar b2)
{
    int rmean,r,g,b;
    rmean = (r1 + r2) / 2;
    r = r1 - r2;
    g = g1 - g2;
    b = b1 - b2;
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}

void getMinColorDistIndex(ColorFeat2 feat1[],ColorFeat2 feat2[],const int featsize,int &index1,int &index2)
{
    int mindist = INT_MAX;
    int tmpdist;
    for(int i=0;i<featsize;i++)
    {
        for(int j=0;j<featsize;j++)
        {
            if(feat1[i].per>0&&feat2[j].per>0)
            {
                tmpdist = colorDist(feat1[i].rgb,feat2[j].rgb);
                if(tmpdist<mindist)
                {
                    mindist = tmpdist;
                    index1 = i;
                    index2 = j;
                }
            }
        }
    }
}

double getColorFeatureDist(ColorFeat2 feat1[],ColorFeat2 feat2[],const int featsize)
{
    ColorFeat2 ft1[10],ft2[10];
    double colFeatureDist = 0;

    for(int i=0;i<featsize;i++)
    {
        ft1[i] = feat1[i];
        ft2[i] = feat2[i];
    }
    int index1,index2;

    for(int i=0;i<featsize;i++)
    {
        index1 = -1;
        index2=-1;
        getMinColorDistIndex(ft1,ft2,featsize,index1,index2);
        if(index1 > -1 && index2 > -1)
        {
            int tmpdist = colorDist(ft1[index1].rgb,ft2[index2].rgb);
            int minPer = ft1[index1].per > ft2[index2].per ? ft2[index2].per : ft1[index1].per;
            colFeatureDist = colFeatureDist + tmpdist*minPer;

            ft1[index1].per = ft1[index1].per - minPer;
            ft2[index2].per = ft2[index2].per - minPer;
        }
    }
    return colFeatureDist/1000.0;
}

void imageRGB2HSI(const Mat& image, Mat &hsi)
{
    int nl = image.rows;
    int nc = image.cols;
    if(image.isContinuous()){
        nc = nc * nl;
        nl = 1;
    }

    hsi.create(nl, nc, image.type());
    float b, g, r, h, s, in;
    for(int i = 0; i < nl; i++) {
        const uchar * src = image.ptr<uchar>(i);
        uchar * dst = hsi.ptr<uchar>(i);
        for(int j = 0; j < nc; j++) {
            b = src[j*3];
            g = src[j*3+1];
            r = src[j*3+2];

            //I
            in = (r+g+b)/3;

            //S
            int min_val = 0;
            min_val = std::min(r, std::min(b, g));
            s = 1 - 3*(min_val/(b+g+r));
            if(s < 0.00001) {
                s = 0;
            } else if(s > 0.99999) {
                s = 1;
            }

            //H
            if(s != 0) {
                h = 0.5*((r-g)+(r-b))/sqrt((r-g)*(r-g) + (r-b)*(g-b));
                h = acos(h);
                if(b > g) {
                    h = 2*CV_PI - h;
                }
            } else h = 0;
            dst[3*j] = (h*180)/CV_PI; //H
            dst[3*j+1] = s; //S
            dst[3*j+2] = in;//I
        }
    }//over;
}

int getLinePoint(const Point &start, const Point &end, vector<Point>& res)
{
    double dist= getEuclideanDist(start, end);
    double addx = (end.x - start.x)/dist;
    double addy = (end.y - start.y)/dist;

    res.clear();
    for(int i = 0; ;i++) {
        Point p(int(start.x + i*addx), int(start.y + i*addy));
        res.push_back(p);
        if(getEuclideanDist(p, end) < 2) break;
    }
    return res.size();
}

bool vectorHasExist(const vector<int>& vec, int x) {
    int size = vec.size();
    for(int i = size-1; i >= 0; i--) {
        if(vec.at(i) == x) return true;
    }
    return false;
}

/**
 * @brief vectorCountFindNxt
 * @param vec
 * @param x
 * @return
 */
int vectorCountFindNxt(vector<int>& vec, int& idx, int& pos) {
    int count = 1;
    bool flag = false;

    int size = vec.size();
    int value = vec.at(idx);
    pos = idx;
    for(int i = idx+1; i < size; i++) {
        if(vec.at(i) == -1) continue;
        if(vec.at(i) == value) {
            count++;
            vec.at(i) = -1;
            pos = i;
            continue;
        }
        if(!flag) { idx = i; flag = true;}
    }
    if(!flag) idx = size;
    return count;
}

double getAngle180ByThreeP(Point p1, Point p2, Point p3)
{
    double angle180;
    //get the angle Vec(p2p1) with Vec(p2p3)
    double a_b_x = p1.x - p2.x;
    double a_b_y = p1.y - p2.y;
    double c_b_x = p3.x - p2.x;
    double c_b_y = p3.y - p2.y;
    double ab_mul_cb = a_b_x * c_b_x + a_b_y * c_b_y;
    double dist_ab = sqrt(a_b_x * a_b_x + a_b_y * a_b_y);
    double dist_cd = sqrt(c_b_x * c_b_x + c_b_y * c_b_y);
    double cosValue = ab_mul_cb / (dist_ab * dist_cd);
    angle180 = acos(cosValue) * 180 / CV_PI;
    return angle180;
}
