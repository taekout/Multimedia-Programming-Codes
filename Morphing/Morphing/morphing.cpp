#include <stdio.h>
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <fstream>

#include "MatrixInverse.h"

using namespace std;

#define WINDOW1		1
#define	WINDOW2		2

int gSetPoint = 0;
IplImage *g_img;
IplImage *g_dst;
static int indexPoint1 = 0;
static int indexPoint2 = 0;
CvPoint pt1[50];
CvPoint pt2[50];
int ptIndexOri[50];
int ptIndexDst[50];
int nIndexTmp = 0;
int nIndexTmp2 = 0;
int bDataFromFile = false;

void ReadPointsFromFile(ifstream &file);

void OnMouse1(int event, int x, int y, int flags, void * param);
void OnMouse2(int event, int x, int y, int flags, void * param);

CvSubdiv2D *InitDelaunay(int width, int height);
CvSubdiv2D *init_delaunay(CvMemStorage *storage, CvRect rect);
void draw_subdiv( IplImage* img, CvSubdiv2D* subdiv, CvScalar delaunay_color, CvScalar voronoi_color );
void draw_subdiv_edge( IplImage* img, CvSubdiv2DEdge edge, CvScalar color , CvPoint *pt);
void locate_point( CvSubdiv2D* subdiv, CvPoint2D32f fp, IplImage* img, CvScalar active_color );

CvSubdiv2D *DelaunayLoop(IplImage *img, CvPoint *pt);

int main(int argc, char *argv[])
{
	IplImage *img = cvLoadImage("morph1.jpg");
	g_img = img;
	IplImage *dst = cvLoadImage("morph2.jpg");
	g_dst = dst;

	ifstream inputfile;
	inputfile.open("log.txt");
	if(inputfile.is_open() == true)
	{
		// Read Files.
		bDataFromFile = true;
		ReadPointsFromFile(inputfile);
	}

	cvNamedWindow("Morphing1");
	cvNamedWindow("Morphing2");
	cvShowImage("Morphing1", img);
	cvShowImage("Morphing2", dst);
	cvSetMouseCallback("Morphing1", OnMouse1);
	cvSetMouseCallback("Morphing2", OnMouse2);
	int key = 1;
	while(key != 27)
	{
		key = cvWaitKey();
		if(key == 'p' || key == 'P')
		{
			for(int i = 0 ; i < indexPoint1 ; i++)
			{
				cvCircle(img, cvPoint(pt1[i].x, pt1[i].y), 3, CV_RGB(255, 0, 0), -1);
			}
			for(int i = 0 ; i < indexPoint2 ; i++)
			{
				cvCircle(dst, cvPoint(pt2[i].x, pt2[i].y), 3, CV_RGB(0, 0, 255), -1);
			}
			cvShowImage("Morphing1", img);
			cvShowImage("Morphing2", dst);
		}
		if(key == 'd' || key == 'D')
		{
			DelaunayLoop(img, pt1);
			// For image2(==dst), I use the index acquired from the first DelaunayLoop.

			cvShowImage("Morphing1", img);
			cvShowImage("Morphing2", dst);
		}
	}

	// Create Log
	if(bDataFromFile == false)
	{
		ofstream fp;
		fp.open("log.txt");
		fp << indexPoint1 << endl;
		for(int i = 0 ; i < indexPoint1 ; i++)
		{
			fp << pt1[i].x << " " << pt1[i].y << endl;
		}
		fp << indexPoint2 << endl;
		for(int i = 0 ; i < indexPoint2 ; i++)
		{
			fp << pt2[i].x << " " << pt2[i].y << endl;
		}
		fp.close();
	}

	return	0;
}

