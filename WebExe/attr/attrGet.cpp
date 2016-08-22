/*************************************************************************
	> File Name: attrGet.cpp
	> Author: zhangyan
	> Mail: 1534589721@qq.com 
	> Created Time: Tue 09 Aug 2016 09:55:19 AM CST
 ************************************************************************/

#include<iostream>
#include<stdlib.h> 
#include "reference.h"
using namespace std;

int main(int argc, char** argv) 
{
	if(argc != 8) {
		printf("Usage: ./exe imgPath posPath fcollar fsleeve fuplenth fcolor ftexture\n");
		exit(1);
	}	

	//get pos data to MyLocation;
	MyLocation* loc = new MyLocation;
	if(loc->loadPosition(argv[2]) != 1) {
		printf("load pos data error!\n");
		exit(1);
	}

	AttrRelation * subtool = new AttrRelation;

	//collar:
	int x1 = testWayCollar(argv[1], loc);
	//cout << x1 << endl;
	FILE* fp = fopen(argv[3], "w");
	fprintf(fp, "%s\n", subtool->getRelateValue("collar", x1).c_str());
	fclose(fp);

	//sleeve:
	//int x2 = baseWaySleeve(argv[1], loc);
	int x2 = addSkinSleeve(argv[1], loc);
	fp = fopen(argv[4], "w");
	fprintf(fp, "%s\n", subtool->getRelateValue("sleeve", x2).c_str());
	fclose(fp);

	//uplenth:
	int x3 = upLengAnalyze(argv[1], loc);	
	fp = fopen(argv[5], "w");
	fprintf(fp, "%s\n", subtool->getRelateValue("up_length", x3).c_str());
	fclose(fp);

	//color:
	getColor(argv[1], loc, "tmp/color");
	//int clothesMainColorMean[3];
	//getMeanColor(clothMainColor, 5, clothesMainColorMean);
	
	//texture:
	
	return 0;
}
