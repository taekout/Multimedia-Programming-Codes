// painting.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

IplImage * img = NULL;
IplImage * dst = NULL;
CvPoint prePt = cvPoint(-1,-1);
CvPoint postPt = cvPoint(-1,-1);


void doMorphing()
{
	if(!img) return;
	if(prePt.x<0 || postPt.x<0) return;

	cvSet(dst, CV_RGB(0,0,0));
	printf("%d %d", prePt.x, postPt.x);
	int i;
	float x,y;
	float x1,y1;

	for(x=0; x<img->width; x+=0.25f)
		for(y=0; y<img->height; y+=0.25f)
		{
			y1 = y;
			x1 = x;
			float alpha;
			float dx1;
			float dx2;
			if (x<prePt.x)
			{
				alpha = x / float(prePt.x);
				dx1 = 0.0f;
				dx2 = postPt.x - prePt.x;
			}
			else
			{
				alpha = (x-prePt.x)/float(img->width - prePt.x);
				dx1 = postPt.x - prePt.x;
				dx2 = 0.0f;
			}			
			x1 = x + dx1*(1-alpha) + dx2*(alpha);

			if(x1<0 || x1>dst->width-1) continue;
			CvScalar s= cvGet2D(img, (int)y,(int)x);
			cvSet2D(dst,(int)y1,(int)x1,s);
		}

	cvShowImage("test", dst);
	cvCopy(dst, img);


	prePt = cvPoint(-1,-1);
	postPt = cvPoint(-1,-1);
	printf("done");
}


void myMouse( int event, int x, int y, int flags, void* param)
{
    if( !img )
        return;

	if(event == CV_EVENT_LBUTTONDOWN)
	{
		prePt = cvPoint(x,y);
		
	}
  	else if(event == CV_EVENT_LBUTTONUP)
	{
		postPt = cvPoint(x,y);

		doMorphing();
	}

}

int main(int argc, char* argv[])
{
	img = cvLoadImage("d:\\test.jpg");
	dst = cvCloneImage(img);
	cvSet(dst, CV_RGB(0,0,0));

	cvNamedWindow("test");
	cvShowImage("test", img);
	cvSetMouseCallback( "test", myMouse);

	cvWaitKey();
	
	cvDestroyWindow("test");
	cvReleaseImage(&img);	
	cvReleaseImage(&dst);	
	
	return 0;
}