CvSubdiv2D *DelaunayLoop(IplImage *img, CvPoint *pt)
{
	CvScalar active_facet_color = CV_RGB( 255, 0, 0 );
	CvScalar delaunay_color  = CV_RGB( 0,0,0);
	CvScalar voronoi_color = CV_RGB(0, 180, 0);
	// Process Delaunay Triangulation.
	CvRect rect = {0, 0, img ->width, img ->height};
	CvMemStorage *storage;
	storage = cvCreateMemStorage(0);
	CvSubdiv2D * subdiv;
	subdiv = init_delaunay(storage, rect);
	//draw_subdiv(img, subdiv, cvScalar(255, 0, 0), cvScalar(0, 0, 255));
	for(int i = 0; i < indexPoint1 ; i++)//indexPoint1 ; i++ )
	{
		CvPoint2D32f fp = cvPointTo32f(pt[i]);

		locate_point( subdiv, fp, img, active_facet_color );

		if( cvWaitKey(100) >= 0 )
			break;

		cvSubdivDelaunay2DInsert( subdiv, fp );
		cvCalcSubdivVoronoi2D( subdiv );
		//cvSet( img, bkgnd_color, 0 );
		//draw_subdiv( img, subdiv, delaunay_color, voronoi_color );

		if( cvWaitKey(100) >= 0 )
			break;
	}
	draw_subdiv( img, subdiv, delaunay_color, voronoi_color );
	return subdiv;
}

void locate_point( CvSubdiv2D* subdiv, CvPoint2D32f fp, IplImage* img,
				  CvScalar active_color )
{
	CvSubdiv2DEdge e;
	CvSubdiv2DEdge e0 = 0;
	CvSubdiv2DPoint* p = 0;

	cvSubdiv2DLocate( subdiv, fp, &e0, &p );

	if( e0 )
	{
		e = e0;
		do
		{
//			draw_subdiv_edge( img, e, active_color );
			e = cvSubdiv2DGetEdge(e,CV_NEXT_AROUND_LEFT);
		}
		while( e != e0 );
	}

	cvCircle(img, cvPoint(cvRound(fp.x), cvRound(fp.y)), 3, active_color, CV_FILLED, 8, 0 );
}



void draw_subdiv( IplImage* img, CvSubdiv2D* subdiv, CvScalar delaunay_color, CvScalar voronoi_color )
{
	CvSeqReader  reader;
	int i, total = subdiv->edges->total;
	int elem_size = subdiv->edges->elem_size;

	cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );

	for( i = 0; i < total; i++ )
	{
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

		if( CV_IS_SET_ELEM( edge ))
		{
//			draw_subdiv_edge(img, (CvSubdiv2DEdge)edge + 1, voronoi_color );
			draw_subdiv_edge(img, (CvSubdiv2DEdge)edge, delaunay_color, pt1);
		}
/*
		CvSubdiv2DEdge delEdge = (CvSubdiv2DEdge)edge;
		CvSubdiv2DPoint *pDelOrg = cvSubdiv2DEdgeOrg(delEdge);
		CvSubdiv2DPoint *pDelDst = cvSubdiv2DEdgeOrg(delEdge);
		CvPoint2D32f pt32f_org = pDelOrg ->pt;
		CvPoint2D32f pt32f_dst = pDelDst ->pt;
		CvPoint ptOrg = cvPointFrom32f(pt32f_org);
		CvPoint ptDst = cvPointFrom32f(pt32f_dst);*/


		CV_NEXT_SEQ_ELEM( elem_size, reader );
	}
}

void draw_subdiv_edge( IplImage* img, CvSubdiv2DEdge edge, CvScalar color, CvPoint *pt)
{
	CvSubdiv2DPoint* org_pt;
	CvSubdiv2DPoint* dst_pt;
	CvPoint2D32f org;
	CvPoint2D32f dst;
	CvPoint iorg, idst;

	org_pt = cvSubdiv2DEdgeOrg(edge);
	dst_pt = cvSubdiv2DEdgeDst(edge);

	if( org_pt && dst_pt )
	{
		org = org_pt->pt;
		dst = dst_pt->pt;

		iorg = cvPoint( cvRound( org.x ), cvRound( org.y ));
		idst = cvPoint( cvRound( dst.x ), cvRound( dst.y ));

		cvLine( img, iorg, idst, color, 1, CV_AA, 0 );
		cout << iorg.x << " " << iorg.y << endl;
		cvWaitKey(1000);
		cvShowImage("Morphing1", img);
		// Get Indices of the points from pt1. and then match it with pt2 later.
		for(int i = 0 ; i < indexPoint1 ; i++)
		{
			if(pt[i].x == iorg.x && pt[i].y == iorg.y)
			{
				ptIndexOri[nIndexTmp] = i;
				nIndexTmp ++;
			}
			if(pt[i].x == idst.x && pt[i].y == idst.y)
			{
				ptIndexDst[nIndexTmp2] = i;
				nIndexTmp2 ++; 
			}
		}
	}
}

