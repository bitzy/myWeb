#include "imagesp.h"
#include "hist2DTool.h"
#include "attrRecognize.h"

#include "../AttrBase/planimetry_tools.h"
SPCELL::SPCELL()
{
    TheSpIdx = -1;
    ThePixels.clear();
}

void SPCELL::setCellBase(int spIdx, const vector<Point> &pos)
{
    TheSpIdx = spIdx;

    int size = pos.size();
    for(int i = 0; i < size; i++) {
        ThePixels.push_back(pos.at(i));
    }
}

void SPCELL::setCenter(const Point &p)
{
    TheCenter = p;
}

void SPCELL::setColor(const Point3d &value)
{
    TheSpColor = value;
}

void SPCELL::setColor(const Vec3b &value)
{
    TheSpColor.x = value[2];
    TheSpColor.y = value[1];
    TheSpColor.z = value[0];
}

int SPCELL::getIdx()
{
    return TheSpIdx;
}

vector<Point> &SPCELL::getPixels()
{
    return ThePixels;
}

int SPCELL::getPixelsSize()
{
    return ThePixels.size();
}

Point &SPCELL::getPixel(unsigned int j)
{
    assert(j < ThePixels.size());
    return ThePixels.at(j);
}

Point &SPCELL::getCenter()
{
    return TheCenter;
}

Point3d &SPCELL::getColor()
{
    return TheSpColor;
}

//==========================================================================
SPIMAGE::SPIMAGE(const Mat& src, const Mat& seg2D)
{
    src.copyTo(TheSrc);

    if(seg2D.type() != CV_32SC1)
        seg2D.convertTo(TheSeg, CV_32SC1);
    else
        seg2D.copyTo(TheSeg);

    double minValue, maxValue;
    int minIdx, maxIdx;
    minMaxLoc(seg2D, &minValue, &maxValue);
    minIdx = int(minValue);
    maxIdx = int(maxValue);

    spInfo.clear();
    vector<Point> pos;
    for(int i = minIdx; i <= maxIdx; i++){
        getPosFromSegByIdx(i, pos);
        int size = pos.size();
        if(size > 0) {
            addSpCell(i, pos);
            spInfo.back().setCenter(pos.at(size/2));
        }
    }
}

void SPIMAGE::addSpCell(int spIdx, const vector<Point> &pos)
{
    SPCELL spCellnew;
    spCellnew.setCellBase(spIdx, pos);
    spInfo.push_back(spCellnew);
}

/**
 * @brief SPIMAGE::setColorByCenterForEachSp
 * @param img2D
 */
void SPIMAGE::setColorByCenterForEachSp()
{
    int size = spInfo.size();
    for(int i = 0; i < size; i++) {//each sp;
        Point center = spCellGetCenter(i);
        getSpCell(i).setColor(TheSrc.at<Vec3b>(center.y, center.x));
    }
}

/**
 * @brief SPIMAGE::setColorByHistomForEachSp
 * @param img2D
 */
void SPIMAGE::setColorByHistomForEachSp()
{
    for(size_t i = 0; i < spInfo.size(); i++) {
        int dots = getSpCell(i).getPixelsSize();

        //cvt the sp area ==> hsv;
        Mat imgSpLine;
        imgSpLine.create(Size(1, dots), CV_8UC3);
        for(int j = 0; j < dots; j++) {
            Point pos = getSpCellIdxPixelAt(i, j);
            Vec3b color = TheSrc.at<Vec3b>(pos.y, pos.x);
            imgSpLine.at<Vec3b>(1, j) = color;
        }
        cvtColor(imgSpLine, imgSpLine, CV_BGR2HSV);

        //get the h/s maximum value;
        //get the v maximum value;
        HistoGram2D histTool;
        Point res;
        histTool.getHistogramMaxHS(imgSpLine, res);
        int V_num;
        histTool.getHistogram1DMax(imgSpLine, V_num);

        //cvt the hsv value ==> rgb;
        Mat tmp;
        tmp.create(Size(1,1), CV_8UC3);
        tmp.at<Vec3b>(0,0) = Vec3b(res.x, res.y, V_num);
        cvtColor(tmp, tmp, CV_HSV2BGR);
        getSpCell(i).setColor(tmp.at<Vec3b>(0,0));
        // set hsv color;
        //        getSpCell(i).setColor(Point3d(res.x, res.y, V_num));
    }
}

