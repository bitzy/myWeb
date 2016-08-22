#ifndef SLEEVEHEADER_H
#define SLEEVEHEADER_H

#include "../AttrBase/cvTool.h"
#include "attrRecognize.h"
#include "../AttrBase/myfunc_base.h"


typedef struct nodeForClothlen
{
    int num;
    double dist;
    nodeForClothlen() {
        num=0;
        dist = 0;
    }
} nodeForClothlen;

/**
 * @brief sleeveProcessing
 *      - locad pose data;
 *      - get bounding box;
 *      - scale the image & data to new size;
 * @param fpath
 * @param loc
 * @param HSTYLE
 * @return
 *      - the new scaled Image.
 */
Mat sleeveProcessing(const char * fpath, MyLocation* loc,
                     int& HSTYLE, bool& leftFlag);
Mat preProcessing(const char* fpath, MyLocation* loc);


//tool:
double cov(const int hist1[], const int hist2[], int dim);

#endif // SLEEVEHEADER_H
