// Image Classification.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <fstream>
#include <stdlib.h>

#define	CSV_FILE_WRITE_MODE
#define	NUMBER_OF_IMAGES	113

using namespace std;
using namespace cv;

#define FILENAME "image_"
#define CSV_NAME "features.csv"
#define CSV_NORMALIZED_NAME	"normalized_features.csv"

float meanMax[3], varianceMax[3], absoluteDeviationMax[3], standardDeviationMax[3], skewnessMax[3], kurtosisMax[3];
float meanMin[3], varianceMin[3], absoluteDeviationMin[3], standardDeviationMin[3], skewnessMin[3], kurtosisMin[3];

float uniform_0dMax[3], correlation_0dMax[3], sumSquares_0dMax[3], invDifMoments_0dMax[3], sumAvg_0dMax[3], entropy_0dMax[3];
float uniform_45dMax[3], correlation_45dMax[3], sumSquares_45dMax[3], invDifMoments_45dMax[3], sumAvg_45dMax[3], entropy_45dMax[3];
float uniform_90dMax[3], correlation_90dMax[3], sumSquares_90dMax[3], invDifMoments_90dMax[3], sumAvg_90dMax[3], entropy_90dMax[3];
double contrast_0dMax[3], contrast_45dMax[3], contrast_90dMax[3];

float uniform_0dMin[3], correlation_0dMin[3], sumSquares_0dMin[3], invDifMoments_0dMin[3], sumAvg_0dMin[3], entropy_0dMin[3];
float uniform_45dMin[3], correlation_45dMin[3], sumSquares_45dMin[3], invDifMoments_45dMin[3], sumAvg_45dMin[3], entropy_45dMin[3];
float uniform_90dMin[3], correlation_90dMin[3], sumSquares_90dMin[3], invDifMoments_90dMin[3], sumAvg_90dMin[3], entropy_90dMin[3];
double contrast_0dMin[3], contrast_45dMin[3], contrast_90dMin[3];

float nEdgeMin, nEdgeMax;
double edge_distributionMin, edge_distributionMax;
long long histogram_distributionMin[3];
double histogram_distributionMax[3];

void InitializeGlobalData(void);

class ImagePro
{
public:
	// Member Image
	IplImage *m_img;
	string imagename;

	// Histogram Array
	int **histogram;
	unsigned long long histogram_distribution[3];
	double histogram_distribution_normalized[3];
	// Co Occurrence Matrix
	int ***occurM_0d;//[3][256][256];
	int ***occurM_45d;//[3][256][256];
	int ***occurM_90d;//[3][256][256];
	ImagePro() : m_img(NULL)
	{
		InitializeData();
	}

	ImagePro(string imagename) : m_img(NULL)
	{
		histogram_distribution[0] = 0.0; histogram_distribution[1] = 0.0; histogram_distribution[2] = 0.0;
		m_img = cvLoadImage(imagename.c_str());
		if(m_img == NULL)
		{
			cout << "Image not exist." << endl;
			exit(-1);
		}
		InitializeData();
		this ->imagename = imagename;
	}
	~ImagePro(){
		if(m_img != NULL)
			cvReleaseImage(&m_img);
	}

	void MakeHistogram(void);
	void DisplayHistogram(void);

	// Statistical Measurements
	float mean[3], variance[3], absoluteDeviation[3], standardDeviation[3], skewness[3], kurtosis[3];
//	float meanMax[3], varianceMax[3], absoluteDeviationMax[3], standardDeviationMax[3], skewnessMax[3], kurtosisMax[3];
//	float meanMin[3], varianceMin[3], absoluteDeviationMin[3], standardDeviationMin[3], skewnessMin[3], kurtosisMin[3];
	// Co-occurrence Features
	//  0 degree occurrence data, 45 degree occurrence data, 90 degree occurrence data, 
	float uniform_0d[3],  correlation_0d[3],  sumSquares_0d[3],  invDifMoments_0d[3],  sumAvg_0d[3],  entropy_0d[3];
	float uniform_45d[3], correlation_45d[3], sumSquares_45d[3], invDifMoments_45d[3], sumAvg_45d[3], entropy_45d[3];
	float uniform_90d[3], correlation_90d[3], sumSquares_90d[3], invDifMoments_90d[3], sumAvg_90d[3], entropy_90d[3];
	double contrast_0d[3], contrast_45d[3], contrast_90d[3];

	float nEdgeCount;
	float nBlockEdgeCount[10][10];
	double edge_distribution;
//	float uniform_0dMax[3], correlation_0dMax[3], sumSquares_0dMax[3], invDifMoments_0dMax[3], sumAvg_0dMax[3], entropy_0dMax[3];
//	float uniform_45dMax[3], correlation_45dMax[3], sumSquares_45dMax[3], invDifMoments_45dMax[3], sumAvg_45dMax[3], entropy_45dMax[3];
//	float uniform_90dMax[3], correlation_90dMax[3], sumSquares_90dMax[3], invDifMoments_90dMax[3], sumAvg_90dMax[3], entropy_90dMax[3];
//	double contrast_0dMax[3], contrast_45dMax[3], contrast_90dMax[3];

//	float uniform_0dMin[3], correlation_0dMin[3], sumSquares_0dMin[3], invDifMoments_0dMin[3], sumAvg_0dMin[3], entropy_0dMin[3];
//	float uniform_45dMin[3], correlation_45dMin[3], sumSquares_45dMin[3], invDifMoments_45dMin[3], sumAvg_45dMin[3], entropy_45dMin[3];
//	float uniform_90dMin[3], correlation_90dMin[3], sumSquares_90dMin[3], invDifMoments_90dMin[3], sumAvg_90dMin[3], entropy_90dMin[3];
//	double contrast_0dMin[3], contrast_45dMin[3], contrast_90dMin[3];

	// Statistics functions
	void ComputeStatistics(void);
	void GetCoOccurrence(void);
	void ComputeCoOccurFeatures(void);
	void CountCannyEdges(void);
	void InitializeData(void);
	void DisplayData(void);
	void DisplayDataInFile(ofstream &out);
	void DisplayDataInFile2(ofstream &out);
	void NormalizeData(void);
};

class ImageFeature : public ImagePro
{
public:
	ImageFeature() : ImagePro()
	{
	}
	void ReadDataFromFile(ifstream &in, string input_filename, ofstream &out, string output_filename, int nLines);
};

