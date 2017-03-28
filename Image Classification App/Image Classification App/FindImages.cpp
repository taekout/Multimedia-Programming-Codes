#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

using namespace std;
using namespace cv;

#define	CSV_NORMALIZED_FILE		"normalized_features.csv"
#define	FILE_NUMBERS			113
#define	NUMBER_OF_FILES_TO_PICK	12

float geometric_difference[FILE_NUMBERS];
float arithmetic_difference[FILE_NUMBERS];
int geometric_image_order[FILE_NUMBERS];
int arithmetic_image_order[FILE_NUMBERS];
int imageNo = -1;

class FeatureAnalyzer
{
public:
	// Data
	// Filename Data
	char imagename[FILE_NUMBERS][40];
	// Statistics
	float mean[FILE_NUMBERS][3], variance[FILE_NUMBERS][3], absoluteDeviation[FILE_NUMBERS][3], standardDeviation[FILE_NUMBERS][3]
			, skewness[FILE_NUMBERS][3], kurtosis[FILE_NUMBERS][3];
	// Co-occurrence Features
	//  0 degree occurrence data, 45 degree occurrence data, 90 degree occurrence data, 
	float uniform_0d[FILE_NUMBERS][3],  correlation_0d[FILE_NUMBERS][3],  sumSquares_0d[FILE_NUMBERS][3],
				invDifMoments_0d[FILE_NUMBERS][3],  /*sumAvg_0d[FILE_NUMBERS][3],*/  entropy_0d[FILE_NUMBERS][3];
	float uniform_45d[FILE_NUMBERS][3], correlation_45d[FILE_NUMBERS][3], sumSquares_45d[FILE_NUMBERS][3],
				invDifMoments_45d[FILE_NUMBERS][3], /*sumAvg_45d[FILE_NUMBERS][3],*/ entropy_45d[FILE_NUMBERS][3];
	float uniform_90d[FILE_NUMBERS][3], correlation_90d[FILE_NUMBERS][3], sumSquares_90d[FILE_NUMBERS][3],
				invDifMoments_90d[FILE_NUMBERS][3], /*sumAvg_90d[FILE_NUMBERS][3],*/ entropy_90d[FILE_NUMBERS][3];
	double contrast_0d[FILE_NUMBERS][3], contrast_45d[FILE_NUMBERS][3], contrast_90d[FILE_NUMBERS][3];
	float edgeCount[FILE_NUMBERS];
	double edge_distribution[FILE_NUMBERS];

	double arithmeticMean[FILE_NUMBERS][3], geometricMean[FILE_NUMBERS][3];

	// Histogram data
	CvHistogram imageHisto[FILE_NUMBERS];
	double histoDistribution[FILE_NUMBERS][3];
	double histogramMatchingDifference[FILE_NUMBERS][3];

	// functions
	FeatureAnalyzer()
	{
		for(int i = 0 ; i < FILE_NUMBERS ; i++)
		{
			geometric_image_order[i] = i;
			arithmetic_image_order[i] = i;
			edgeCount[FILE_NUMBERS] = -1.0f;
		}
	}
	~FeatureAnalyzer(){}

	void GetGeometricMean(int nWantedImageNo);
	void GetArithmeticMean(void);

	void GetGeometricMean_UsingStatistics(void);
	void GetArithmeticMean_UsingStatistics(void);

	void FileRead(ifstream &in);
	int FindImage(int imageNo);
	int MontageImages(int numberOfImages, vector<string> &filenames, string resultOutputName);

	// Make histogram
	double * CompareHistogram(IplImage* BufSrc, IplImage* BufRef, int method);
	void GetAllHistogramMean(void);
};