SPCELL &SPIMAGE::getSpCell(unsigned int i)
{
    assert(i < spInfo.size());
    return spInfo.at(i);
}

SPCELL &SPIMAGE::getSpCellByIdx(int spIdx)
{
    int i, size = spInfo.size();
    for(i = 0; i < size; i++) {
        if(getSpCell(i).getIdx() == spIdx) break;
    }
    assert(i < size);
    return getSpCell(i);
}

int SPIMAGE::getSpNum()
{
    return spInfo.size();
}

int SPIMAGE::spCellGetIdx(unsigned int i)
{
    assert(i < spInfo.size());
    return getSpCell(i).getIdx();
}

Point &SPIMAGE::spCellGetCenter(unsigned int i)
{
    assert(i < spInfo.size());
    return getSpCell(i).getCenter();
}

Point3d &SPIMAGE::spCellGetColor(unsigned int i)
{
    assert(i < spInfo.size());
    return getSpCell(i).getColor();
}

vector<Point> &SPIMAGE::spCellGetPixelsByIdx(int spIdx)
{
    return getSpCellByIdx(spIdx).getPixels();
}

Point &SPIMAGE::getSpCellIdxPixelAt(unsigned int i, int j)
{
    assert(i < spInfo.size());
    return getSpCell(i).getPixel(j);
}

Point &SPIMAGE::spCellGetCenterByIdx(int spIdx)
{
    return getSpCellByIdx(spIdx).getCenter();
}

Point3d &SPIMAGE::spCellGetColorByIdx(int spIdx)
{
    return getSpCellByIdx(spIdx).getColor();
}

void SPIMAGE::getPosFromSegByIdx(int spIdx, vector<Point>& pos)
{
    pos.clear();

    for(int i = 0; i < TheSeg.rows; i++) {
        int* ptr = TheSeg.ptr<int>(i);
        for(int j = 0; j < TheSeg.cols; j++) {
            if(ptr[j] == spIdx) pos.push_back(Point(j, i));
        }
    }
}

void SPIMAGE::setMaskByIdx(Mat& mask, int spIdx)
{
    vector<Point> pos = spCellGetPixelsByIdx(spIdx);
    int size = pos.size();
    for(int i = 0; i < size; i++) {
        Point tmp = pos.at(i);
        mask.at<uchar>(tmp.y, tmp.x) = 1;
    }
}

void SPIMAGE::setMaskByPos(Mat &mask, vector<Point> &pos)
{
    int size = pos.size();
    for(int i = 0; i < size; i++) {
        Point tmp = pos.at(i);
        mask.at<uchar>(tmp.y, tmp.x) = 1;
    }
}

Mat SPIMAGE::getMaskByIdxs(const vector<int>& spIdxs) {
    Mat mask(TheSrc.size(), CV_8U, Scalar(0));
    int spsNum = spIdxs.size();
    for(int i = 0; i < spsNum; i++) {
        setMaskByIdx(mask, spIdxs.at(i));
    }
    return mask;
}

void SPIMAGE::getSpColorImage(const char* outName)
{
    Mat srcRes;
    srcRes.create(TheSrc.size(), TheSrc.type());

    for(size_t i = 0; i < spInfo.size(); i++) {//each super pixel;
        Point3d color = spCellGetColor(i);
        int dots = getSpCell(i).getPixelsSize();
        for(int j = 0; j < dots; j++) {
            Point aix = getSpCellIdxPixelAt(i, j);
            srcRes.at<Vec3b>(aix.y, aix.x)[0] = color.z;//b
            srcRes.at<Vec3b>(aix.y, aix.x)[1] = color.y;//g
            srcRes.at<Vec3b>(aix.y, aix.x)[2] = color.x;//r
        }
    }
    imwrite(outName, srcRes);
}

