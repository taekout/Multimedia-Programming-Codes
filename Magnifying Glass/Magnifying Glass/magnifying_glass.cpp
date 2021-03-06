// MP19re.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
// 처음클릭위치, 드래깅후위치

void MyMouse(int events, int x, int y, int flags, void * param);
void DoMagnify(IplImage * img, int x, int y);
int main(int argc, char * argv[])
{
	IplImage * img = cvLoadImage("c:\\test.jpg");

	cvNamedWindow("img");
	cvShowImage("img", img);

	cvSetMouseCallback("img", MyMouse, (void *) img);

	cvWaitKey();

	cvDestroyWindow("img");
	cvReleaseImage(&img);

	return 0;
}

void MyMouse(int events, int x, int y, int flags, void * param)
{
	IplImage * img = (IplImage *) param;

	if(flags == CV_EVENT_FLAG_LBUTTON)
	{
		DoMagnify(img,x,y);
	}
	else if(events == CV_EVENT_LBUTTONUP)
	{

		cvShowImage("img",img);
	}
}
void DoMagnify(IplImage * img, int x, int y)
{
	IplImage * tmp = cvCreateImage(cvSize(img->width,img->height),8,3);
	cvCopy(img,tmp);

	float radius = 50;
	cvCircle(tmp,cvPoint(x,y),radius,cvScalar(1.0f,0,0),3);

	float x1,x2,y1,y2;

	float s = 2.0f;
	for( y2 = y-radius ; y2 < y+radius ; y2++)
		for( x2 = x-radius ; x2 < x+radius ; x2++)
		{
			float dist = (x2-x)*(x2-x)+(y2-y)*(y2-y);

			if(dist > radius*radius) continue;

			x1 = 1/s*(x2-x)+x;
			y1 = 1/s*(y2-y)+y;

			if(x1 < 0 || x1 > img->width-1) continue;
			if(y1 < 0 || y1 > img->height-1) continue;
			if(x2 < 0 || x2 > img->width-1) continue;
			if(y2 < 0 || y2 > img->height-1) continue;

			CvScalar f = cvGet2D(img,y1,x1);
			cvSet2D(tmp,y2,x2,f);

		}


		cvShowImage("img", tmp);
		cvReleaseImage(&tmp);
}

