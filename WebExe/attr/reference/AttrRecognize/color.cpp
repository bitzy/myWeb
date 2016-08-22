#include "attrRecognize.h"
#include "sleeveHeader.h"


void getColor(const char* fpath, MyLocation* loc, char* output) {
    Mat src = preProcessing(fpath, loc);

    ColorFeat2 colorFeatureRes[5];
    getClothesMainColor(src, loc, colorFeatureRes);

    FILE* fp = fopen(output, "w");
    if(fp != NULL) {
        for (int i = 0; i < 5; i++) {
            fprintf(fp, "%ld %d %d %d\n", colorFeatureRes[i].per,
                    colorFeatureRes[i].rgb[0],
                    colorFeatureRes[i].rgb[1],
                    colorFeatureRes[i].rgb[2]);
        }
        fclose(fp);
    }
}