/////////////////////////////////////////////////////////////////////////////
//
//                                          Histogram Graph
//
/////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	string filename = FILENAME;
#ifdef	CSV_FILE_WRITE_MODE
	ofstream outCSV;
	outCSV.open(CSV_NAME);
	InitializeGlobalData();

	for(int i = 0 ; i < NUMBER_OF_IMAGES ; i++)
	{
		cout << i << endl;
		char buf[21];
		itoa(i, buf, 10);
		filename = FILENAME;
		filename = filename + buf + ".jpg";
		ImagePro imageManager(filename);
		imageManager.MakeHistogram();

		imageManager.ComputeStatistics();
		imageManager.GetCoOccurrence();
		imageManager.ComputeCoOccurFeatures();
		imageManager.CountCannyEdges();
		//image.DisplayData();
		imageManager.DisplayDataInFile(outCSV);
	}
	outCSV.close();
#endif

	cout << "Start normalizing" << endl;
	ifstream inCSV;
	ofstream outNormalized;
	ImageFeature csvManager;
	csvManager.ReadDataFromFile(inCSV, CSV_NAME, outNormalized, CSV_NORMALIZED_NAME, NUMBER_OF_IMAGES);
//	csvManager.NormalizeData();
//	csvManager.DisplayDataInFile(outCSVNormalized);

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

	// Compute histogram statistics
	for(int k = 0 ; k < 3 ; k++)
	{
		histogram_distribution[k] = 0.0;
		for(int i = 0 ; i < 256 ;i++)
		{
			histogram_distribution[k] += (histogram[k][i] * (i + 1));
		}
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		if(histogram_distribution[k] < histogram_distributionMin[k])
			histogram_distributionMin[k] = histogram_distribution[k];
		if(histogram_distribution[k] > histogram_distributionMax[k])
			histogram_distributionMax[k] = histogram_distribution[k];
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
	cvNamedWindow("Image");
	cvNamedWindow("Histogram");
	cvShowImage("Histogram", histoImg);
	cvShowImage("Image", m_img);
	cvWaitKey();
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
		mean[k] = float(total[k]);
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
		variance[k] = float(total[k]);
	}

	// Compute Absolute Deviation.
	total[0] = 0.0; total[1] = 0.0; total[2] = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < height; j++)
		{
			for(int i = 0 ; i < width ; i++)
			{
				f = cvGet2D(m_img, j, i);
				tmp = abs(f.val[k] - mean[k]);
				total[k] += tmp;
			}
		}
		total[k] /= (N);
		absoluteDeviation[k] = float(total[k]);
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
		standardDeviation[k] = float(sqrt(total[k]));
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
		skewness[k] = float(total[k]);
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
		kurtosis[k] = float(total[k]);
	}

	for(int k = 0 ; k < 3;  k++)
	{
		if(mean[k] > meanMax[k])
			meanMax[k] = mean[k];
		if(variance[k] > varianceMax[k])
			varianceMax[k] = variance[k];
		if(absoluteDeviation[k] > absoluteDeviationMax[k])
			absoluteDeviationMax[k] = absoluteDeviation[k];
		if(standardDeviation[k] > standardDeviationMax[k])
			standardDeviationMax[k] = standardDeviation[k];
		if(skewness[k] > skewnessMax[k])
			skewnessMax[k] = skewness[k];
		if(kurtosis[k] > kurtosisMax[k])
			kurtosisMax[k] = kurtosis[k];

		if(mean[k] < meanMin[k])
			meanMin[k] = mean[k];
		if(variance[k] < varianceMin[k])
			varianceMin[k] = variance[k];
		if(absoluteDeviation[k] < absoluteDeviationMin[k])
			absoluteDeviationMin[k] = absoluteDeviation[k];
		if(standardDeviation[k] < standardDeviationMin[k])
			standardDeviationMin[k] = standardDeviation[k];
		if(skewness[k] < skewnessMin[k])
			skewnessMin[k] = skewness[k];
		if(kurtosis[k] < kurtosisMin[k])
			kurtosisMin[k] = kurtosis[k];
	}
}


void ImagePro::GetCoOccurrence(void)
{
	int d = 3;
	int height = m_img ->height, width = m_img ->width;
	CvScalar f, g;
	// get occurrence Matrix for 0 degree
	for(int j = 0 ; j < height ; j++)
	{
		for(int i = 0 ; i < width - d ; i++)
		{
			// Get Matrix Indices.
			f = cvGet2D(m_img, j, i);
			g = cvGet2D(m_img, j, i + d);
			for(int k = 0 ; k < 3 ; k ++)
				occurM_0d[k][int(f.val[k])][int(g.val[k])] ++;
		}
	}
	// get occurrence Matrix for 45 degree
	for(int j = 0 ; j < height - d + 1; j++)
	{
		for(int i = 0 ; i < width - d + 1 ; i++)
		{
			// Get Matrix Indices.
			f = cvGet2D(m_img, j, i);
			g = cvGet2D(m_img, j + d - 1, i + d - 1);
			for(int k = 0 ; k < 3 ; k ++)
				occurM_45d[k][int(f.val[k])][int(g.val[k])] ++;
		}
	}
	// get occurrence Matrix for 90 degree
	for(int j = 0 ; j < height ; j++)
	{
		for(int i = 0 ; i < width - d ; i++)
		{
			// Get Matrix Indices.
			f = cvGet2D(m_img, j, i);
			g = cvGet2D(m_img, j, i + d);
			for(int k = 0 ; k < 3 ; k ++)
				occurM_90d[k][int(f.val[k])][int(g.val[k])] ++;
		}
	}
}


