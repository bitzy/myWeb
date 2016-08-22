#ifndef SKINCOLORANALYZE_H
#define SKINCOLORANALYZE_H

#include "../AttrBase/cvTool.h"

class SkinColorAnalyze
{    
private:
    Vec3b color;

public:
    //get skin color by rect pixel:
    SkinColorAnalyze(const Mat& img, const Rect& area);
    //get skin color by mat sp color aggregation:
    SkinColorAnalyze(const Mat& img);
    Vec3b getSkinColor();
    void getSkinColorImg(const char* outName);
    bool judgeSkinByPixel(const Vec3b& color);
};

#endif // SKINCOLORANALYZE_H
