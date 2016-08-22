#ifndef SCALETOOL_H
#define SCALETOOL_H
#include "../AttrBase/cvTool.h"

class SCALETOOL {
private:
    int xoffset;
    int yoffset;
    double scale;
public:
    SCALETOOL() {
        xoffset = yoffset = scale = -1;
    }

    void scaleToFixHeight(Mat& src, const Rect& rect, int height = 800) {
        xoffset = rect.x;
        yoffset = rect.y;

        Mat res;
        Mat focusImgROI = src(rect);
        focusImgROI.copyTo(res);
        scale = 1.0*height/focusImgROI.rows;
        resize(res, src, Size(), scale, scale);
    }
    void changePoint(Point& oldPoint) {
        oldPoint.x -= xoffset;
        oldPoint.y -= yoffset;
        oldPoint.x = int(oldPoint.x*scale);
        oldPoint.y = int(oldPoint.y*scale);
    }
    Point getCorrespondPoint(const Point& oldPoint) {
        Point res;
        res.x = oldPoint.x - xoffset;
        res.y = oldPoint.y - yoffset;
        res.x = int(res.x*scale);
        res.y = int(res.y*scale);
        return res;
    }
    void scalePoint(Point& oldPoint) {
        oldPoint.x = (oldPoint.x - xoffset)*scale;
        oldPoint.y = (oldPoint.y - yoffset)*scale;
    }
};

#endif // SCALETOOL_H
