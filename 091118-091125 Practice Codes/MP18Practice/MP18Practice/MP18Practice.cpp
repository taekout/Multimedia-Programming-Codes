// MP18Practice.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "MyButton.h"

IplImage * img = NULL;
IplImage * menu = NULL;
IplImage * tmp = NULL;

int m_mode = 0;

CvPoint g_pt;
CvScalar g_c;

CMyButton g_but[4];


void MyMouse(int events, int x, int y, int flags, void * param)
{
	
	switch(events){
	case CV_EVENT_LBUTTONDOWN:{
			g_pt = cvPoint(x,y);
		}
		break;
	case CV_EVENT_MOUSEMOVE:{
			if(flags == CV_EVENT_FLAG_LBUTTON)
			{
				cvCopy(img, tmp);
				cvRectangle(tmp, g_pt, cvPoint(x,y), g_c, -1);
				for(int x1=g_pt.x; x1<x; x1++)
					for(int y1 =g_pt.y; y1<y; y1++)
					{
						CvScalar f, g;
						f = cvGet2D(tmp, y1, x1);
						g = cvGet2D(img, y1, x1);
						cvSet2D(tmp, y1, x1, 
							cvScalar((f.val[0]+g.val[0])/2, (f.val[1]+g.val[1])/2,
										  (f.val[2]+g.val[2])/2));
					}
				cvShowImage("img", tmp);
			}			
		}
		break;
	case CV_EVENT_LBUTTONUP:	{
			cvRectangle(img, g_pt, cvPoint(x,y), g_c, -1);
			cvShowImage("img", img);
		}
		break;
	default:
		break;
	}
}

void MyPal(int events, int x, int y, int flags, void * param)
{
	switch(events)
	{
	case CV_EVENT_LBUTTONDOWN:
		for(int i=0; i<4; i++)
			if(g_but[i].HitTest(cvPoint(x,y)))
				g_but[i].DoIt(x,y);
		break;
	default:
		break;
	}
}


void SelectColor(int x, int y)
{
	g_c = 	cvGet2D(menu, y, x);

}

void SelectRect(int x, int y)
{
	m_mode =1;
}

void SelectCircle(int x, int y)
{
	m_mode =2;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_c = cvScalar(128,128,128);

	img = cvCreateImage(cvSize(300,300), 8, 3);
	tmp = cvCreateImage(cvSize(img->width, img->height), 8,3);

	IplImage * pal = cvLoadImage("d:\\palette.bmp");
	IplImage * but = cvLoadImage("d:\\button.jpg");
	menu = cvCreateImage(cvSize(300,pal->height + but->height), 8, 3);
	cvSet(img, CV_RGB(255,255,255));
	for(int x = 0; x<pal->width; x++)
		for(int y = 0; y<pal->height; y++)
			cvSet2D(menu, y,x, cvGet2D(pal, y, x));
	for(int x = 0; x<but->width; x++)
		for(int y = 0; y<but->height; y++)
			cvSet2D(menu, y+pal->height,x, cvGet2D(but, y, x));

	g_but[0].m_index = 0;
	g_but[0].m_pt1 = cvPoint(0,0);
	g_but[0].m_pt2 = cvPoint(pal->width,pal->height);
	g_but[0].func = SelectColor;


	cvNamedWindow("img");
	cvNamedWindow("menu");
	cvSetMouseCallback("img", MyMouse);
	cvSetMouseCallback("menu", MyPal);

	cvShowImage("img", img);
	cvShowImage("menu", menu);
	
	cvWaitKey();

	cvDestroyWindow("img");
	cvDestroyWindow("menu");
	cvReleaseImage(&img);
	cvReleaseImage(&menu);
	cvReleaseImage(&tmp);




	return 0;
}

