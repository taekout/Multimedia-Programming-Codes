#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


#include "MatrixInverse.h"



int _tmain(int argc, char* argv[])
{

	IplImage * img = cvLoadImage("d:\\temp\\lena_triangle.png");
	IplImage *dst;
	dst = cvCreateImage(cvSize(800,800), 8, 3);

	int ix1[3]={250,100,400};
	int iy1[3]={100,350,350};
	int ix2[3]={250,50,450};
	int iy2[3]={100,350,100};

	int i;
	for(i=0; i<3; i++)
	{
		cvCircle(img, cvPoint(ix1[i], iy1[i]), 4, CV_RGB(255,0,0), -1);
		cvCircle(img, cvPoint(ix2[i], iy2[i]), 4, CV_RGB(0,0,255), -1);
	}
		
	float a[3][3], ia[3][3];

	for(i=0; i<3; i++)
	{
		a[i][0] = ix1[i];
		a[i][1] = iy1[i];
		a[i][2] = 1.0f;
	}
	
	InverseMatrixGJ(a, ia, 3);
	
	float m[3][3] = {{0,0,0},{0,0,0},{0,0,1}};

	for(i=0; i<3; i++)
	{
		m[0][0] += ia[0][i]*ix2[i];
		m[0][1] += ia[1][i]*ix2[i];
		m[0][2] += ia[2][i]*ix2[i];
		m[1][0] += ia[0][i]*iy2[i];
		m[1][1] += ia[1][i]*iy2[i];
		m[1][2] += ia[2][i]*iy2[i];
	}
		
	int w_1 = img->width;
	int h_1 = img->height;
	int w_2 = dst->width;
	int h_2 = dst->height;
	
	float x1,y1, x2,y2 , w1=1, w2 =1;
	for(y1=0; y1<h_1; y1 += 0.5)
		for(x1=0; x1<w_1; x1 += 0.5)
		{
			x2 = m[0][0]*x1 + m[0][1]*y1 + m[0][2]*w1;
			y2 = m[1][0]*x1 + m[1][1]*y1 + m[1][2]*w1;
			w2 = m[2][0]*x1 + m[2][1]*y1 + m[2][2]*w1;

			x2 = x2/w2;
			y2 = y2/w2;

			if(x1<0 || x1>=w_1) continue;
			if(y1<0 || y1>=h_1) continue;
			if(x2<0 || x2>=w_2) continue;
			if(y2<0 || y2>=h_2) continue;

			CvScalar f = cvGet2D(img, y1, x1);
			cvSet2D(dst, y2, x2, f);

		}


	cvNamedWindow("test");
	cvShowImage("test", img);
	cvNamedWindow("test2");
	cvShowImage("test2", dst);

	cvWaitKey();

	cvDestroyWindow("test");
	cvDestroyWindow("test2");
	cvReleaseImage(&img);
	cvReleaseImage(&dst);
	
	
	return 0;
}