void ReadPointsFromFile(ifstream & inputfile)
{
	inputfile >> indexPoint1;
	for(int i = 0 ; i < indexPoint1 ; i++)
	{
		inputfile >> pt1[i].x;
		inputfile >> pt1[i].y;
	}
	inputfile >> indexPoint2;
	for(int i = 0 ; i < indexPoint2 ; i++)
	{
		inputfile >> pt2[i].x;
		inputfile >> pt2[i].y;
	}
}

CvSubdiv2D *InitDelaunay(int width, int height)
{
	CvRect rect = {0, 0,width, height};
	CvMemStorage *storage;
	storage = cvCreateMemStorage(0);
	CvSubdiv2D * subdiv;
	subdiv = init_delaunay(storage, rect);
	return subdiv;
}

CvSubdiv2D *init_delaunay(CvMemStorage *storage, CvRect rect)
{
	CvSubdiv2D * subdiv;
	subdiv = cvCreateSubdiv2D(CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv), sizeof(CvSubdiv2DPoint), sizeof(CvQuadEdge2D), storage);
	cvInitSubdivDelaunay2D(subdiv, rect);

	// Add points
	// Not did this this time.
	return	subdiv;
}


void OnMouse1(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_LBUTTONDOWN && flags == CV_EVENT_FLAG_LBUTTON)
	{
		pt1[indexPoint1] = cvPoint(x, y);
		cvCircle(g_img, pt1[indexPoint1], 3, CV_RGB(255,0,0), -1);
		cvShowImage("Morphing1", g_img);
		indexPoint1 ++;
	}
}

void OnMouse2(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_LBUTTONDOWN && flags == CV_EVENT_FLAG_LBUTTON)
	{
		pt2[indexPoint2] = cvPoint(x, y);
		cvCircle(g_dst, pt2[indexPoint2], 3, CV_RGB(0,0,255), -1);
		cvShowImage("Morphing2", g_dst);
		indexPoint2 ++;
		cout << x << " " << y << endl;
	}
}