void ImagePro::InitializeData(void)
{
	nEdgeCount = 0;
	for(int i = 0 ; i < 10 ; i++)
	{
		for(int j = 0 ; j < 10 ; j++)
		{
			nBlockEdgeCount[i][j] = 0;
		}
	}

	histogram = new int*[3];
	for(int i = 0 ; i < 3 ; i++)
	{
		histogram[i] = new int[256];
	}
	for(int i = 0 ; i < 3 ; i++)
	{
		for(int  j = 0 ; j < 256 ; j++)
		{
			histogram[i][j] = 0;
		}
	}
	// Allocate memory for occurM_0d, occurM_45d, occurM_90d ==> [3][256][256]
	occurM_0d = new int**[3];
	occurM_45d = new int**[3];
	occurM_90d = new int**[3];
	for(int k = 0 ; k < 3 ; k++)
	{
		occurM_0d[k] = new int*[256];
		occurM_45d[k] = new int*[256];
		occurM_90d[k] = new int*[256];
		for(int j = 0 ; j < 256 ; j++)
		{
			occurM_0d[k][j] = new int[256];
			occurM_45d[k][j] = new int[256];
			occurM_90d[k][j] = new int[256];
		}
	}

	for(int k = 0 ; k < 3 ; k++)
	{
		for(int j = 0 ; j < 256 ; j++)
		{
			for(int i = 0 ; i < 256 ; i++)
			{
				occurM_0d[k][j][i] = 0;
				occurM_45d[k][j][i] = 0;
				occurM_90d[k][j][i] = 0;
			}
		}
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		mean[k] = 0;
		variance[k] = 0;
		absoluteDeviation[k] = 0;
		standardDeviation[k] = 0;
		skewness[k] = 0;
		kurtosis[k] = 0;
		uniform_0d[k] = 0.0f;
		uniform_45d[k] = 0.0f;
		uniform_90d[k] = 0.0f;
		contrast_0d[k] = 0.0f;
		contrast_45d[k] = 0.0f;
		contrast_90d[k] = 0.0f;
		correlation_0d[k] = 0.0f;
		correlation_45d[k] = 0.0f;
		correlation_90d[k] = 0.0f;
		sumSquares_0d[k] = 0.0f;
		sumSquares_45d[k] = 0.0f;
		sumSquares_90d[k] = 0.0f;
		invDifMoments_0d[k] = 0.0f;
		invDifMoments_45d[k] = 0.0f;
		invDifMoments_90d[k] = 0.0f;
		sumAvg_0d[k] = 0.0f;
		sumAvg_45d[k] = 0.0f;
		sumAvg_90d[k] = 0.0f;
		entropy_0d[k] = 0.0f;
		entropy_45d[k] = 0.0f;
		entropy_90d[k] = 0.0f;
		histogram_distribution[k] = 0;
		histogram_distribution_normalized[k] = -1.0;
	}
}

void ImagePro::ComputeCoOccurFeatures(void)
{
	// Compute uniformity. (0/45/90 degrees * 3 RGB components
	double total;
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int j = 0 ; j < 256 ;j++)
		{
			for(int i = 0 ; i < 256 ; i++)
			{
				total += (occurM_0d[k][j][i] * occurM_0d[k][j][i]);
			}
		}
		uniform_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int j = 0 ; j < 256 ;j++)
		{
			for(int i = 0 ; i < 256 ; i++)
			{
				total += (occurM_45d[k][j][i] * occurM_45d[k][j][i]);
			}
		}
		uniform_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int j = 0 ; j < 256 ;j++)
		{
			for(int i = 0 ; i < 256 ; i++)
			{
				total += (occurM_90d[k][j][i] * occurM_90d[k][j][i]);
			}
		}
		uniform_90d[k] = total;
	}

	// Compute Contrast
	float tmp = 0.0f;
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int n = 0 ; n < 255 ; n++)
		{
			tmp = 0.0f;
			for(int j = 0 ; j < 256 ;j++)
			{
				for(int i = 0 ; i < 256 ; i++)
				{
					tmp += occurM_0d[k][j][i];
				}
			}
			total += n * n * tmp;
		}
		contrast_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int n = 0 ; n < 255 ; n++)
		{
			tmp = 0.0f;
			for(int j = 0 ; j < 256 ;j++)
			{
				for(int i = 0 ; i < 256 ; i++)
				{
					tmp += occurM_45d[k][j][i];
				}
			}
			total += n * n * tmp;
		}
		contrast_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int n = 0 ; n < 255 ; n++)
		{
			tmp = 0.0f;
			for(int j = 0 ; j < 256 ;j++)
			{
				for(int i = 0 ; i < 256 ; i++)
				{
					tmp += occurM_90d[k][j][i];
				}
			}
			total += n * n * tmp;
		}
		contrast_90d[k] = total;
	}

	// Compute Correlation
	// First, I gotta compute the mean and standard deviation.
	double mean[3] = {0.0};
