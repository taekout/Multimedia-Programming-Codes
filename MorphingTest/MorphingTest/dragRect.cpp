#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

IplImage *dst;
int x_pre;
int y_pre;

void onMouse(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		x_pre = x;
		y_pre = y;
	}
	else if(event == CV_EVENT_LBUTTONUP)
	{
		cvRectangle(dst, cvPoint(x_pre,y_pre), cvPoint(x,y), CV_RGB(255,0,0), -1);
		cvRectangle(dst, cvPoint(x_pre,y_pre), cvPoint(x,y), CV_RGB(0,0,255), 2);
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

