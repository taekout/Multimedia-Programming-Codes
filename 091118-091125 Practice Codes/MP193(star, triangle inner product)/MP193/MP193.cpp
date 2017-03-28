// MP193.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "MyTriangle.h"
#include "MyStar.h"

int main(int argc, _TCHAR* argv[])
{
	CMyTriangle tri;
	tri.m_pt[0] = cvPoint(100,100);
	tri.m_pt[1] = cvPoint(300,300);
	tri.m_pt[2] = cvPoint(100,300);

	CMyStar star(200,70, cvPoint(300,300));

	IplImage * img =cvLoadImage("c:\\test.jpg");
	IplImage * dst = cvCreateImage
		(cvSize(img->width, img->height), 8, 3);
	
	for(int x= 0; x<img->width; x++)
		for(int y=0; y<img->height; y++)
		{
			if(star.IsIn(cvPoint(x,y)))
				cvSet2D(dst, y, x, cvGet2D(img, y, x));
			else
				cvSet2D(dst, y, x, cvScalar(0,0,0));

		}
//	tri.Draw(dst);
//	star.Draw(dst);


	cvNamedWindow("img");
	cvShowImage("img", img);
	cvNamedWindow("dst");
	cvShowImage("dst", dst);


	cvWaitKey();

	cvDestroyWindow("img");
	cvReleaseImage(&img);
	cvDestroyWindow("dst");
	cvReleaseImage(&dst);
	
	return 0;
}