//	double tmp = 0.0;
	double standarddeviation[3] = {0.0};
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				tmp += occurM_0d[k][i][j];
			}
		}
		tmp /= (256 * 256);
		mean[k] = tmp;
	}
	tmp = 0.0;
	for(int k = 0 ; k < 3 ; k++)
	{
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				tmp += (occurM_0d[k][i][j] - mean[k]) * (occurM_0d[k][i][j] - mean[k]);
			}
		}
		tmp /= (256 * 256);
		tmp = sqrt(tmp);
		standarddeviation[k] = tmp;
	}	// Now we have mean[3] and standard deviation[3] so compute correlation_0d / 45d / 90d
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i * j * occurM_0d[k][i][j] - mean[k] * mean[k]);// see how much mean[k] is.. first
			}
		}
		total /= standarddeviation[k];
		total /= standarddeviation[k];
		correlation_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i * j * occurM_45d[k][i][j] - mean[k] * mean[k]);// see how much mean[k] is.. first
			}
		}
		total /= standarddeviation[k];
		total /= standarddeviation[k];
		correlation_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i * j * occurM_90d[k][i][j] - mean[k] * mean[k]);// see how much mean[k] is.. first
			}
		}
		total /= standarddeviation[k];
		total /= standarddeviation[k];
		correlation_90d[k] = total;
	}


	// Compute Sum of Squares
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i - j) * (i - j) * occurM_0d[k][i][j];
			}
		}
		sumSquares_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i - j) * (i - j) * occurM_45d[k][i][j];
			}
		}
		sumSquares_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += (i - j) * (i - j) * occurM_90d[k][i][j];
			}
		}
		sumSquares_90d[k] = total;
	}

	// Compute Inverse Difference Moments(0 / 45 / 90 degrees)
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += occurM_0d[k][i][j] / (1 + (i-j)*(i-j));
			}
		}
		invDifMoments_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += occurM_45d[k][i][j] / (1 + (i-j)*(i-j));
			}
		}
		invDifMoments_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				total += occurM_90d[k][i][j] / (1 + (i-j)*(i-j));
			}
		}
		invDifMoments_90d[k] = total;
	}

	// Compute Sum Average

	// Compute Entropy
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				if(occurM_0d[k][i][j] == 0)
					total += 0;	// log 0 does not exist, so put the asymptote.
				else
					total += double(occurM_0d[k][i][j]) * log(double(occurM_0d[k][i][j]));
			}
		}
		total = -total;
		entropy_0d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				if(occurM_45d[k][i][j] == 0)
					total += 0;	// log 0 does not exist, so put the asymptote.
				else
					total += double(occurM_45d[k][i][j]) * log(double(occurM_45d[k][i][j]));
			}
		}
		total = -total;
		entropy_45d[k] = total;
	}
	for(int k = 0 ; k < 3 ; k++)
	{
		total = 0.0;
		for(int i = 0 ; i < 256 ; i++)
		{
			for(int j = 0 ; j < 256 ; j++)
			{
				if(occurM_90d[k][i][j] == 0)
					total += 0;	// log 0 does not exist, so put the asymptote.
				else
					total += double(occurM_90d[k][i][j]) * log(double(occurM_90d[k][i][j]));
			}
		}
		total = -total;
		entropy_90d[k] = total;
	}


	for(int k = 0 ;k < 3 ; k++)
	{
		// 0 degrees Max
		if(uniform_0d[k] > uniform_0dMax[k])
			uniform_0dMax[k] = uniform_0d[k];
		if(correlation_0d[k] > correlation_0dMax[k])
			correlation_0dMax[k] = correlation_0d[k];
		if(sumSquares_0d[k] > sumSquares_0dMax[k])
			sumSquares_0dMax[k] = sumSquares_0d[k];
		if(invDifMoments_0d[k] > invDifMoments_0dMax[k])
			invDifMoments_0dMax[k] = invDifMoments_0d[k];
		if(sumAvg_0d[k] > sumAvg_0dMax[k])
			sumAvg_0dMax[k] = sumAvg_0d[k];
		if(entropy_0d[k] > entropy_0dMax[k])
			entropy_0dMax[k] = entropy_0d[k];
		if(contrast_0d[k] > contrast_0dMax[k])
			contrast_0dMax[k] = contrast_0d[k];

		//0 degrees Min
		if(uniform_0d[k] < uniform_0dMin[k])
			uniform_0dMin[k] = uniform_0d[k];
		if(correlation_0d[k] < correlation_0dMin[k])
			correlation_0dMin[k] = correlation_0d[k];
		if(sumSquares_0d[k] < sumSquares_0dMin[k])
			sumSquares_0dMin[k] = sumSquares_0d[k];
		if(invDifMoments_0d[k] < invDifMoments_0dMin[k])
			invDifMoments_0dMin[k] = invDifMoments_0d[k];
		if(sumAvg_0d[k] < sumAvg_0dMin[k])
			sumAvg_0dMin[k] = sumAvg_0d[k];
		if(entropy_0d[k] < entropy_0dMin[k])
			entropy_0dMin[k] = entropy_0d[k];
		if(contrast_0d[k] < contrast_0dMin[k])
			contrast_0dMin[k] = contrast_0d[k];

		// 45 degrees Max
		if(uniform_45d[k] > uniform_45dMax[k])
			uniform_45dMax[k] = uniform_45d[k];
		if(correlation_45d[k] > correlation_45dMax[k])
			correlation_45dMax[k] = correlation_45d[k];
		if(sumSquares_45d[k] > sumSquares_45dMax[k])
			sumSquares_45dMax[k] = sumSquares_45d[k];
		if(invDifMoments_45d[k] > invDifMoments_45dMax[k])
			invDifMoments_45dMax[k] = invDifMoments_45d[k];
		if(sumAvg_45d[k] > sumAvg_45dMax[k])
			sumAvg_45dMax[k] = sumAvg_45d[k];
		if(entropy_45d[k] > entropy_45dMax[k])
			entropy_45dMax[k] = entropy_45d[k];
		if(contrast_45d[k] > contrast_45dMax[k])
			contrast_45dMax[k] = contrast_45d[k];

		//45 degrees Min
		if(uniform_45d[k] < uniform_45dMin[k])
			uniform_45dMin[k] = uniform_45d[k];
		if(correlation_45d[k] < correlation_45dMin[k])
			correlation_45dMin[k] = correlation_45d[k];
		if(sumSquares_45d[k] < sumSquares_45dMin[k])
			sumSquares_45dMin[k] = sumSquares_45d[k];
		if(invDifMoments_45d[k] < invDifMoments_45dMin[k])
			invDifMoments_45dMin[k] = invDifMoments_45d[k];
		if(sumAvg_45d[k] < sumAvg_45dMin[k])
			sumAvg_45dMin[k] = sumAvg_45d[k];
		if(entropy_45d[k] < entropy_45dMin[k])
			entropy_45dMin[k] = entropy_45d[k];
		if(contrast_45d[k] < contrast_45dMin[k])
			contrast_45dMin[k] = contrast_45d[k];

		// 90 degrees Max
		if(uniform_90d[k] > uniform_90dMax[k])
			uniform_90dMax[k] = uniform_90d[k];
		if(correlation_90d[k] > correlation_90dMax[k])
			correlation_90dMax[k] = correlation_90d[k];
		if(sumSquares_90d[k] > sumSquares_90dMax[k])
			sumSquares_90dMax[k] = sumSquares_90d[k];
		if(invDifMoments_90d[k] > invDifMoments_90dMax[k])
			invDifMoments_90dMax[k] = invDifMoments_90d[k];
		if(sumAvg_90d[k] > sumAvg_90dMax[k])
			sumAvg_90dMax[k] = sumAvg_90d[k];
		if(entropy_90d[k] > entropy_90dMax[k])
			entropy_90dMax[k] = entropy_90d[k];
		if(contrast_90d[k] > contrast_90dMax[k])
			contrast_90dMax[k] = contrast_90d[k];

		//90 degrees Min
		if(uniform_90d[k] < uniform_90dMin[k])
			uniform_90dMin[k] = uniform_90d[k];
		if(correlation_90d[k] < correlation_90dMin[k])
			correlation_90dMin[k] = correlation_90d[k];
		if(sumSquares_90d[k] < sumSquares_90dMin[k])
			sumSquares_90dMin[k] = sumSquares_90d[k];
		if(invDifMoments_90d[k] < invDifMoments_90dMin[k])
			invDifMoments_90dMin[k] = invDifMoments_90d[k];
		if(sumAvg_90d[k] < sumAvg_90dMin[k])
			sumAvg_90dMin[k] = sumAvg_90d[k];
		if(entropy_90d[k] < entropy_90dMin[k])
			entropy_90dMin[k] = entropy_90d[k];
		if(contrast_90d[k] < contrast_90dMin[k])
			contrast_90dMin[k] = contrast_90d[k];
	}
}

