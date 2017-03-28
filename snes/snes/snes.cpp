#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <fstream>
#include <iostream>

using namespace std;

void sub(IplImage * src, IplImage *dst)
{
	for(int i = 0 ; i < src->height ; i++)
		for(int j = 0 ; j < src ->width ; j++)
		{
			if(j + 1 >= src ->width) continue;
			CvScalar f;
			f = cvGet2D(src, i, j);
			CvScalar f2;
			f2 = cvGet2D(src, i, j+1);
			CvScalar g;
			g.val[0] = f.val[0] - f2.val[0];
			g.val[1] = f.val[1] - f2.val[1];
			g.val[2] = f.val[2] - f2.val[2];
			cvSet2D(dst, i, j, g);
		}
}

void sub2(IplImage *src, IplImage *dst)
{
	for(int i = 0 ; i < src->height ; i++)
		for(int j = 0 ; j < src ->width ; j++)
		{
			if(i + 1 >= src ->height) continue;
			CvScalar f;
			f = cvGet2D(src, i, j);
			CvScalar f2;
			f2 = cvGet2D(src, i+1, j);
			CvScalar g;
			g.val[0] = f.val[0] - f2.val[0];
			g.val[1] = f.val[1] - f2.val[1];
			g.val[2] = f.val[2] - f2.val[2];
			cvSet2D(dst, i, j, g);
		}
}

int ** runLength(IplImage *img)
{
	CvScalar **f = new CvScalar*[1000];
	for(int i = 0 ; i < 1000 ; i++)
	{
		f[i] = new CvScalar[1000];
		for(int j = 0;  j < 1000 ; j++)
			f[i][j].val[0] = 0;
	}
	int **nLen = new int*[1000];
	for(int i = 0 ; i < 1000 ; i++)
	{
		nLen[i] = new int[1000];
		for(int j = 0 ; j < 1000 ; j++)
			nLen[i][j] = 0;
	}
	int nIndex = 0;
	for(int h = 0 ; h < img ->height ; h++)
	{
		int j = 0 ; int nCount = -1;
		nIndex = 0;
		for(int i = 0 ; i < img ->width ; i++)
		{
			j = i;
			CvScalar tmp; tmp.val[0] = 0;
			CvScalar st; st = cvGet2D(img, h, i);
			CvScalar en; en = cvGet2D(img, h, j);
			while(st.val[0] == en.val[0] && (i + j) < img ->width)
			{
				j++;
				en = cvGet2D(img, h, j);
				tmp.val[0] = st.val[0];
			}
			nCount = j - i;
			nLen[h][nIndex] = nCount;
			f[h][nIndex].val[0] = tmp.val[0];
			nIndex ++;
			i = j;
		}
		nLen[h][nIndex] = -1;
	}
	return nLen;
}

int main(void)
{
	IplImage * img;
	img = cvLoadImage("fight.png");
	IplImage * dst = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage * dst2 = cvCreateImage(cvGetSize(img), 8, 3);


	cvNamedWindow("HelloCV");
	cvShowImage("HelloCV", img);

	sub(img, dst);
	cvNamedWindow("DST");
	cvShowImage("DST", dst);

	cvWaitKey();

	int **ppLen;
	ppLen = runLength(dst);


	ofstream out;
	out.open("runlength.txt");
	if(out.is_open())
	{
		for(int j = 0 ; j < 200 ; j++)
		{
			for(int i = 0 ; i < 200 ; i++)
			{
				if(ppLen[j][i] == -1)
					i = 200;
				out << ppLen[j][i];
				out << ",";
			}
			out << endl;
		}
		out.close();
	}
	unsigned int total = 0;
	for(int j = 0; j < 200 ; j++)
	{
		for(int i = 0 ; i < 200 ; i++)
		{
			total+=ppLen[j][i];
		}
	}
	//55351 
	//92160
	cout << total;
	cvSaveImage("subtract.jpg", dst);
	cvDestroyAllWindows();
	cvReleaseImage(&img);
	return 1;
}