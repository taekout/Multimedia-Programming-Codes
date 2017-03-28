// Image Classification.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace std;
using namespace cv;

#define FILENAME "Images\\image_0.jpg"

class ImagePro
{
public:
	IplImage *m_img;
	int histogram[3][256];
	ImagePro(string imagename)
	{
		m_img = cvLoadImage(imagename.c_str());
		if(m_img == NULL)
		{
			cout << "Image not exist." << endl;
			exit(-1);
		}
		for(int i = 0 ; i < 3 ; i++)
		{
			for(int  j = 0 ; j < 256 ; j++)
			{
				histogram[i][j] = 0;
			}
		}
	}
	~ImagePro(){
		if(m_img != NULL)
			cvReleaseImage(&m_img);
	}

	void MakeHistogram(void);
	void DisplayHistogram(void);

	// Statistical Measurements
	float mean[3];
	float variance[3];
	float absoluteDeviation[3];
	float standardDeviation[3];
	float skewness[3];
	float kurtosis[3];
	// Statistics functions
	void ComputeStatistics(void);
};

/////////////////////////////////////////////////////////////////////////////
//
//                                          Histogram Graph
//
/////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	ImagePro image(FILENAME);
	image.MakeHistogram();
	image.DisplayHistogram();
	image.ComputeStatistics();

	cout << "mean : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.mean[i] << endl;
	cout << "variance : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.variance[i] << endl;
	cout << "absoluteDeviation : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.absoluteDeviation[i] << endl;
	cout << "standardDeviation : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.standardDeviation[i] << endl;
	cout << "skewness : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.skewness[i] << endl;
	cout << "kurtosis : ";
	for(int i = 0 ; i < 3 ; i++)
		cout << image.kurtosis[i] << endl;

	return	1;
}

void ImagePro::MakeHistogram(void)
{
	// Count to make a histogram.
	int width = m_img ->width;
	int height = m_img ->height;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int x = 0 ; x < width ; x++)
		{
			for(int y = 0 ; y < height ; y++)
			{
				CvScalar f = cvGet2D(m_img, y, x);
				histogram[k][int(f.val[k])]++;                // ÇØ´ç ¹à±â °ªÀÇ ¼ö + 1
			}
		}
//		for(int bri = 0 ; bri < 256 ; bri++)
//			cout << k << "ÀÇ ¹à±â " << bri << " : " << histogram[k][bri] << endl;
	}
}

void ImagePro::DisplayHistogram(void)
{
	int i, j, k;
	IplImage *histoImg = cvCreateImage(cvSize(256,600),8,3);
	cvSet(histoImg, CV_RGB(255,255,255));
	float value;

	int height = m_img ->height/3-1;
	for(i = 0 ; i < 3 ; i++)
	{
		for(j = 0 ; j < 256 ; j++)
		{
			value = histogram[i][j] / float(m_img->height*m_img->width/256*4.0f)*height;
			if(value >= height) value = height-1;
			for(k = 0 ; k < int(value) ; k++)
			{
				if(i == 0) cvSet2D(histoImg, (i + 1) * height - k, j, CV_RGB(0, 0, 255));
				if(i == 1) cvSet2D(histoImg, (i + 1) * height - k, j, CV_RGB(0, 255, 0));
				if(i == 2) cvSet2D(histoImg, (i + 1) * height - k, j, CV_RGB(255, 0, 0));
			}
		}
	}
	return;
}

void ImagePro::ComputeStatistics(void)
{
	double total[3] = {0.0};
	int width = m_img ->width, height = m_img ->height;
	int N = width * height;

	// Compute Mean
	CvScalar f;
	double tmp;
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height ; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				total[k] += f.val[k];
			}
		}
		total[k] /= N;
		mean[k] = total[k];
	}

	// Compute Variance
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height ; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				tmp = abs(f.val[k] - mean[k]);
				tmp = tmp * tmp;
				total[k] += tmp;
			}
		}
		total[k] /= N;
		variance[k] = total[k];
	}

	// Compute Standard Deviation
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				tmp = f.val[k] - mean[k];
				tmp = tmp * tmp;
				total[k] += tmp;
			}
		}
		total[k] /= (N-1);
		standardDeviation[k] = sqrt(total[k]);
	}

	// Compute Skewness
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				tmp = f.val[k] - mean[k];
				tmp /= standardDeviation[k];
				tmp = tmp * tmp * tmp;
				total[k] += tmp;
			}
		}
		total[k] /= N;
		skewness[k] = total[k];
	}

	// Compute Kurtosis
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				tmp = f.val[k] - mean[k];
				tmp /= standardDeviation[k];
				tmp = tmp * tmp * tmp * tmp;
				total[k] += tmp;
			}
		}
		total[k] /= N;
		total[k] -= 3;
		kurtosis[k] = total[k];
	}
}







