#include "svmpredict.h"

int print_null(const char */*s*/,...) {
    return 0;
}

struct svm_node *x;
int max_nr_attr = 64;

int predict_probability=1;

void predictOne(struct inputPredict &input,
                struct outputPredict &output,struct svm_model* modelUsed) {
    int svm_type=svm_get_svm_type(modelUsed);
    int nr_class=svm_get_nr_class(modelUsed);
    double *prob_estimates=NULL;
    prob_estimates = (double *) malloc(nr_class*sizeof(double));

    int i = 0;
    double predict_label;

    int tmpsize = input.data.size();
    for(i = 0; i < tmpsize; i++) {
        if(i>=max_nr_attr-1)	// need one more for index = -1
        {
            max_nr_attr *= 2;
            x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
        }
        x[i].index = i+1;
        x[i].value = input.data[i];
    }
    x[i].index = -1;

    if (predict_probability && (svm_type==C_SVC || svm_type==NU_SVC))
    {
        predict_label = svm_predict_probability(modelUsed,x,prob_estimates);
        int tmp1 = (int)(predict_label+0.5);
        output.n = tmp1;
        output.conf1 = prob_estimates[0];
        output.conf2 = prob_estimates[1];
    }
    if(predict_probability)
        free(prob_estimates);
}

int svm_predict2b(struct inputPredict &input,
                  struct outputPredict &output,
                  struct svm_model **model, int mark)
{
    // parse options
    predict_probability = 1;
    x = (struct svm_node *)malloc(max_nr_attr*sizeof(struct svm_node));

    struct svm_model* modelUsed;
    modelUsed = model[mark];
    predictOne(input, output, modelUsed);
    free(x);
    return 0;
}
