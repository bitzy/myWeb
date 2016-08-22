#ifndef _SVMPREDICT
#define _SVMPREDICT
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include "svm.h"
using namespace std;

typedef struct inputPredict
{
    vector<float> data; //hog feature
    int n;              //dimension
}inputPredict;

typedef struct outputPredict
{
    double conf1;   //confidence 1
    double conf2;   //confidence 2
    int n;          //dimension
}outputPredict;


int svm_predict2b(struct inputPredict &input,
                  struct outputPredict &output,
                  struct svm_model **model,
                  int mark);

#endif