int main(void)
{
	ifstream in;
	in.open(CSV_NORMALIZED_FILE);
	if(in.is_open() == false)
	{
		cout << "cannot open file " << endl;
		exit(-1);
	}
	int imageNo;
	cout << endl << "Which image number do you want? I will get similar images for you! : " ;
	cin >> imageNo;
	if(imageNo < 0 || imageNo > FILE_NUMBERS - 1)
	{
		cout << "Image Number Range : [ 0, " << FILE_NUMBERS << " ] " << endl;
		exit(-5);
	}
	::imageNo = imageNo;
	FeatureAnalyzer imageFeatures;
	imageFeatures.FileRead(in);
	imageFeatures.GetAllHistogramMean();
	imageFeatures.GetArithmeticMean();
	imageFeatures.GetGeometricMean(imageNo);
	//imageFeatures.GetArithmeticMean_UsingStatistics();
	//imageFeatures.GetGeometricMean_UsingStatistics();

	

	imageFeatures.FindImage(imageNo);
	string resultOutputName = "resultImage_withGeometricMean.jpg";
//	char filenamesToCombine[NUMBER_OF_FILES_TO_PICK][40];
	vector<string> filenamesToCombine;
	string basis = "image_";
	char numberStr[10] = {};
	for(int i = 0 ; i < NUMBER_OF_FILES_TO_PICK + 1 ; i++)
	{
		int nPicked = geometric_image_order[i];
		itoa(nPicked, numberStr, 10);
		string currentFile;
		currentFile = basis;
		currentFile += numberStr;
		currentFile += ".jpg";
		filenamesToCombine.push_back(currentFile);
	}
	imageFeatures.MontageImages(NUMBER_OF_FILES_TO_PICK, filenamesToCombine, resultOutputName);

	filenamesToCombine.clear();
	resultOutputName = "resultImage_withArithmeticMean.jpg";
	for(int i = 0 ; i < NUMBER_OF_FILES_TO_PICK + 1 ; i++)
	{
		int nPicked = arithmetic_image_order[i];
		itoa(nPicked, numberStr, 10);
		string currentFile;
		currentFile = basis;
		currentFile += numberStr;
		currentFile += ".jpg";
		filenamesToCombine.push_back(currentFile);
	}

	imageFeatures.MontageImages(NUMBER_OF_FILES_TO_PICK, filenamesToCombine, resultOutputName);

	return	0;
}



