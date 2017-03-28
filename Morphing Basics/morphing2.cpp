#include <stdio.h>
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