void SPIMAGE::getLocalImg(
        const vector<int> &spIdxs,
        const char *outName)
{
    Mat mask = getMaskByIdxs(spIdxs);

    Mat res;
    TheSrc.copyTo(res, mask);
    imwrite(outName, res);
}

/**
 * @brief SPIMAGE::getConfDiff
 *  from [start, size-1] choose a split dot,
 *  s.t.==> diff(colors[...]) maximum.
 *
 *  diff = between - diff([start, split]) + diff([split+1, end]).
 * @param colors
 * @param start
 * @param end
 * @param posRes
 * @return
 */
double SPIMAGE::getSplitIdxByColor(
        const vector<Point3d>& colors,
        const vector<double>& spPercent,
        int &posRes)
{
    double conf;

    bool flag = false;
    int start = 0, end = colors.size()-1;
    assert(end >= 1);
    for(int i = start; i < end; i++) {
        Point3d center1, center2;
        double onePeriod = diff(colors, spPercent, start, i, center1);
        double twoPeriod = diff(colors, spPercent, i+1, end, center2);
        double between = getEuclideanDist(center1, center2);
        double tmpdiff = between - onePeriod - twoPeriod;
        //-----------------------------------------------------------------
        if(system_debug) {
            cout << i << ":" << tmpdiff << "="
                 << between << "-" << onePeriod << "-"
                 << twoPeriod << endl;
        }
        //-----------------------------------------------------------------
        if(!flag) { conf = tmpdiff; posRes = i; flag = true; continue;}
        if(tmpdiff > conf) { conf = tmpdiff; posRes = i; }

        //        double tmpdiff = onePeriod + twoPeriod;
        //        if(!flag) {conf = tmpdiff; posRes = i; flag = true; }
        //        else if(tmpdiff < conf) {conf = tmpdiff; posRes = i;}
        //        //-----------------------------------------------------------------
        //        //        cout << i << ":" << tmpdiff << "="
        //        //             << onePeriod << "+" << twoPeriod << endl;
        //        //-----------------------------------------------------------------
    }
    //-----------------------------------------------------------------
    //    cout << endl;
    //-----------------------------------------------------------------

    return conf;
}

/**
 * @brief SPIMAGE::diff
 *  get center point ==> center;
 *      center = sum(color[start...end])/(end-start+1);
 *
 *  get period distance value ==> diffvalue;
 *      diffvalue = sum[i=start, end-1](sum[j=i+1, end](Dist(i, j)))/n;
 *      n = (end-start+1)*(end-start)/2;
 *      Dist(i, j) = EuclideanDist(color[i] - color[j]);
 *
 * @param colors
 * @param start
 * @param end
 * @param center
 * @return
 */
double SPIMAGE::diff(const vector<Point3d> &colors,
                     const vector<double>& spPercent,
                     const int start, const int end,
                     Point3d& center)
{
    double diffValue = 0;

    double totalPercent = 0;
    for(int i = start; i <= end; i++) {
        totalPercent += spPercent.at(i);
    }

    Point3d centerMid(0., 0., 0.);
    for(int i = start; i < end; i++) {
        Point3d color1 = colors.at(i);
        centerMid += color1 * (spPercent.at(i)/totalPercent);

        for(int j = i+1; j <= end; j++) {
            Point3d color2 = colors.at(j);
            diffValue += getEuclideanDist(color1, color2)
                    * spPercent.at(i) *spPercent.at(j) *2;
        }
    }
    Point3d tmp = colors.at(end);

    //    int n = (end-start+1)*(end-start)/2;
    centerMid += tmp * (spPercent.at(end)/totalPercent);
    center = centerMid;
    //    if(n != 0)
    //        diffValue /= n;
    return diffValue;
}

