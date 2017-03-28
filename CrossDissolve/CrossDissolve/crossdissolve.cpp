// Bracost.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include "VideoManager.h"

using namespace std;

IplImage * GetCrossDissolve(IplImage *src, IplImage *dst, float t);

int main(int argc, CHAR* argv[])
{
	IplImage * src = cvLoadImage("cross1.jpg");
	IplImage * dst = cvLoadImage("cross2.jpg");
	IplImage * transition = NULL;

	if(src == NULL)
	{
		cout << "Failed in loading" << endl;
		return -1;
	}
	if(dst == NULL)
	{
		cout << "Failed in loading" << endl;
		return -1;
	}

	cvNamedWindow("src");
	cvShowImage("src", src);
	cvNamedWindow("dst");
	cvShowImage("dst", dst);

	VideoManager myVideo;
	myVideo.Initialize("cross_dissolve.avi", 0, 60, cvGetSize(src), 1);
	char buf[20];
	int i = 0 ;
	string tmp;
	for(float t = 0.0f ; t <= 1.0 ; t += 0.01, i++)
	{
		cout << t << endl;
		transition = GetCrossDissolve(src, dst, t);
		myVideo.AddFrame(transition);
		tmp = "Cross_Dissolve_Image";
		itoa(i, buf, 10);
		tmp += buf;
		tmp += ".jpg";
		cvSaveImage(tmp.c_str(), transition);
		cvShowImage("transition", transition);
		cvWaitKey(20);
	}
	myVideo.ReleaseVideoWrite();

	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&transition);
	cvDestroyAllWindows();

	return 0;
}

IplImage * GetCrossDissolve(IplImage *src, IplImage *dst, float t)
{
	IplImage * transition = cvCreateImage(cvGetSize(src), 8, 3);
	int height = src ->height;
	int width = src ->width;
	for(int j = 0 ; j < height ; j++)
	{
		for(int i = 0 ; i < width ; i++)
		{
			CvScalar f1 = cvGet2D(src, j, i);
			CvScalar f2 = cvGet2D(dst, j, i);
			CvScalar f;
			for(int k = 0 ; k < 3 ; k++)
			{
				if(f2.val[k] > 255.0)
					cout << "A" << endl;
			}
			f.val[0] = (f1.val[0] * (1-t) + f2.val[0] * t);
			f.val[1] = (f1.val[1] * (1-t) + f2.val[1] * t);
			f.val[2] = (f1.val[2] * (1-t) + f2.val[2] * t);
			cvSet2D(transition, j, i, f);
		}
	}
	return transition;
}