/*

int main(int argc, char* argv[])
{

	IplImage * img = cvLoadImage("morph1.jpg");
	IplImage *dst;
	dst = cvCreateImage(cvSize(800,800), 8, 3);

	int ix1[3]={250,100,400};
	int iy1[3]={100,350,350};
	int ix2[3]={250,50,450};
	int iy2[3]={100,350,100};

	int i;
	for(i=0; i<3; i++)
	{
		cvCircle(img, cvPoint(ix1[i], iy1[i]), 4, CV_RGB(255,0,0), -1);
		cvCircle(img, cvPoint(ix2[i], iy2[i]), 4, CV_RGB(0,0,255), -1);
	}

	float a[3][3], ia[3][3];

	for(i=0; i<3; i++)
	{
		a[i][0] = ix1[i];
		a[i][1] = iy1[i];
		a[i][2] = 1.0f;
	}

	InverseMatrixGJ(a, ia, 3);


//M = inverse( a )
//           ( b )
//
//N = ( c )
//    ( d )
//
//              % P_abc = inv(abc) * P
//			  % and then express the P_abc in the a'b'c' space. Then it will be like moving the old triangle P to the new triangle area

//transformation --> NMP (P= point)

	float m[3][3] = {{0,0,0},{0,0,0},{0,0,1}};

	for(i=0; i<3; i++)
	{
		m[0][0] += ia[0][i]*ix2[i];
		m[0][1] += ia[1][i]*ix2[i];
		m[0][2] += ia[2][i]*ix2[i];
		m[1][0] += ia[0][i]*iy2[i];
		m[1][1] += ia[1][i]*iy2[i];
		m[1][2] += ia[2][i]*iy2[i];
	}

	int w_1 = img->width;
	int h_1 = img->height;
	int w_2 = dst->width;
	int h_2 = dst->height;

	float x1,y1, x2,y2 , w1=1, w2 =1;
	for(y1=0; y1<h_1; y1 += 0.5)
		for(x1=0; x1<w_1; x1 += 0.5)
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

			CvScalar f = cvGet2D(img, y1, x1);
			cvSet2D(dst, y2, x2, f);

		}


		cvNamedWindow("test");
		cvShowImage("test", img);
		cvNamedWindow("test2");
		cvShowImage("test2", dst);

		cvWaitKey();

		cvDestroyWindow("test");
		cvDestroyWindow("test2");
		cvReleaseImage(&img);
		cvReleaseImage(&dst);


		return 0;
}

*/
/*


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
int i;
float x,y;
float x1,y1;

for(x=0; x<img->width; x+=0.25f)
for(y=0; y<img->height; y+=0.25f)
{
float alpha;
float beta;
float dx[4];
float dy[4];

if (x<prePt.x && y<prePt.y)
{
alpha = x / float(prePt.x);
beta = y / float(prePt.y);
dx[0] = 0.0f;	dx[1] = 0.0f;	dx[2] = postPt.x - prePt.x;	dx[3] = 0.0f;
dy[0] = 0.0f;	dy[1] = 0.0f;	dy[2] = postPt.y - prePt.y;	dy[3] = 0.0f;
}
else if (x>=prePt.x && y<prePt.y)
{
alpha = (x-prePt.x)/float(img->width - prePt.x);
beta = y / float(prePt.y);
dx[0] = 0.0f;	dx[1] = 0.0f;	dx[2] = 0.0f;	dx[3] = postPt.x - prePt.x;
dy[0] = 0.0f;	dy[1] = 0.0f;	dy[2] = 0.0f;	dy[3] = postPt.y - prePt.y;
}			
else if (x>=prePt.x && y>=prePt.y)
{
alpha = (x-prePt.x)/float(img->width - prePt.x);
beta =  (y-prePt.y)/float(img->height - prePt.y);
dx[0] = postPt.x - prePt.x;	dx[1] = 0.0f;	dx[2] = 0.0f;	dx[3] = 0.0f;
dy[0] = postPt.y - prePt.y;	dy[1] = 0.0f;	dy[2] = 0.0f;	dy[3] = 0.0f;
}			
else if (x<prePt.x && y>=prePt.y)
{
alpha = x / float(prePt.x);
beta =  (y-prePt.y)/float(img->height - prePt.y);
dx[0] = 0.0f;	dx[1] = postPt.x - prePt.x;	dx[2] = 0.0f;	dx[3] = 0.0f;
dy[0] = 0.0f;	dy[1] = postPt.y - prePt.y;	dy[2] = 0.0f;	dy[3] = 0.0f;
}
else {x1 = x; y1=y;}

x1 = x+ (1-alpha)*(1-beta)*dx[0] 
+(alpha)*(1-beta)*dx[1]
+(alpha)*(beta)*dx[2]
+(1-alpha)*(beta)*dx[3];
y1 = y+ (1-alpha)*(1-beta)*dy[0] 
+(alpha)*(1-beta)*dy[1]
+(alpha)*(beta)*dy[2]
+(1-alpha)*(beta)*dy[3];

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
	img = cvLoadImage("cross1.jpg");
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
*/
/*

// painting.cpp : ÄÜ¼Ö ÀÀ¿ë ÇÁ·Î±×·¥¿¡ ´ëÇÑ ÁøÀÔÁ¡À» Á¤ÀÇÇÕ´Ï´Ù.
//
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
img = cvLoadImage("c:\\test.jpg");
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




*/


