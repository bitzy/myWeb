#include "attrRecognize.h"
using namespace cv;

#define POSE_NECK "neck"
#define POSE_LSHOULDER "left_shoulder"
#define POSE_RSHOULDER "right_shoulder"
#define POSE_LHIP "left_hip"
#define POSE_RHIP "right_hip"

#define COLORFEATSIZE 5
#define C_FEAT_MAX 262144

ColorFeat rigionColorFeatAll[C_FEAT_MAX];

int compForColor(const void *a,const void *b) {
    return (((ColorFeat *)b)->per) - (((ColorFeat *)a) ->per);
}

int compForColor2(const void *a,const void *b) {
    return (((ColorFeat2 *)b)->per) - (((ColorFeat2 *)a) ->per);
}

void KmeansCluster(ColorFeat kmeansRes[15],ColorFeat rigionColorFeatAll[],const int featAllsize)
{
    int i;
    for(i=0;i<30;i++)
    {
        kmeansRes[i]=rigionColorFeatAll[i];
    }

    for(;i<featAllsize&&rigionColorFeatAll[i].per>0;i++)
    {
        int mindist=1000000;
        int mark = -1;
        for(int j=0;j<30;j++)
        {
            int colDist = colorDist(kmeansRes[j].rgb,rigionColorFeatAll[i].rgb);
            if(colDist<mindist)
            {
                mark = j;
                mindist=colDist;
            }
        }
        kmeansRes[mark].per = kmeansRes[mark].per+rigionColorFeatAll[i].per;
    }
    qsort(kmeansRes,30,sizeof(kmeansRes[0]),compForColor);
}


void KmeansCluster2(cv::Mat src2[],int clustercount,char saveFilePathWithoutExtention[],ColorFeat2 colorfeatureRes[])
{
    char savefilename[300];
    Mat afterCompresColorRigion[2];
    for(int i=0;i<2;i++)
    {
        int zhaibian = src2[i].cols>src2[i].rows?src2[i].rows:src2[i].cols;
        double scale = 30.0/zhaibian;
        resize(src2[i],afterCompresColorRigion[i],Size(),scale,scale);
    }


    int datanum = afterCompresColorRigion[0].cols*afterCompresColorRigion[0].rows + afterCompresColorRigion[1].cols*afterCompresColorRigion[1].rows;


    Mat dataForCluster = Mat(datanum,1,CV_32FC3);
    Mat src;
    int datanumiter = 0;
    RNG rng(12345); //随机数产生器
    for(int i=0;i<2;i++)
    {
        afterCompresColorRigion[i].copyTo(src);

        sprintf(savefilename,"%s_afterCompress_%d.jpg",saveFilePathWithoutExtention,i);
        imwrite(savefilename,src);
        for(int j=0;j<src.rows;j++)
        {
            uchar *ptr = src.ptr<uchar>(j);
            for(int k=0;k<src.cols;k++)
            {
                float *ptr2 = dataForCluster.ptr<float>(datanumiter);
                ptr2[0] = (float)ptr[3*k+2];//r
                ptr2[1] = (float)ptr[3*k+1];//g
                ptr2[2] = (float)ptr[3*k];//b
                datanumiter++;
            }
        }
    }
    Mat labels,centers(clustercount,1,dataForCluster.type());
    randShuffle(dataForCluster, 1, &rng);
    kmeans(dataForCluster,clustercount+2,labels,TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
           3, KMEANS_PP_CENTERS, centers);

    ColorFeat2 *colorfeat2 = (ColorFeat2 *)malloc((clustercount+2)*sizeof(ColorFeat2));
    memset(colorfeat2,0,(clustercount+2)*sizeof(ColorFeat2));

    Mat color[20];


    for(int i=0;i<clustercount+2;i++)
    {
        float *ptr = centers.ptr<float>(i);
        for(int j=0;j<3;j++)
        {
            colorfeat2[i].rgb[j] = ptr[j];
        }
    }

    for(int i=0;i<labels.rows;i++)
    {
        int clusterIndex = labels.at<int>(i);
        colorfeat2[clusterIndex].per++;
    }
    qsort(colorfeat2,clustercount+2,sizeof(colorfeat2[0]),compForColor2);
    int sum = 0;
    for(int i = 0;i<clustercount;i++)
    {
        sum+=colorfeat2[i].per;
    }
    for(int i = 0;i<clustercount;i++)
    {
        colorfeat2[i].per = colorfeat2[i].per*100/sum;
    }
    for(int i=0;i<clustercount;i++)
    {

        color[i] = Mat(Size(500,500),CV_8UC3,Scalar(colorfeat2[i].rgb[2],colorfeat2[i].rgb[1],colorfeat2[i].rgb[0]));
        //sprintf(savefilename,"%s_color%d_%d%d%lld.jpg",saveFilePathWithoutExtention,i,colorfeat2[i].per);
        //imwrite(savefilename,color[i]);
        colorfeatureRes[i] = colorfeat2[i];
    }
}

