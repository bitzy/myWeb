#include <stdio.h>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <highgui.h>
using namespace cv;

int preIdx[14] = {-1, 0, 1, 2, 3, 1, 5, 6, 2, 5, 8, 9, 10, 11};
Point dots[14];
void drawPoint(Mat&, Point&, int);

int main(int argc, char** argv)
{	
	if(argc != 4) {
		printf("Usage:./exe inputImg outputPath dataPath\n");
		exit(1);
	}

	Mat image = imread(argv[1]);
	FILE* fp = fopen(argv[3], "r");
	char str[30];
	char num1[10], num2[10];
	int number1, number2;
	int count = 0;
	while((fscanf(fp, "%[^\n]%*c", str)) != EOF) {
		if(strcmp(str, "null") == 0 || strcmp(str, "-1,-1") == 0) {
			count++;
			continue;
		}
		sscanf(str, "%[^,]%*c%s", num1, num2);

		number1 = atoi(num1);
		number2 = atoi(num2);
		//printf("%d:%d,%d\n", count+1, number1, number2);

		dots[count].x = number1;
		dots[count].y = number2;
		drawPoint(image, dots[count], count);
		count++;
	}
	imwrite(argv[2], image);
	fclose(fp);

	return 0;
}

void drawPoint(Mat& img, Point& p, int count) {
	if(preIdx[count] == -1) return;

	Point p1 = dots[preIdx[count]];
	line(img, p1, p, Scalar(0,0,255), 2);
}