void ImagePro::DisplayData(void)
{
	cout << "mean : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << mean[i] << " ";
	cout << endl << "variance : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << variance[i] << " ";
	cout << endl << "absoluteDeviation : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << absoluteDeviation[i] << " ";
	cout << endl << "standardDeviation : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << standardDeviation[i] << " ";
	cout << endl << "skewness : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << skewness[i] << " ";
	cout << endl << "kurtosis : " << endl;
	for(int i = 0 ; i < 3 ; i++)
		cout << kurtosis[i] << " ";
	cout << endl;

	cout << "uniformity_0d : " << uniform_0d[0] << " " << uniform_0d[1] <<  " " << uniform_0d[2] << endl;
	cout << "uniformity_45d : " << uniform_45d[0] << " " << uniform_45d[1] <<  " " << uniform_45d[2] << endl;
	cout << "uniformity_90d : " << uniform_90d[0] << " " << uniform_90d[1] <<  " " << uniform_90d[2] << endl;

	cout << "contrast_0d : " << contrast_0d[0] << " " <<  contrast_0d[1] <<  " " << contrast_0d[2] << endl;
	cout << "contrast_45d : " << contrast_45d[0] << " " <<  contrast_45d[1] <<  " " << contrast_45d[2] << endl;
	cout << "contrast_90d : " << contrast_90d[0] << " " <<  contrast_90d[1] <<  " " << contrast_90d[2] << endl;

	cout << "correlation_0d : " << correlation_0d[0] <<  " " << correlation_0d[1] << " " <<  correlation_0d[2] << endl;
	cout << "correlation_45d : " << correlation_45d[0] <<  " " << correlation_45d[1] << " " <<  correlation_45d[2] << endl;
	cout << "correlation_90d : " << correlation_90d[0] <<  " " << correlation_90d[1] << " " <<  correlation_90d[2] << endl;

	cout << "sumSquares_0d : " << sumSquares_0d[0] <<  " " << sumSquares_0d[1] <<  " " << sumSquares_0d[2] << endl;
	cout << "sumSquares_45d : " << sumSquares_45d[0] <<  " " << sumSquares_45d[1] <<  " " << sumSquares_45d[2] << endl;
	cout << "sumSquares_90d : " << sumSquares_90d[0] <<  " " << sumSquares_90d[1] <<  " " << sumSquares_90d[2] << endl;

	cout << "invDifMoments_0d  : " << invDifMoments_0d[0] <<  " " << invDifMoments_0d[1] <<  " " << invDifMoments_0d[2] << endl;
	cout << "invDifMoments_45d  : " << invDifMoments_45d[0] <<  " " << invDifMoments_45d[1] <<  " " << invDifMoments_45d[2] << endl;
	cout << "invDifMoments_90d  : " << invDifMoments_90d[0] <<  " " << invDifMoments_90d[1] <<  " " << invDifMoments_90d[2] << endl;

	cout << "sumAvg_0d : " << sumAvg_0d[0] <<  " " << sumAvg_0d[1] <<  " " << sumAvg_0d[2] << endl;
	cout << "sumAvg_45d : " << sumAvg_45d[0] <<  " " << sumAvg_45d[1] <<  " " << sumAvg_45d[2] << endl;
	cout << "sumAvg_90d : " << sumAvg_90d[0] <<  " " << sumAvg_90d[1] <<  " " << sumAvg_90d[2] << endl;

	cout << "entropy_0d : " << entropy_0d[0] <<  " " << entropy_0d[1] <<  " " << entropy_0d[2] << endl;
	cout << "entropy_45d : " << entropy_45d[0] <<  " " << entropy_45d[1] <<  " " << entropy_45d[2] << endl;
	cout << "entropy_90d : " << entropy_90d[0] <<  " " << entropy_90d[1] <<  " " << entropy_90d[2] << endl;
}
/*

	//  0 degree occurrence data, 45 degree occurrence data, 90 degree occurrence data, 
	float uniform_0d[3],  contrast_0d[3],  correlation_0d[3],  sumSquares_0d[3],  invDifMoments_0d[3],  sumAvg_0d[3],  entropy_0d[3];
	float uniform_45d[3], contrast_45d[3], correlation_45d[3], sumSquares_45d[3], invDifMoments_45d[3], sumAvg_45d[3], entropy_45d[3];
	float uniform_90d[3], contrast_90d[3], correlation_90d[3], sumSquares_90d[3], invDifMoments_90d[3], sumAvg_90d[3], entropy_90d[3];
*/