double SPIMAGE::getSleeveTercent(const Point &A, const Point &B, const Point &C, int& tercent, const char* outName)
{
    vector<int> spIdxs;
    vector<int> spPNum;
    vector<Point> spEdgePoint;

    spIdxs.clear();
    spPNum.clear();
    spEdgePoint.clear();
    int upNum = getSpIdxsOnLine(A, B, spIdxs, spPNum, spEdgePoint);
    getSpIdxsOnLine(B, C, spIdxs, spPNum, spEdgePoint);

    int totalCount = 0;
    int spCount = spIdxs.size();
    for(int i = 0; i < spCount; i++) { totalCount += spPNum.at(i); }

    vector<double> spPercent;
    vector<Point3d> colors;
    colors.clear();
    spPercent.clear();
    for(int i = 0; i < spCount; i++) {
        spPercent.push_back(spPNum.at(i)*1.0/totalCount);
        Point3d color = spCellGetColorByIdx(spIdxs.at(i));
        colors.push_back(color);
    }

    //-----------------------------------------------------------------
    if(system_debug) {
        Mat colorMap(10, 120, CV_8UC3, Scalar(0));
        char nametmp[100];
        int totalWidth = 0;
        //    cout << "draw:" << endl;
        for(int i = 0; i < spCount; i++) {
            Point3d color = colors.at(i);
            int width = int(spPercent.at(i)*100);
            rectangle(colorMap, Rect(totalWidth, 0, width, 10),
                      Scalar(color.z, color.y,color.x), -1);
            totalWidth += width;
            //show every sp:
            //        Mat mask(TheSrc.size(), CV_8U, Scalar(0));
            //        setMaskByIdx(mask, spIdxs.at(i));
            //        sprintf(nametmp, "%s%d.jpg", outName, i);
            //        Mat res;
            //        TheSrc.copyTo(res, mask);
            //        imwrite(nametmp, res);
        }
        sprintf(nametmp, "%s-colormap.jpg", outName);
        imwrite(nametmp, colorMap);
    }
    //        cout << outName << "-up:"  << endl;
    //        for (int i = 0; i < upNum; i++) {
    //            cout << "idx = " << spIdxs.at(i) << ";"
    //                 << "size = " << spPercent.at(i) << ";"
    //                 << "point = (" << spEdgePoint.at(i).x
    //                 << "," << spEdgePoint.at(i).y << ");" << endl;

    //            Mat mask(TheSrc.size(), CV_8U, Scalar(0));
    //            setMaskByIdx(mask, spIdxs.at(i));
    //            sprintf(nametmp, "up%s%d.jpg", outName, i);
    //            Mat res;
    //            TheSrc.copyTo(res, mask);
    //            imwrite(nametmp, res);
    //        }
    //        cout << endl;

    //        cout << outName << "-down:"  << endl;
    //        int tmpSize1 = spIdxs.size();
    //        for(int i = upNum; i < tmpSize1; i++) {
    //            cout << "idx = " << spIdxs.at(i) << ";"
    //                 << "size = " << spPercent.at(i) << ";"
    //                 << "point = (" << spEdgePoint.at(i).x
    //                 << "," << spEdgePoint.at(i).y << ");" << endl;

    //            Mat mask(TheSrc.size(), CV_8U, Scalar(0));
    //            setMaskByIdx(mask, spIdxs.at(i));
    //            sprintf(nametmp, "down%s%d.jpg", outName, i);
    //            Mat res;
    //            TheSrc.copyTo(res, mask);
    //            imwrite(nametmp, res);
    //        }
    //        sprintf(nametmp, "%s-arm.jpg", outName);
    //        getLocalImg(spIdxs, nametmp);
    //-----------------------------------------------------------------

    int Idx;
    double resconf = getSplitIdxByColor(colors, spPercent, Idx);
    //    cout << "confValue:" << resconf << endl;

    Point resPoint = spEdgePoint.at(Idx);
    if(Idx < upNum) {
        tercent = getDistTercent(resPoint, A, B);
    } else {
        tercent = getDistTercent(resPoint, B, C) + 500;
    }

    //-----------------------------------------------------------------
    if(system_debug) {
        Mat res;// = imread("spColor.jpg");
        char nametmp[100];
        TheSrc.copyTo(res);
        circle(res, resPoint, 2, Scalar(0,0,255), -1);
        //    //show all edge point;
        //    for (int i = 0; i < spCount; i++) {
        //        circle(res, spEdgePoint.at(i), 2, Scalar(0,0,255), -1);
        //    }
        sprintf(nametmp, "%s-res.jpg", outName);
        imwrite(nametmp, res);
    }
    //-----------------------------------------------------------------
    return resconf;
}

