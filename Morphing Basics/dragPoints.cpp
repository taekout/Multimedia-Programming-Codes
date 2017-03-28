#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

void onMouse1(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON)
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

