#include "stdafx.h"
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

void MakeScale(float sx, float sy, float m[][3]);
void MakeRotate(float theta, float m[][3]);
void MakeTranslate(float tx, float ty, float m[][3]);
void MultiplyMatrix(float m[][3], float A[][3], float B[][3]);	// m = A B

int main()
{
	IplImage * src = NULL;
	
	while(!src)
	{
		char buf[255];
		printf("Input File Path: ");
		scanf("%s",buf);
		src= cvLoadImage(buf);

		if(src==NULL)
			printf("File not Found! \n");
	}

	IplImage *dst;
	dst = cvCreateImage(cvSize(800,800), 8, 3);

	int w_1 = src->width;
	int h_1 = src->height;
	int w_2 = dst->width;
	int h_2 = dst->height;

	float x1, y1, w1=1;
	float x2, y2, w2=1;

	float sx = 2.0f;				// scaling for x-axis
	float sy = 0.5f;				// scaling for y-axis
	float theta = 30.0f;			// rotation degree
	float tx = 100;					// translation for x-axis
	float ty = 200;					// translation for y-axis

	float m[3][3];
	float m1[3][3];
	float m2[3][3];
	float m3[3][3];
	float m4[3][3];

	MakeScale(sx, sy, m1);
	MakeRotate(theta, m2);
	MakeTranslate(tx,ty, m3);
								
	MultiplyMatrix(m4, m2, m1);	//m4 = m2 m1
	MultiplyMatrix(m,  m3, m4); //m  = m3 m4
								//m  = m3 m2 m1

	for(y1=0; y1<h_1; y1++)
		for(x1=0; x1<w_1; x1++)
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

			CvScalar f = cvGet2D(src, y1, x1);
			cvSet2D(dst, y2, x2, f);

		}

	cvNamedWindow("source");
	cvNamedWindow("result");

	cvShowImage("source", src);
	cvShowImage("result", dst);

	cvWaitKey();

	cvDestroyWindow("source");
	cvDestroyWindow("result");

	cvReleaseImage(&src);
	cvReleaseImage(&dst);

	return 0;
}


void MakeScale(float sx, float sy, float m[][3])
{
	m[0][0] = sx;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
	m[1][0] = 0.0f;	m[1][1] = sy;	m[1][2] = 0.0f;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
}

void MakeRotate(float theta, float m[][3])
{
	float rad = theta * 3.141592f / 180.0f;
	float c = cos(rad);
	float s = sin(rad);

	m[0][0] = c;	m[0][1] = -s;	m[0][2] = 0.0f;
	m[1][0] = s;	m[1][1] = c;	m[1][2] = 0.0f;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
}

void MakeTranslate(float tx, float ty, float m[][3])
{
	m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = tx;
	m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = ty;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
}

void MultiplyMatrix(float m[][3], float A[][3], float B[][3])	// m = A B
{
	int i,j,k;
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
		{
			m[i][j] = 0.0f;
			for(k=0; k<3; k++)
				m[i][j] += A[i][k]*B[k][j];
		}
}