void ImagePro::DisplayDataInFile(ofstream &out)
{
	out << imagename << " " << mean[0] << " " << mean[1] << " " << mean[2] << " "
		<< variance[0] << " " << variance[1] << " " << variance[2] << " "
		<< absoluteDeviation[0] << " " << absoluteDeviation[1] << " " << absoluteDeviation[2] << " "
		<< standardDeviation[0] << " " << standardDeviation[1] << " " << standardDeviation[2] << " "
		<< skewness[0] << " " << skewness[1] << " " << skewness[2] << " "
		<< kurtosis[0] << " " << kurtosis[1] << " " << kurtosis[2] << " "
		<< uniform_0d[0] << " "<< uniform_0d[1] << " " << uniform_0d[2] << " "
		<< uniform_45d[0] << " "<< uniform_45d[1] << " " << uniform_45d[2] << " "
		<< uniform_90d[0] << " "<< uniform_90d[1] << " " << uniform_90d[2] << " "
		<< contrast_0d[0] << " "<< contrast_0d[1] << " " << contrast_0d[2] << " "
		<< contrast_45d[0] << " "<< contrast_45d[1] << " " << contrast_45d[2] << " "
		<< contrast_90d[0] << " "<< contrast_90d[1] << " " << contrast_90d[2] << " "
		<< correlation_0d[0] << " "<< correlation_0d[1] << " " << correlation_0d[2] << " "
		<< correlation_45d[0] << " "<< correlation_45d[1] << " " << correlation_45d[2] << " "
		<< correlation_90d[0] << " "<< correlation_90d[1] << " " << correlation_90d[2] << " "
		<< sumSquares_0d[0] << " "<< sumSquares_0d[1] << " " << sumSquares_0d[2] << " "
		<< sumSquares_45d[0] << " "<< sumSquares_45d[1] << " " << sumSquares_45d[2] << " "
		<< sumSquares_90d[0] << " "<< sumSquares_90d[1] << " " << sumSquares_90d[2] << " "
		<< invDifMoments_0d[0] << " "<< invDifMoments_0d[1] << " " << invDifMoments_0d[2] << " "
		<< invDifMoments_45d[0] << " "<< invDifMoments_45d[1] << " " << invDifMoments_45d[2] << " "
		<< invDifMoments_90d[0] << " "<< invDifMoments_90d[1] << " " << invDifMoments_90d[2] << " "
		//<< sumAvg_0d[0] << " "<< sumAvg_0d[1] << " " << sumAvg_0d[2] << " "
		//<< sumAvg_45d[0] << " "<< sumAvg_45d[1] << " " << sumAvg_45d[2] << " "
		//<< sumAvg_90d[0] << " "<< sumAvg_90d[1] << " " << sumAvg_90d[2] << " "
		<< entropy_0d[0] << " "<< entropy_0d[1] << " " << entropy_0d[2] << " "
		<< entropy_45d[0] << " "<< entropy_45d[1] << " " << entropy_45d[2] << " "
		<< entropy_90d[0] << " "<< entropy_90d[1] << " " << entropy_90d[2] << " "
		<< nEdgeCount << " "
		<< edge_distribution << " "
		<< histogram_distribution[0] << " "
		<< histogram_distribution[1] << " "
		<< histogram_distribution[2] << " "
		<< endl;
}


void ImagePro::DisplayDataInFile2(ofstream &out)
{
	out << imagename << " " << mean[0] << " " << mean[1] << " " << mean[2] << " "
		<< variance[0] << " " << variance[1] << " " << variance[2] << " "
		<< absoluteDeviation[0] << " " << absoluteDeviation[1] << " " << absoluteDeviation[2] << " "
		<< standardDeviation[0] << " " << standardDeviation[1] << " " << standardDeviation[2] << " "
		<< skewness[0] << " " << skewness[1] << " " << skewness[2] << " "
		<< kurtosis[0] << " " << kurtosis[1] << " " << kurtosis[2] << " "
		<< uniform_0d[0] << " "<< uniform_0d[1] << " " << uniform_0d[2] << " "
		<< uniform_45d[0] << " "<< uniform_45d[1] << " " << uniform_45d[2] << " "
		<< uniform_90d[0] << " "<< uniform_90d[1] << " " << uniform_90d[2] << " "
		<< contrast_0d[0] << " "<< contrast_0d[1] << " " << contrast_0d[2] << " "
		<< contrast_45d[0] << " "<< contrast_45d[1] << " " << contrast_45d[2] << " "
		<< contrast_90d[0] << " "<< contrast_90d[1] << " " << contrast_90d[2] << " "
		<< correlation_0d[0] << " "<< correlation_0d[1] << " " << correlation_0d[2] << " "
		<< correlation_45d[0] << " "<< correlation_45d[1] << " " << correlation_45d[2] << " "
		<< correlation_90d[0] << " "<< correlation_90d[1] << " " << correlation_90d[2] << " "
		<< sumSquares_0d[0] << " "<< sumSquares_0d[1] << " " << sumSquares_0d[2] << " "
		<< sumSquares_45d[0] << " "<< sumSquares_45d[1] << " " << sumSquares_45d[2] << " "
		<< sumSquares_90d[0] << " "<< sumSquares_90d[1] << " " << sumSquares_90d[2] << " "
		<< invDifMoments_0d[0] << " "<< invDifMoments_0d[1] << " " << invDifMoments_0d[2] << " "
		<< invDifMoments_45d[0] << " "<< invDifMoments_45d[1] << " " << invDifMoments_45d[2] << " "
		<< invDifMoments_90d[0] << " "<< invDifMoments_90d[1] << " " << invDifMoments_90d[2] << " "
		//<< sumAvg_0d[0] << " "<< sumAvg_0d[1] << " " << sumAvg_0d[2] << " "
		//<< sumAvg_45d[0] << " "<< sumAvg_45d[1] << " " << sumAvg_45d[2] << " "
		//<< sumAvg_90d[0] << " "<< sumAvg_90d[1] << " " << sumAvg_90d[2] << " "
		<< entropy_0d[0] << " "<< entropy_0d[1] << " " << entropy_0d[2] << " "
		<< entropy_45d[0] << " "<< entropy_45d[1] << " " << entropy_45d[2] << " "
		<< entropy_90d[0] << " "<< entropy_90d[1] << " " << entropy_90d[2] << " "
		<< nEdgeCount << " "
		<< edge_distribution << " "
		<< histogram_distribution_normalized[0] << " "
		<< histogram_distribution_normalized[1] << " "
		<< histogram_distribution_normalized[2] << " "
		<< endl;
}

