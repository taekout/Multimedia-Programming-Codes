// Bracost.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

//#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>

void BrightContrast(IplImage * src, IplImage * dst, float a, float b);

int main(int argc, CHAR* argv[])
{
	IplImage * src = cvLoadImage("cross1.jpg");

	if(src == NULL){	
		printf("Fail in Loading!\n");
		return 0;
	}
	
	IplImage * dst = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U,3);
	cvCopy(src, dst);

	cvNamedWindow("Window");
	cvShowImage("Window", dst);
	
	int c;
	float a = 1.0f;
	float b = 0.0f;

	do{
		c = cvWaitKey();
		printf("%d was pressed! ", c);
 
		switch(c)
		{
		case 49:					// '1' pressed
			b += 10.0f;
			break;
		case 50:					// '2' pressed
			b -= 10.0f;
			break;
		case 51:					// '3' pressed
			a += 0.2f;
			break;
		case 52:					// '4' pressed
			a -= 0.2f;
			break;
		default:
			break;				
		}
		printf("Brightness: %5.3f Contrast: %5.3f\n", b, a);
		BrightContrast(src, dst, a, b);
		cvShowImage("Window", dst);

	}while(c != 32);				// space was pressed

	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvDestroyWindow("Window");

	return 0;
}


void BrightContrast(IplImage * src, IplImage * dst, float a, float b)
{
	cvCopy(src, dst);
	int x, y;
	int t;

	for(x=src->width/2; x<src->width ; x++)
	{
		for(y=0; y<src->height ; y++)
		{
			CvScalar s;
			
			float value;
			s = cvGet2D(src, y, x);
			for(t=0; t<3; t++)
			{
				value = s.val[t]*a+b;
				if(value>255) value = 255;
				if(value<0) value = 0;
				s.val[t] = (int) value;
			}
			cvSet2D(dst,y,x,s);
		}
	}
	return;
}