void getRGB(int a,int rgb[])
{
    rgb[0] = (a & 0x003f) << 2;
    rgb[1] = ((a >> 6) & 0x003f) << 2;
    rgb[2] = ((a >> 12) & 0x003f) << 2;
}


void getMultiRigionColorFeature(const Mat src,struct Location */*loc*/,char saveFilePathWithoutExtention[],
                                myRectangle colorFeatureRigion,ColorFeat multiRigionColorFeature[])
{
    Mat colorRigion = src(Range(colorFeatureRigion.start.y, colorFeatureRigion.end.y),
                          Range(colorFeatureRigion.start.x, colorFeatureRigion.end.x));
    //KmeansCluster2(colorRigion,10);
    for(int j=0;j<C_FEAT_MAX;j++)
    {
        rigionColorFeatAll[j].rgb = j;
        rigionColorFeatAll[j].per = 0;
    }

    for(int j = 0; j < colorRigion.rows; j++)
    {
        uchar* p = colorRigion.ptr<uchar>(j);

        for(int k = 0; k < colorRigion.cols * 3; k += 3)
        {
            /*用iter的低24位保存颜色信息，低24位依次为 XXXXXX    XXXXXX    XXXXXX  ,左侧为高位，右侧为低位， 从左到右依次为bgr*/

            unsigned int iter = ((p[k] & 0x00FC) << 10)//b
                    | ((p[k + 1] & 0x00FC) << 4)//g
                    | ((p[k + 2] & 0x00FC) >> 2);//r


            rigionColorFeatAll[iter].per ++;
        }
    }

    //将rigionColorFeatAll中的per非零元素移动到前，per为0元素移动到后
    int low = 0, high = C_FEAT_MAX - 1;

    while(low < high)
    {
        //寻找左边per等于0的元素位置
        while(rigionColorFeatAll[low].per!=0 && low < high)
            low ++;
        //寻找右边per不等于0的元素位置
        while((0==rigionColorFeatAll[high].per) && low < high)
            high --;

        rigionColorFeatAll[low] =rigionColorFeatAll[high];
        rigionColorFeatAll[high].per = 0;
        low ++;
        high--;
    }
    int N = low+1;
    qsort(rigionColorFeatAll,N,sizeof(rigionColorFeatAll[0]),compForColor);
    /*
    ColorFeat kmeansRes[30];
    KmeansCluster(kmeansRes,rigionColorFeatAll,N);
    for(int j=0;j<30;j++)
    {
    multiRigionColorFeature[j] = kmeansRes[j];
    }
    */
    //int rgbTmp[3];
    for(int i = 0; i < COLORFEATSIZE; i++)
    {
        //getRGB(rigionColorFeatAll[i].rgb,rgbTmp);
        if(rigionColorFeatAll[i].per == 0)
            continue;

        for(int j = i + 1; j < COLORFEATSIZE ; j++)
        {
            if(rigionColorFeatAll[j].per == 0)
                continue;

            double t1 = colorDist(rigionColorFeatAll[i].rgb, rigionColorFeatAll[j].rgb);

            if(t1 < 3000)
            {
                rigionColorFeatAll[i].per += rigionColorFeatAll[j].per;
                rigionColorFeatAll[j].per = 0;
            }
        }
    }
    qsort(rigionColorFeatAll,N,sizeof(rigionColorFeatAll[0]),compForColor);
    for(int j=0;j<30;j++)
    {
        multiRigionColorFeature[j] = rigionColorFeatAll[j];
    }
    char savefilename[300];
    sprintf(savefilename,"%s_rigion.jpg",saveFilePathWithoutExtention);
    imwrite(savefilename,colorRigion);
    //}
}


