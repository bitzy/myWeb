#ifndef IMAGESP_H
#define IMAGESP_H

#include "../AttrBase/cvTool.h"
#include <vector>

class SPCELL
{
private:
    int TheSpIdx;       //the seg notation;
    vector<Point> ThePixels;

    Point TheCenter;
    Point3d TheSpColor;     //the sp's center color;

public:
    SPCELL();
    void setCellBase(int spIdx, const vector<Point>& pos);
    void setCenter(const Point& p);
    void setColor(const Point3d& value);
    void setColor(const Vec3b& value);

    int getIdx();
    vector<Point>& getPixels();
    int getPixelsSize();
    Point &getPixel(unsigned int j);
    Point &getCenter();
    Point3d &getColor();
};

/**
 * @brief The SPIMAGE class
 * usage:
 *      SPIMAGE(img.cols, img.rows);
 *      get segment information to spInfo:
 *          getImageSpInfo(seg, SPIMAGE);
 *      set center color value:
 *          SPIMAGE.setCenterColorByHist(img);
 *      or:
 *          SPIMAGE.setCenterColorToSP(img);
 */
class SPIMAGE
{
private:
    Mat TheSrc;
    Mat TheSeg;//CV_32SC1
    vector<SPCELL> spInfo;

public:   
    //    SPIMAGE(const Mat& src, const Mat& labels, const int num);
    SPIMAGE(const Mat& src, const Mat& seg2D);
    void addSpCell(int spIdx, const vector<Point>& pos);
    void setColorByCenterForEachSp();
    void setColorByHistomForEachSp();


    SPCELL& getSpCell(unsigned int i);
    SPCELL& getSpCellByIdx(int spIdx);
    int getSpNum();

    int spCellGetIdx(unsigned int i);
    Point &spCellGetCenter(unsigned int i);
    Point3d &spCellGetColor(unsigned int i);
    vector<Point>& spCellGetPixelsByIdx(int spIdx);
    Point &getSpCellIdxPixelAt(unsigned int i, int j);
    Point &spCellGetCenterByIdx(int spIdx);
    Point3d &spCellGetColorByIdx(int spIdx);

    //=========================================================================
    void getPosFromSegByIdx(int spIdx,
                            vector<Point>& pos);
    void setMaskByIdx(Mat& mask, int spIdx);
    void setMaskByPos(Mat& mask, vector<Point>& pos);
    Mat getMaskByIdxs(const vector<int>& spIdxs);
    void getSpColorImage(const char* outName);
    void getLocalImg(const vector<int>& spIdxs,
                     const char* outName);
    //    double getSplitPoint(const vector<int> &spIdxs,
    //                         const vector<double> &spPercent,
    //                         int &spIdxRes,
    //                         const char* outName);
    double getSplitIdxByColor(const vector<Point3d> &colors,
                              const vector<double> &spPercent,
                              int &posRes);
    double diff(const vector<Point3d> &colors,
                const vector<double> &spPercent,
                const int start, const int end,
                Point3d &center);

    double getSleeveTercent(const Point& A,
                            const Point& B,
                            const Point& C,
                            int &tercent,
                            const char* outName);
    int getSpIdxsOnLine(const Point& start,
                        const Point& end,
                        vector<int> &spIdxs,
                        vector<int> &spPNum,
                        vector<Point> &spEdgePoint);
    double getDistTercent(const Point& p, const Point& A, const Point& B);
    //=========================================================================
    Mat getRectColorImage(const Rect& rect);
};

#endif // IMAGESP_H