// Divide by zero exception is destined to happen.
void ImagePro::NormalizeData(void)
{
	for(int k = 0 ; k < 3 ; k++)
	{
		// normalize statistics data
		mean[k] -= meanMin[k];
		mean[k] /= (meanMax[k] - meanMin[k]);
		variance[k] -= varianceMin[k];
		variance[k] /= (varianceMax[k] - varianceMin[k]);
		absoluteDeviation[k] -= absoluteDeviationMin[k];
		absoluteDeviation[k] /= (absoluteDeviationMax[k] - absoluteDeviationMin[k]);
		standardDeviation[k] -= standardDeviationMin[k];
		standardDeviation[k] /= (standardDeviationMax[k] - standardDeviationMin[k]);
		skewness[k] -= skewnessMin[k];
		skewness[k] /= (skewnessMax[k] - skewnessMin[k]);
		kurtosis[k] -= kurtosisMin[k];
		kurtosis[k] /= (kurtosisMax[k] - kurtosisMin[k]);

		// normalize co-occurrence data
		// 0 degree data normalized first
		uniform_0d[k] -= uniform_0dMin[k];
		uniform_0d[k] /= (uniform_0dMax[k] - uniform_0dMin[k]);
		correlation_0d[k] -= correlation_0dMin[k];
		correlation_0d[k] /= (correlation_0dMax[k] - correlation_0dMin[k]);
		sumSquares_0d[k] -= sumSquares_0dMin[k];
		sumSquares_0d[k] /= sumSquares_0dMax[k] - sumSquares_0dMin[k];
		invDifMoments_0d[k] -= invDifMoments_0dMin[k];
		invDifMoments_0d[k] /= invDifMoments_0dMax[k] - invDifMoments_0dMin[k];
		//sumAvg_0d[k] -= sumAvg_0dMin[k];
		//sumAvg_0d[k] /= sumAvg_0dMax[k] - sumAvg_0dMin[k];
		entropy_0d[k] -= entropy_0dMin[k];
		entropy_0d[k] /= entropy_0dMax[k] - entropy_0dMin[k];
		contrast_0d[k] -= contrast_0dMin[k];
		contrast_0d[k] /= contrast_0dMax[k] - contrast_0dMin[k];

		//45 degree data normalized next
		uniform_45d[k] -= uniform_45dMin[k];
		uniform_45d[k] /= (uniform_45dMax[k] - uniform_45dMin[k]);
		correlation_45d[k] -= correlation_45dMin[k];
		correlation_45d[k] /= (correlation_45dMax[k] - correlation_45dMin[k]);
		sumSquares_45d[k] -= sumSquares_45dMin[k];
		sumSquares_45d[k] /= sumSquares_45dMax[k] - sumSquares_45dMin[k];
		invDifMoments_45d[k] -= invDifMoments_45dMin[k];
		invDifMoments_45d[k] /= invDifMoments_45dMax[k] - invDifMoments_45dMin[k];
		//sumAvg_45d[k] -= sumAvg_45dMin[k];
		//sumAvg_45d[k] /= sumAvg_45dMax[k] - sumAvg_45dMin[k];
		entropy_45d[k] -= entropy_45dMin[k];
		entropy_45d[k] /= entropy_45dMax[k] - entropy_45dMin[k];
		contrast_45d[k] -= contrast_45dMin[k];
		contrast_45d[k] /= contrast_45dMax[k] - contrast_45dMin[k];

		// 90 degree data normalized last
		uniform_90d[k] -= uniform_90dMin[k];
		uniform_90d[k] /= (uniform_90dMax[k] - uniform_90dMin[k]);
		correlation_90d[k] -= correlation_90dMin[k];
		correlation_90d[k] /= (correlation_90dMax[k] - correlation_90dMin[k]);
		sumSquares_90d[k] -= sumSquares_90dMin[k];
		sumSquares_90d[k] /= sumSquares_90dMax[k] - sumSquares_90dMin[k];
		invDifMoments_90d[k] -= invDifMoments_90dMin[k];
		invDifMoments_90d[k] /= invDifMoments_90dMax[k] - invDifMoments_90dMin[k];
		//sumAvg_90d[k] -= sumAvg_90dMin[k];
		//sumAvg_90d[k] /= sumAvg_90dMax[k] - sumAvg_90dMin[k];
		entropy_90d[k] -= entropy_90dMin[k];
		entropy_90d[k] /= entropy_90dMax[k] - entropy_90dMin[k];
		contrast_90d[k] -= contrast_90dMin[k];
		contrast_90d[k] /= contrast_90dMax[k] - contrast_90dMin[k];
		histogram_distribution[k] -= histogram_distributionMin[k];
		histogram_distribution_normalized[k] = histogram_distribution[k] / double(histogram_distributionMax[k] - histogram_distributionMin[k]);
	}
	nEdgeCount -= nEdgeMin;
	nEdgeCount /= nEdgeMax - nEdgeMin;
	edge_distribution -= double(edge_distributionMin);
	edge_distribution /= double(edge_distributionMax - edge_distributionMin);
}


/*
	float meanMax[3], varianceMax[3], absoluteDeviationMax[3], standardDeviationMax[3], skewnessMax[3], kurtosisMax[3];
	float meanMin[3], varianceMin[3], absoluteDeviationMin[3], standardDeviationMin[3], skewnessMin[3], kurtosisMin[3];
	// Co-occurrence Features
	//  0 degree occurrence data, 45 degree occurrence data, 90 degree occurrence data, 
	float uniform_0d[3],  correlation_0d[3],  sumSquares_0d[3],  invDifMoments_0d[3],  sumAvg_0d[3],  entropy_0d[3];
	float uniform_45d[3], correlation_45d[3], sumSquares_45d[3], invDifMoments_45d[3], sumAvg_45d[3], entropy_45d[3];
	float uniform_90d[3], correlation_90d[3], sumSquares_90d[3], invDifMoments_90d[3], sumAvg_90d[3], entropy_90d[3];
	double contrast_0d[3], contrast_45d[3], contrast_90d[3];

	float uniform_0dMax[3], correlation_0dMax[3], sumSquares_0dMax[3], invDifMoments_0dMax[3], sumAvg_0dMax[3], entropy_0dMax[3];
	float uniform_45dMax[3], correlation_45dMax[3], sumSquares_45dMax[3], invDifMoments_45dMax[3], sumAvg_45dMax[3], entropy_45dMax[3];
	float uniform_90dMax[3], correlation_90dMax[3], sumSquares_90dMax[3], invDifMoments_90dMax[3], sumAvg_90dMax[3], entropy_90dMax[3];
	double contrast_0dMax[3], contrast_45dMax[3], contrast_90dMax[3];

	float uniform_0dMin[3], correlation_0dMin[3], sumSquares_0dMin[3], invDifMoments_0dMin[3], sumAvg_0dMin[3], entropy_0dMin[3];
	float uniform_45dMin[3], correlation_45dMin[3], sumSquares_45dMin[3], invDifMoments_45dMin[3], sumAvg_45dMin[3], entropy_45dMin[3];
	float uniform_90dMin[3], correlation_90dMin[3], sumSquares_90dMin[3], invDifMoments_90dMin[3], sumAvg_90dMin[3], entropy_90dMin[3];
	double contrast_0dMin[3], contrast_45dMin[3], contrast_90dMin[3];
	*/