//zy: color distance calculation:
inline int distForColorByCount(int i, int j)
{
    int r1, r2, g1, g2, b1, b2;
    r1 = ((i & 0x0003f000) >> 10);
    g1 = ((i & 0x00000fc0) >> 4);
    b1 = ((i & 0x0000003f) << 2);
    r2 = ((j & 0x0003f000) >> 10);
    g2 = ((j & 0x00000fc0) >> 4);
    b2 = ((j & 0x0000003f) << 2);
    int rmean = ((r1 + r2) >> 1);

    int r,g,b;
    r = r1 - r2;
    g = g1 - g2;
    b = b1 - b2;
    return (((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8);
}

int compForColorByCount(const void *a , const void *b) {
    return (((ColorFeat *)b)->per) - (((ColorFeat *)a) ->per);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief getClothColorFeatureByCount: get color feature(context)
/// \param colorRigion: image's areas for color feature extract
/// \param colorRigionNum: area's count
/// \param colorFeatureRes: output for color feature
/// \param threshold: 3000
/// \param saveFilePathWithoutExtention: outputname
///
void getClothColorFeatureByCount(
        Mat colorRigion[],
        const int colorRigionNum,
        ColorFeat colorFeatureRes[],
        double threshold)
{
    //get color feat:
    ColorFeat cFeat[C_FEAT_MAX];
    for(int i = 0; i < C_FEAT_MAX; i++)	{//C_FEAT_MAX = 262144
        cFeat[i].rgb = i;
        cFeat[i].per = 0;
    }
    for (int m = 0;m < colorRigionNum;m++) {
        // every area:

        int tmpRow = colorRigion[m].rows;
        for(int i = 0; i < tmpRow; i++) {
            uchar* p = colorRigion[m].ptr<uchar>(i);

            int tmpCol = colorRigion[m].cols * 3;
            for(int j = 0; j < tmpCol; j += 3) {
                //compress the pixel's rgb & count the frenquency
                unsigned int r = p[j+2]; //8bit ==> 32 bit
                unsigned int g = p[j+1];
                unsigned int b = p[j];
                unsigned int iter = ((r & 0x000000FC) << 10)
                        | ((g & 0x000000FC) << 4) | ((b & 0x000000FC) >> 2);
                cFeat[iter].per++;
            }
        }//area deal with;
    }

    //let the 0 elment to the right position: begin with low+1;
    int low = 0, high = C_FEAT_MAX - 1;
    while(low < high) {
        while(cFeat[low].per && low < high) low++;
        while((!cFeat[high].per) && low < high) high--;

        if(low < high) {
            cFeat[low] = cFeat[high];
            cFeat[high].per = 0;
            low++;
            high--;
        }
    }

    //sort(exclude the 0 elements) descent:
    int N = low + 1;
    qsort(cFeat, N, sizeof(cFeat[0]), compForColorByCount);

    //join the close color cell:
    int len = (N >= COLORFEATSIZE ? N - COLORFEATSIZE : 0);
    for(int i = 0; i < len; i++) {
        if (cFeat[i].per == 0) continue;

        for(int j = i + 1; j < len ; j++) {
            if (cFeat[j].per == 0) continue;

            double tmpDist = distForColorByCount(cFeat[i].rgb, cFeat[j].rgb);
            if(tmpDist < threshold) {//threshold = 3000
                cFeat[i].per += cFeat[j].per;
                cFeat[j].per = 0;
            }
        }
    }

    //sort again:
    qsort(cFeat, N, sizeof(ColorFeat), compForColorByCount);

    //deal top COLORFEATSIZE:
    double sum = 0;
    for(int i = 0; i < COLORFEATSIZE; i++) {
        sum += cFeat[i].per;
    }
    for(int i = 0 ; i < COLORFEATSIZE; i++) {
        cFeat[i].per = cFeat[i].per * 100 / sum;
    }

    memcpy(colorFeatureRes, cFeat, sizeof(ColorFeat) * COLORFEATSIZE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief getClothesMainColor: get color feature(Interface)
/// \param src: srcImg file
/// \param loc: location
/// \param saveFilePathWithoutExtention: output image's name
/// \param colorFeatureRes: gets feature
///
void getClothesMainColor(cv::Mat src, MyLocation */*loc*/, ColorFeat2 colorFeatureRes[])
{    
    Mat src2;
    src.copyTo(src2);

    //draw picture of feature extract area:
    myRectangle colorFeatureRigion[2];
    colorFeatureRigion[0].start.x = lshoulder.x + (neck.x - lshoulder.x)/5;
    colorFeatureRigion[0].start.y = lshoulder.y + (lhip.y - lshoulder.y)/5;
    colorFeatureRigion[0].end.x = neck.x - (neck.x - lshoulder.x)/5;
    colorFeatureRigion[0].end.y = lhip.y - (lhip.y - lshoulder.y)/5;

    colorFeatureRigion[1].start.x = neck.x + (rshoulder.x - neck.x)/5;
    colorFeatureRigion[1].start.y = neck.y + (rhip.y - rshoulder.y)/5;
    colorFeatureRigion[1].end.x = rshoulder.x - (rshoulder.x - neck.x)/5;
    colorFeatureRigion[1].end.y = rhip.y - (rhip.y - rshoulder.y)/5;

    char savefilename[300];
    //==================================================
    rectangle(src2, colorFeatureRigion[0].start, colorFeatureRigion[0].end,
            Scalar(rand()%256,rand()%256,rand()%256), 2);
    rectangle(src2, colorFeatureRigion[1].start, colorFeatureRigion[1].end,
            Scalar(rand()%256,rand()%256,rand()%256), 2);
    sprintf(savefilename,"timg_colorFeature_flagArea.jpg");
    imwrite(savefilename,src2);
    //==================================================

    //get mat of feature extract area:
    Mat colorRigion[2];
    //reverse point-byzy:
    int startx = colorFeatureRigion[0].start.x;
    int starty = colorFeatureRigion[0].start.y;
    int endx = colorFeatureRigion[0].end.x;
    int endy = colorFeatureRigion[0].end.y;
    if(starty > endy) {
        endy = colorFeatureRigion[0].start.y;
        starty = colorFeatureRigion[0].end.y;
    }
    if(startx > endx) {
        startx = colorFeatureRigion[0].end.x;
        endx = colorFeatureRigion[0].start.x;
    }
    colorRigion[0]= src(Range(starty, endy), Range(startx, endx));

    //reverse point-byzy:
    startx = colorFeatureRigion[1].start.x;
    starty = colorFeatureRigion[1].start.y;
    endx = colorFeatureRigion[1].end.x;
    endy = colorFeatureRigion[1].end.y;
    if(starty > endy) {
        endy = colorFeatureRigion[1].start.y;
        starty = colorFeatureRigion[1].end.y;
    }
    if(startx > endx) {
        startx = colorFeatureRigion[1].end.x;
        endx = colorFeatureRigion[1].start.x;
    }
    colorRigion[1]= src(Range(starty, endy),Range(startx, endx));

    //get feature:
    ColorFeat colorFeatureRes2[5];
    getClothColorFeatureByCount(colorRigion, 2, colorFeatureRes2, 3000);
    int i = 0;
    while(i < COLORFEATSIZE) {
        colorFeatureRes[i].rgb[0] = ((colorFeatureRes2[i].rgb & 0x3F000) >> 10);//r
        colorFeatureRes[i].rgb[1] = ((colorFeatureRes2[i].rgb & 0x0FC0) >> 4);//g
        colorFeatureRes[i].rgb[2] = ((colorFeatureRes2[i].rgb & 0x003F) << 2);//b
        colorFeatureRes[i].per = colorFeatureRes2[i].per;
        i++;
    }
}
