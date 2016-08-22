#include "skinmodel.h"
#include <QFile>
#include <QString>
#include <QStringList>
#include <iostream>
using namespace std;

skinModel::skinModel()
{
    loadModelFromFile();
}

double skinModel::getSkinDensityByRGB(int r, int g, int b)
{    
    return skinHist[r][g][b];
}

double skinModel::getNoSkinDensityByRGB(int r, int g, int b)
{
    return noSkinHist[r][g][b];
}

void skinModel::loadModelFromFile()
{
    QFile file(":/skin/Model/BayesSkinModel/skin.model");
    if(file.open(QIODevice::ReadOnly)) {
        QString line = file.readLine(LINE_MAX_LENTH);
        long long skinCount = line.simplified().toLongLong();
        for(int i = 0; i < COLOR_BIT; i++) {
            for(int j = 0; j < COLOR_BIT; j++) {
                line = file.readLine(LINE_MAX_LENTH);
                QStringList numStr = line.simplified().split(" ");
                if(numStr.size() != COLOR_BIT) exit(0);
                for(int k = 0; k < COLOR_BIT; k++) {
                    long long value = numStr.at(k).toLongLong();
                    skinHist[i][j][k] = value*1.0/skinCount;
                }
            }
        }
    }
    file.close();
    QFile noSkinFile(":/skin/Model/BayesSkinModel/noskin.model");
    if(noSkinFile.open(QIODevice::ReadOnly)) {
        QString line = noSkinFile.readLine(LINE_MAX_LENTH);
        long long noskinCount = line.simplified().toLongLong();
        for (int i = 0; i < COLOR_BIT; i++) {
            for(int j = 0; j < COLOR_BIT; j++) {
                line = noSkinFile.readLine(LINE_MAX_LENTH);
                QStringList numStr = line.simplified().split(" ");
                for(int k = 0; k < COLOR_BIT; k++) {
                    long long value = numStr.at(k).toLongLong();
                    noSkinHist[i][j][k] = value*1.0/noskinCount;
                }
            }
        }
    }
    noSkinFile.close();
}

bool skinModel::judgePixelSkin(int r, int g, int b)
{
    double skinProbability = getSkinDensityByRGB(r, g, b);
    double noSkinProbability = getNoSkinDensityByRGB(r, g, b);
    if(skinProbability > noSkinProbability)
        return true;
    else return false;
}

bool skinModel::judgePixelSkin(int bgr[])
{
    double skinProbability = getSkinDensityByRGB(bgr[2], bgr[1], bgr[0]);
    double noSkinProbability = getNoSkinDensityByRGB(bgr[2], bgr[1], bgr[0]);
    if(skinProbability > noSkinProbability)
        return true;
    else return false;
}