int SPIMAGE::getSpIdxsOnLine(const Point &start, const Point &end,
                             vector<int> &spIdxs,
                             vector<int> &spPNum,
                             vector<Point> &spEdgePoint)
{
    int originalSize = spIdxs.size();
    //--------------------------------------------------
    //    cout << "before:" << originalSize << endl;
    //--------------------------------------------------

    vector<Point> points;
    int size = getLinePoint(start, end, points);//total lenth;
    //--------------------------------------------------
    //    cout << "point on line:" << size << endl;
    //--------------------------------------------------

    vector<int> tmp;
    for(int i = 0; i < size; i++) {
        Point p = points.at(i);
        tmp.push_back(TheSeg.at<int>(p.y, p.x));
    }
    for(int i = 0; i < size;) {
        int idx = tmp.at(i);
        int pos;
        int count = vectorCountFindNxt(tmp, i, pos);//return the next element;
        //--------------------------------------------------
        //        cout << "get sp:" << idx << "-" << count << endl;
        //--------------------------------------------------
        if(!vectorHasExist(spIdxs, idx)) {
            spIdxs.push_back(idx);
            spPNum.push_back(count);
            spEdgePoint.push_back(points.at(pos));
        }
    }
    int newAddspNum = spPNum.size() - originalSize;
    //--------------------------------------------------
    //    cout << "result:" << endl;
    //    for(int i = originalSize; i < spIdxs.size();i++) {
    //        cout << "sp " << spIdxs.at(i) << ":"
    //                << percent.at(i) <<";"
    //                << spEdgePoint.at(i).x
    //                << "," << spEdgePoint.at(i).y<< endl;
    //    }
    //--------------------------------------------------
    return newAddspNum;
}

double SPIMAGE::getDistTercent(const Point &p, const Point &A, const Point &B)
{
    double res;
    res = getEuclideanDist(p, A)/getEuclideanDist(A, B)*500;
    return res;
}

Mat SPIMAGE::getRectColorImage(const Rect &rect)
{
    Mat src;
    TheSeg(rect).copyTo(src);

    vector<int> sps;
    for(int i = 0; i < rect.height; i++) {
        const int* ptr = src.ptr<int>(i);
        for(int j = 0; j < rect.width; j++) {
            int value = ptr[j];
            if(!vectorHasExist(sps, value))
                sps.push_back(value);
        }
    }
    //    Mat mask = getMaskByIdxs(sps);
    //    Mat resone;
    //    TheSrc.copyTo(resone, mask);
    //    imwrite("faceres.jpg", resone);
    int size = sps.size();
    Mat res(1, size, CV_8UC3);
    for(int i = 0; i < size; i++) {
        int spIdx = sps.at(i);
        Point3d color = spCellGetColorByIdx(spIdx);
        Vec3b colorValue(color.z, color.y, color.x);
        res.at<Vec3b>(0, i) = colorValue;
    }
    return res;
}
