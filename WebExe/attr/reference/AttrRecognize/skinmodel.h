#ifndef SKINMODEL_H
#define SKINMODEL_H

#define COLOR_BIT 256
#define LINE_MAX_LENTH 1024

class skinModel
{
private:
    long long skinCount;
    long long noSkinCount;
    double skinHist[COLOR_BIT][COLOR_BIT][COLOR_BIT];
    double noSkinHist[COLOR_BIT][COLOR_BIT][COLOR_BIT];

    double getSkinDensityByRGB(int r, int g, int b);
    double getNoSkinDensityByRGB(int r, int g, int b);
public:
    skinModel();
    void loadModelFromFile();
    bool judgePixelSkin(int r, int g, int b);    
    bool judgePixelSkin(int bgr[]);
};

#endif // SKINMODEL_H