void FeatureAnalyzer::FileRead(ifstream & in)
{
	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{	
		// Open File and read data and then normalize and then save it in outCSVNormalized
		in >> this ->imagename[i];
		for(int k = 0 ; k < 3 ; k++)
			in >> mean[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >> variance[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >> absoluteDeviation[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  standardDeviation[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  skewness[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  kurtosis[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  uniform_90d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  contrast_90d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  correlation_90d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  sumSquares_90d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  invDifMoments_90d[i][k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_0d[i][k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_45d[i][k];
		//for(int k = 0 ; k < 3 ; k++)
		//	in >>  sumAvg_90d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_0d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_45d[i][k];
		for(int k = 0 ; k < 3 ; k++)
			in >>  entropy_90d[i][k];
		in >> edgeCount[i];
		in >> edge_distribution[i];
		for(int k = 0 ; k < 3 ; k++)
			in >> histoDistribution[i][k];
//		NormalizeData();
//		DisplayDataInFile(out);
	}
}

void FeatureAnalyzer::GetGeometricMean(int nImageNo)
{
	double mean_tmp, variance_tmp, absoluteDeviation_tmp, standardDeviation_tmp, skewness_tmp, kurtosis_tmp;
	double uniform_0d_tmp, uniform_45d_tmp, uniform_90d_tmp, correlation_0d_tmp, correlation_45d_tmp, correlation_90d_tmp;
	double sumSquares_0d_tmp, sumSquares_45d_tmp, sumSquares_90d_tmp;
	double invDifMoments_0d_tmp, invDifMoments_45d_tmp, invDifMoments_90d_tmp;
	//double sumAvg_0d_tmp, sumAvg_45d_tmp, sumAvg_90d_tmp;
	double entropy_0d_tmp, entropy_45d_tmp, entropy_90d_tmp;
	double edgeCount_tmp;
	double edge_distribution_tmp;


	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		for(int k = 0 ; k < 3 ; k++)
		{
			mean_tmp = (mean[i][k] - mean[nImageNo][k]) * (mean[i][k] - mean[nImageNo][k]);
			variance_tmp = (variance[i][k] - variance[nImageNo][k]) * (variance[i][k] - variance[nImageNo][k]);
			absoluteDeviation_tmp = (absoluteDeviation[i][k] - absoluteDeviation[nImageNo][k]) * (absoluteDeviation[i][k] - absoluteDeviation[nImageNo][k]);
			standardDeviation_tmp = (standardDeviation[i][k] - standardDeviation[nImageNo][k]) * (standardDeviation[i][k] - standardDeviation[nImageNo][k]);
			skewness_tmp = (skewness[i][k]- skewness[nImageNo][k]) * (skewness[i][k] - skewness[nImageNo][k]);
			kurtosis_tmp = (kurtosis[i][k] - kurtosis[nImageNo][k]) * (kurtosis[i][k] - kurtosis[nImageNo][k]);
			uniform_0d_tmp = (uniform_0d[i][k] - uniform_0d[nImageNo][k]) * (uniform_0d[i][k] - uniform_0d[nImageNo][k]);
			uniform_45d_tmp = (uniform_45d[i][k] - uniform_45d[nImageNo][k]) * (uniform_45d[i][k] - uniform_45d[nImageNo][k]);
			uniform_90d_tmp = (uniform_90d[i][k] - uniform_90d[nImageNo][k]) * (uniform_90d[i][k] - uniform_90d[nImageNo][k]);
			correlation_0d_tmp = (correlation_0d[i][k] - correlation_0d[nImageNo][k]) * (correlation_0d[i][k] - correlation_0d[nImageNo][k]);
			correlation_45d_tmp = (correlation_45d[i][k] - correlation_45d[nImageNo][k]) * (correlation_45d[i][k] - correlation_45d[nImageNo][k]) ;
			correlation_90d_tmp = (correlation_90d[i][k] - correlation_90d[nImageNo][k]) *  (correlation_90d[i][k] - correlation_90d[nImageNo][k]);
			sumSquares_0d_tmp = (sumSquares_0d[i][k] - sumSquares_0d[nImageNo][k]) * (sumSquares_0d[i][k] - sumSquares_0d[nImageNo][k]);
			sumSquares_45d_tmp = (sumSquares_45d[i][k] - sumSquares_45d[nImageNo][k]) * (sumSquares_45d[i][k] - sumSquares_45d[nImageNo][k]);
			sumSquares_90d_tmp = (sumSquares_90d[i][k] - sumSquares_90d[nImageNo][k]) * (sumSquares_90d[i][k] - sumSquares_90d[nImageNo][k]);
			invDifMoments_0d_tmp = (invDifMoments_0d[i][k] - invDifMoments_0d[nImageNo][k]) * (invDifMoments_0d[i][k] - invDifMoments_0d[nImageNo][k]);
			invDifMoments_45d_tmp = (invDifMoments_45d[i][k] - invDifMoments_45d[nImageNo][k]) * (invDifMoments_45d[i][k] - invDifMoments_45d[nImageNo][k]);
			invDifMoments_90d_tmp = (invDifMoments_90d[i][k] - invDifMoments_90d[nImageNo][k]) * (invDifMoments_90d[i][k] - invDifMoments_90d[nImageNo][k]);
			//sumAvg_0d_tmp = sumAvg_0d[i][k] * sumAvg_0d[i][k];
			//sumAvg_45d_tmp = sumAvg_45d[i][k] * sumAvg_45d[i][k];
			//sumAvg_90d_tmp = sumAvg_90d[i][k] * sumAvg_90d[i][k];
			entropy_0d_tmp = (entropy_0d[i][k] - entropy_0d[nImageNo][k]) * (entropy_0d[i][k] - entropy_0d[nImageNo][k]);
			entropy_45d_tmp = (entropy_45d[i][k] - entropy_45d[nImageNo][k]) * (entropy_45d[i][k] - entropy_45d[nImageNo][k]);
			entropy_90d_tmp = (entropy_90d[i][k] - entropy_90d[nImageNo][k]) * (entropy_90d[i][k] - entropy_90d[nImageNo][k]);
			edgeCount_tmp = (edgeCount[i] - edgeCount[nImageNo]) * (edgeCount[i] - edgeCount[nImageNo]);
			edge_distribution_tmp = (edge_distribution[i] - edge_distribution[nImageNo]) * (edge_distribution[i] - edge_distribution[nImageNo]);
			//histogram things
			double histoDiffInTotal = 0.0;
			for(int e = 0 ; e < 4 ; e++)
				histoDiffInTotal += (histoDistribution[i][e] - histoDistribution[nImageNo][e]) * (histoDistribution[i][e] - histoDistribution[nImageNo][e]);
			//histoDiffInTotal += (histoDistribution[i][0] * histoDistribution[i][0];
			//histoDiffInTotal += histoDistribution[i][1] * histoDistribution[i][1];
			//histoDiffInTotal += histoDistribution[i][2] * histoDistribution[i][2];
			//histoDiffInTotal += histoDistribution[i][3] * histoDistribution[i][3];

			double total = (mean_tmp + variance_tmp + absoluteDeviation_tmp + standardDeviation_tmp + skewness_tmp + kurtosis_tmp)+
				uniform_0d_tmp + uniform_45d_tmp + uniform_90d_tmp +
				correlation_0d_tmp + correlation_45d_tmp + correlation_90d_tmp +
				sumSquares_0d_tmp + sumSquares_45d_tmp + sumSquares_90d_tmp +
				invDifMoments_0d_tmp + invDifMoments_45d_tmp + invDifMoments_90d_tmp +
				//sumAvg_0d_tmp + sumAvg_45d_tmp + sumAvg_90d_tmp +
				entropy_0d_tmp + entropy_45d_tmp + entropy_90d_tmp +
				edgeCount_tmp + edge_distribution_tmp + histoDiffInTotal;

			geometricMean[i][k] = sqrt(total);
		}
	}
}

void FeatureAnalyzer::GetArithmeticMean(void)
{
	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		for(int k = 0 ; k < 3 ; k++)
		{
			arithmeticMean[i][k] = ((mean[i][k] + variance[i][k] + absoluteDeviation[i][k] + standardDeviation[i][k] + skewness[i][k] + kurtosis[i][k]) +
				uniform_0d[i][k] + uniform_45d[i][k] + uniform_90d[i][k] + correlation_0d[i][k] + correlation_45d[i][k] + correlation_90d[i][k] +
				sumSquares_0d[i][k] + sumSquares_45d[i][k] + sumSquares_90d[i][k] + invDifMoments_0d[i][k] + invDifMoments_45d[i][k] + invDifMoments_90d[i][k] +
				/*sumAvg_0d[i][k] + sumAvg_45d[i][k] + sumAvg_90d[i][k] + */entropy_0d[i][k] + entropy_45d[i][k] + entropy_90d[i][k] +
				edgeCount[i] + edge_distribution[i] + histoDistribution[i][k]) / (24.0);
		}
	}
}

int FeatureAnalyzer::FindImage(int imageNo)
{
	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		double geometricDifference[3];
		double arithmeticDifference[3];
		for(int k = 0 ; k < 3 ; k++)
		{
			geometricDifference[k] = fabs(geometricMean[i][k]); // geometric differences are already calculated in GetGeometricMean func.
			arithmeticDifference[k]  = fabs(arithmeticMean[i][k] - arithmeticMean[imageNo][k]);
		}
		geometric_difference[i] = (geometricDifference[0] + geometricDifference[1] + geometricDifference[2]) / 3.0;
		arithmetic_difference[i] = (arithmeticDifference[0] + arithmeticDifference[1] + arithmeticDifference[2]) / 3.0;
		/////////////////////////////////////
		//////////// HERE ADD dfSCORE to GEOMEAN and ARITHMEAN!!!
		double histoMatchingDiffForOneImage = (histogramMatchingDifference[i][0] + histogramMatchingDifference[i][1] +
												histogramMatchingDifference[i][2]) / 3.0;
		geometric_difference[i] += histoMatchingDiffForOneImage / 10.0;
		arithmetic_difference[i] += histoMatchingDiffForOneImage / 2.0;
	}

	// Bubble sort method. just to order in similarity.
	float tmp; int itmp;
	for(int x = 0; x < FILE_NUMBERS ; x++)
	{
		for(int y = 0; y < FILE_NUMBERS-1 ; y++)
		{
			if(geometric_difference[y] > geometric_difference[y+1])
			{
				tmp = geometric_difference[y+1];
				geometric_difference[y+1] = geometric_difference[y];
				geometric_difference[y] = tmp;
				// Swap image order for arithmetic
				itmp = geometric_image_order[y+1];
				geometric_image_order[y+1] = geometric_image_order[y];
				geometric_image_order[y] = itmp;
			}
		}
	}
	for(int x = 0; x < FILE_NUMBERS ; x++)
	{
		for(int y = 0; y < FILE_NUMBERS-1 ; y++)
		{
			if(arithmetic_difference[y] > arithmetic_difference[y+1])
			{	// Swap
				tmp = arithmetic_difference[y+1];
				arithmetic_difference[y+1] = arithmetic_difference[y];
				arithmetic_difference[y] = tmp;
				// Swap image order for arithmetic
				itmp = arithmetic_image_order[y+1];
				arithmetic_image_order[y+1] = arithmetic_image_order[y];
				arithmetic_image_order[y] = itmp;
			}
		}
	}
	return	NULL;
}

int FeatureAnalyzer::MontageImages(int numberOfImages, vector<string> & filenames, string resultOutputName)
{
	string command = "montage ";
	for(int i = 0 ; i < numberOfImages ; i++)
	{
		command += filenames[i];
		command += " ";
	}
	command += resultOutputName;
	cout << command.c_str() << endl;

	return	system(command.c_str());
}


void FeatureAnalyzer::GetGeometricMean_UsingStatistics(void)
{
	double mean_tmp, variance_tmp, absoluteDeviation_tmp, standardDeviation_tmp, skewness_tmp, kurtosis_tmp;

	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		for(int k = 0 ; k < 3 ; k++)
		{
			mean_tmp = mean[i][k] * mean[i][k];
			variance_tmp = variance[i][k] * variance[i][k];
			absoluteDeviation_tmp = absoluteDeviation[i][k] * absoluteDeviation[i][k];
			standardDeviation_tmp = standardDeviation[i][k] * standardDeviation[i][k];
			skewness_tmp = skewness[i][k] * skewness[i][k];
			kurtosis_tmp = kurtosis[i][k] * kurtosis[i][k];

			double total = mean_tmp + variance_tmp + absoluteDeviation_tmp + standardDeviation_tmp + skewness_tmp + kurtosis_tmp;

			geometricMean[i][k] = sqrt(total);
		}
	}
}

void FeatureAnalyzer::GetArithmeticMean_UsingStatistics(void)
{
	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		for(int k = 0 ; k < 3 ; k++)
		{
			arithmeticMean[i][k] = (mean[i][k] + variance[i][k] + absoluteDeviation[i][k] + standardDeviation[i][k] + skewness[i][k] + kurtosis[i][k]) / 6.0;
		}
	}
}

double * FeatureAnalyzer::CompareHistogram(IplImage* BufSrc, IplImage* BufRef, int method)
{
	double *dfScore = new double[3];


	CvHistogram *histoSrc , *histoRef;

	int nHistoSize = 256;
	float range[]={0,256};
	float* ranges[] = { range };
	IplImage *channel = cvCreateImage(cvGetSize(BufSrc), 8, 1);
	//CvHistogram *histSrc = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, hist_range);
	CvHistogram *histSrc_red;
	CvHistogram *histSrc_green;
	CvHistogram *histSrc_blue;
	// I will here get the histogram of the color source image.
	histSrc_red = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	histSrc_green = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	histSrc_blue = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	cvSetImageCOI(BufSrc,3);
	cvCopy(BufSrc, channel);
	cvResetImageROI(BufSrc);
	cvCalcHist(&channel, histSrc_red, 0, NULL);
	cvSetImageCOI(BufSrc,2);
	cvCopy(BufSrc,channel);
	cvResetImageROI(BufSrc);
	cvCalcHist( &channel, histSrc_green, 0, NULL );
	cvSetImageCOI(BufSrc,1);
	cvCopy(BufSrc,channel);
	cvResetImageROI(BufSrc);
	cvCalcHist( &channel, histSrc_blue, 0, NULL );
	cvReleaseImage(&channel);

	channel = cvCreateImage(cvGetSize(BufRef), 8, 1);
	//CvHistogram *histSrc = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, hist_range);
	CvHistogram *histRef_red;
	CvHistogram *histRef_green;
	CvHistogram *histRef_blue;
	// I will here get the histogram of the color reference image.
	histRef_red = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	histRef_green = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	histRef_blue = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, ranges, 1);
	cvSetImageCOI(BufRef,3);
	cvCopy(BufRef, channel);
	cvResetImageROI(BufRef);
	cvCalcHist(&channel, histRef_red, 0, NULL);
	cvSetImageCOI(BufRef,2);
	cvCopy(BufRef,channel);
	cvResetImageROI(BufRef);
	cvCalcHist( &channel, histRef_green, 0, NULL );
	cvSetImageCOI(BufRef,1);
	cvCopy(BufRef,channel);
	cvResetImageROI(BufRef);
	cvCalcHist( &channel, histRef_blue, 0, NULL );
	cvReleaseImage(&channel);

//	CvHistogram *histRef = cvCreateHist(1, &nHistoSize, CV_HIST_ARRAY, hist_range);
//	cvCalcHist(&BufSrc, histSrc);//, 0, NULL );
//	cvCalcHist(&BufRef, histRef, 0, NULL );  cvNormalizeHist( histSrc, 100);
	cvNormalizeHist( histSrc_red, 100);
	cvNormalizeHist( histSrc_green, 100);
	cvNormalizeHist( histSrc_blue, 100);
	cvNormalizeHist( histRef_red, 100);
	cvNormalizeHist( histRef_green, 100);
	cvNormalizeHist( histRef_blue, 100);
	// Score = 0.0 ~ 100.0
	switch(method)
	{
	case 1 :	// High score = better match  --> Perfect Match == 1 Mismatch == -1 ==> After normalizing, [1,0]
		dfScore[0] = cvCompareHist( histSrc_red, histRef_red, CV_COMP_CORREL);
		dfScore[1] = cvCompareHist( histSrc_green, histRef_green, CV_COMP_CORREL);
		dfScore[2] = cvCompareHist( histSrc_blue, histRef_blue, CV_COMP_CORREL);
		for(int i = 0 ; i < 3 ; i++)
		{
			dfScore[i] = (1 - dfScore[i]) / 2.0; // Now 0 == perfect match, 1 == mismatch.
		}
		break;
	case 2 :	// low score == a better match --> Perfect Match == 0 Mismatch value == unbounded  ==> maybe i should not use this.
		dfScore[0] = cvCompareHist( histSrc_red, histRef_red, CV_COMP_CHISQR);
		dfScore[1] = cvCompareHist( histSrc_green, histRef_green, CV_COMP_CHISQR);
		dfScore[2] = cvCompareHist( histSrc_blue, histRef_blue, CV_COMP_CHISQR);
		break;
	case 3 :	// high score == a better match --> Perfect Match == 100 Mismatch == 0 After normalization, [1,0]
		dfScore[0] = cvCompareHist( histSrc_red, histRef_red, CV_COMP_INTERSECT);
		dfScore[1] = cvCompareHist( histSrc_green, histRef_green, CV_COMP_INTERSECT);
		dfScore[2] = cvCompareHist( histSrc_blue, histRef_blue, CV_COMP_INTERSECT);
		for(int i = 0 ; i < 3 ; i++)
		{
			dfScore[i] = 1 - dfScore[i]/100.0;	// Now Score = [0,1] Perfect Match == 0 mismatch == 1
		}
		break;
	case 4 :	// low score == a better match --> Perfect Match == 0 Mismatch == 1
		dfScore[0] = cvCompareHist( histSrc_red, histRef_red, CV_COMP_BHATTACHARYYA);
		dfScore[1] = cvCompareHist( histSrc_green, histRef_green, CV_COMP_BHATTACHARYYA);
		dfScore[2] = cvCompareHist( histSrc_blue, histRef_blue, CV_COMP_BHATTACHARYYA);
		break;
	default :
		cout << "method must be 0 - 4. " << endl;
		exit(-10);
	}

	return dfScore;
}

void FeatureAnalyzer::GetAllHistogramMean(void)
{
	/*
		1. For LOOP over all images
		2. Compute histogram matching using 3 methods(1, 3, 4)   BETWEEN the desired image and all the other images
		3. Save it in 'double histogram
	*/
	if(imageNo == -1)
	{
		cout << "Image has not been chosen yet. " << endl;
		exit(-11);
	}
	char numberStr[10];
	itoa(imageNo, numberStr, 10);
	string chosenFile = "image_";
	chosenFile += numberStr;
	chosenFile += ".jpg";
	IplImage *chosenImage = cvLoadImage(chosenFile.c_str());
	for(int i = 0 ; i < FILE_NUMBERS ; i++)
	{
		itoa(i, numberStr, 10);
		string currentFile;
		currentFile = "image_";
		currentFile += numberStr;
		currentFile += ".jpg";
		double *tmp;
		IplImage *curImage = cvLoadImage(currentFile.c_str());
		tmp = CompareHistogram(chosenImage, curImage, 1);
		histogramMatchingDifference[i][0] = (tmp[0] + tmp[1] + tmp[2]) / 3.0;
		tmp = CompareHistogram(chosenImage, curImage, 3);
		histogramMatchingDifference[i][1] = (tmp[0] + tmp[1] + tmp[2]) / 3.0;
		tmp = CompareHistogram(chosenImage, curImage, 4);
		histogramMatchingDifference[i][2] = (tmp[0] + tmp[1] + tmp[2]) / 3.0;
		cvReleaseImage(&curImage);
	}
}