void InitializeGlobalData(void)
{

	for(int k = 0 ; k < 3 ; k++)
	{
		meanMax[k] = -100000000.0;
		varianceMax[k] = -100000000.0;
		absoluteDeviationMax[k] = -100000000.0;
		standardDeviationMax[k] = -100000000.0;
		skewnessMax[k] = -100000000.0;
		kurtosisMax[k] -100000000.0;

		meanMin[k] = 100000000.0;
		varianceMin[k] = 100000000.0;
		absoluteDeviationMin[k] = 100000000.0;
		standardDeviationMin[k] = 100000000.0;
		skewnessMin[k] = 100000000.0;
		kurtosisMin[k] = 100000000.0;

		uniform_0dMax[k] = -100000000.0;
		correlation_0dMax[k] = -100000000.0;
		sumSquares_0dMax[k] = -100000000.0;
		invDifMoments_0dMax[k] = -100000000.0;
		sumAvg_0dMax[k] = -100000000.0;
		entropy_0dMax[k] = -100000000.0;
		uniform_45dMax[k] = -100000000.0;
		correlation_45dMax[k] = -100000000.0;
		sumSquares_45dMax[k] = -100000000.0;
		invDifMoments_45dMax[k] = -100000000.0;
		sumAvg_45dMax[k] = -100000000.0;
		entropy_45dMax[k] = -100000000.0;
		uniform_90dMax[k] = -100000000.0;
		correlation_90dMax[k] = -100000000.0;
		sumSquares_90dMax[k] = -100000000.0;
		invDifMoments_90dMax[k] = -100000000.0;
		sumAvg_90dMax[k] = -100000000.0;
		entropy_90dMax[k] = -100000000.0;
		contrast_0dMax[k] = -100000000.0;
		contrast_45dMax[k] = -100000000.0;
		contrast_90dMax[k] = -100000000.0;

		uniform_0dMin[k] = 100000000.0;
		correlation_0dMin[k] = 100000000.0;
		sumSquares_0dMin[k] = 100000000.0;
		invDifMoments_0dMin[k] = 100000000.0;
		sumAvg_0dMin[k] = 100000000.0;
		entropy_0dMin[k] = 100000000.0;
		uniform_45dMin[k] = 100000000.0;
		correlation_45dMin[k] = 100000000.0;
		sumSquares_45dMin[k] = 100000000.0;
		invDifMoments_45dMin[k] = 100000000.0;
		sumAvg_45dMin[k] = 100000000.0;
		entropy_45dMin[k] = 100000000.0;
		uniform_90dMin[k] = 100000000.0;
		correlation_90dMin[k] = 100000000.0;
		sumSquares_90dMin[k] = 100000000.0;
		invDifMoments_90dMin[k] = 100000000.0;
		sumAvg_90dMin[k] = 100000000.0;
		entropy_90dMin[k] = 100000000.0;
		contrast_0dMin[k] = 100000000.0;
		contrast_45dMin[k] = 100000000.0;
		contrast_90dMin[k] = 100000000.0;
		histogram_distributionMin[k] = LLONG_MAX;
		histogram_distributionMax[k] = -1;
	}
	nEdgeMin = 100000000.0;
	nEdgeMax = -100000000.0;
	edge_distributionMin = DBL_MAX;
	edge_distributionMax = DBL_MIN;
}

void ImageFeature::ReadDataFromFile(ifstream &in, string input_filename, ofstream &out, string output_filename, int nLines)
{
	in.open(input_filename.c_str());
	if(in.is_open() == false)
	{
		cout << " Hi. The CSV file does not exist." << endl;
		exit(-3);
	}

	out.open(output_filename.c_str());

	for(int i = 0 ; i < nLines ; i++)
	{
		// Open File and read data and then normalize and then save it in outCSVNormalized
		in >> this ->imagename;
		for(int k = 0 ; k < 3 ; k++)
			in >> mean[k];
		for(int k = 0 ; k < 3 ; k++)
			in >> variance[k];
		for(int k = 0 ; k < 3 ; k++)
			in >> absoluteDeviation[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  standardDeviation[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  skewness[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  kurtosis[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_90d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_90d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_90d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_90d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_90d[k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_0d[k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_45d[k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_90d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_0d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_45d[k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_90d[k];
		in >> nEdgeCount;
		in >> edge_distribution;
		for(int k = 0 ; k < 3 ; k++)
			in >> histogram_distribution[k];

		NormalizeData();
		DisplayDataInFile2(out);
	}
}


void ImagePro::CountCannyEdges(void)
{
	IplImage *canny, *grayImage;
	grayImage = cvCreateImage(cvGetSize(m_img), 8, 1);
	canny = cvCreateImage(cvGetSize(m_img), 8, 1);
	cvCvtColor(m_img, grayImage, CV_BGR2GRAY);
	cvCanny(grayImage, canny, 40, 130, 3);

	int width = m_img ->width;
	int height = m_img ->height;
	Scalar f ;
	for(int j = 0 ; j < height ; j++)
	{
		for(int i = 0 ; i < width ; i++)
		{
			f = cvGet2D(canny, j, i);
			if(f.val[0] > 128.0)
			{
				nEdgeCount ++;
			}
		}
	}
	if(nEdgeCount < nEdgeMin)
		nEdgeMin = nEdgeCount;
	if(nEdgeCount > nEdgeMax)
		nEdgeMax = nEdgeCount;

	int blockHeight = float(height)/10.0f;
	int blockWidth = float(width)/10.0f;
	for(int j = 0 ; j < 10 ; j++)
	{
		for(int i = 0 ; i < 10 ; i++)
		{
			for(int h = 0 ; h < blockHeight ; h++)
			{
				for(int w = 0 ; w < blockWidth ; w++)
				{
					f = cvGet2D(canny, j * 10 + h, i * 10 + w);
					if(f.val[0] > 128.0)
					{
						// float nBlockEdgeCount[10][10];
						nBlockEdgeCount[j][i] ++;
					}
				}
			}
		}
	}
	//Now we have edge counts for each block so we compute distribution of edge descriptor(kind like a shape descriptor).
	edge_distribution = 0.0;
	for(int j = 0 ; j < 10 ; j++)
	{
		for(int i = 0 ; i < 10 ; i++)
		{	// double edge_distribution;
			edge_distribution += (nBlockEdgeCount[j][i] * (i + 1) * (j + 1) * (i + 1) * (j + 1) / 1000.0);
		}
	}
	if(edge_distribution < edge_distributionMin)
		edge_distributionMin = edge_distribution;
	if(edge_distribution > edge_distributionMax)
		edge_distributionMax = edge_distribution;
}



