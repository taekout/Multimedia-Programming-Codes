#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

IplImage *dst;

void onMouse(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		cvSet2D(dst, y, x, CV_RGB(0,0,0));
		cvShowImage("result", dst);
	}
}

int main()
{
	dst = cvCreateImage(cvSize(800,800), 8, 3);
	cvSet(dst, CV_RGB(255,255,255));

	cvNamedWindow("result");
	cvShowImage("result", dst);
	cvSetMouseCallback("result", onMouse);

	cvWaitKey();

	cvDestroyWindow("result");
	cvReleaseImage(&dst);

	return 0;
}

