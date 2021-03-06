/*

	The reason I should not use Delaunay ? ==> The triangulation is not unique!
*/
#include <stdio.h>
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <fstream>

#include "MyTriangle.h"
#include "MatrixInverse.h"

using namespace std;

#define WINDOW1		1
#define	WINDOW2		2


class Point{
public:
	float x;
	float y;

public:
	Point(){x = -1; y = -1;}
	Point(float x, float y)
	{
		this ->x = x; this ->y = y;
	}
	Point &operator =(const Point & src)
	{
		x = src.x;
		y = src.y;
		return *this;
	}
	Point &operator =(const CvPoint &src)
	{
		x = src.x;
		y = src.y;
		return *this;
	}
	// ADD * operator!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Point & operator *(float t)
	{
		Point *pt = new Point(this ->x, this ->y);
		pt ->x = pt ->x * t;
		pt ->y = pt ->y * t;
		return *pt;
	}
	Point & operator+=(const Point &src)
	{
		x = x + src.x;
		y = y + src.y;
		return (*this);
	}
	Point & operator-=(const Point &src)
	{
		x = x - src.x;
		y = y - src.y;
		return	(*this);
	}
	const Point &operator-(const Point &src) const
	{
		return Point(*this) -= src;
	}
	const Point &operator +(const Point &src) const
	{
		return Point(*this) += src;
	}
	bool operator==(const Point &other) const {
		// Compare the values, and return a bool result.
		return (other.x == this ->x) &&(other.y == this ->y);
	}
	bool operator!=(const Point &other) const {
		return !(*this == other);
	}
	bool operator==(const CvPoint &other) const {
		// Compare the values, and return a bool result.
		return (other.x == this ->x) &&(other.y == this ->y);
	}
	bool operator!=(const CvPoint &other) const {
		return !(*this == other);
	}

	Point GetPoint(void)
	{
		Point ret(x, y);
		return	ret;
	}
	void NormalizeVector();
	static float GetLengthVector(Point p);
	static void CrossProduct(const Point p1, const Point p2, Point &p3);
	static float DotProduct(const Point p1, const Point p2)
	{
		return	p1.x * p2.x + p1.y * p2.y;
	}
};

class Edge
{
public:
	Point a, b;
	Edge(Point _a, Point _b)
	{
		a = _a;
		b = _b;
	}
	bool operator ==(const Edge &other) const {
		if((a == other.a) && (b == other.b) == true)
			return true;
		else if((a == other.b) && (b == other.a))
			return true;
		return false;
	}
};

typedef struct delaunayEdge
{
	int nIndexOrg;
	int nIndexDst;
	Point a;
	Point b;
} delaunayEdge;

typedef struct delaunayData
{
	int nEdges;
	delaunayEdge edges[500];
	delaunayData()
	{
		nEdges = 0;
		for(int i = 0 ; i < 500 ;i++)
		{
			edges[i].nIndexOrg = -1;
			edges[i].nIndexDst = -1;
			edges[i].a.x = -1;
			edges[i].a.y = -1;
			edges[i].b.x = -1;
			edges[i].b.y = -1;
		}
	}
} delaunayData;

typedef struct triangle
{
	bool foundTriangle;
	int indexA, indexB, indexC;
	triangle()
	{
		indexA = -1 ;
		indexB = -1 ;
		indexC = -1 ;
		foundTriangle = false;
	}
	bool operator ==(const triangle &other) const {
		if((indexA == other.indexA) && (indexB == other.indexB) && (indexC == other.indexC) == true)
			return true;
		return false;
	}
} triangle;

typedef struct triangles
{
	int nTriangles;
	triangle tri[500];
	triangles()
	{
		nTriangles = 0;
	}
	triangles operator +=(const triangle new_tri)
	{
		tri[nTriangles].foundTriangle = new_tri.foundTriangle;
		tri[nTriangles].indexA = new_tri.indexA;
		tri[nTriangles].indexB = new_tri.indexB;
		tri[nTriangles].indexC = new_tri.indexC;
		return (*this);
	}
} triangles;

IplImage *g_src;
IplImage *g_dst;
static int indexPoint1 = 0;
static int indexPoint2 = 0;
CvPoint pt1[500];
CvPoint pt2[500];
delaunayData g_delaunayData;
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

// Find the connecting point in the edge
triangles g_triangles;
triangles g_trianglesWithoutDuplicates;
int nTmpInitIndex = -1, nTmpIndex2 = -1, nTmpIndex3 = -1;
void FindEdge(delaunayData &array, int index, int initialIndex, int N);
void DetermineTriangles(delaunayData delData);
void Transform(IplImage *src, IplImage *dst, triangle tri);

void MatrixMult3X3(float resultM[][3], float M1[][3], float M2[][3])
{
	int i = 0, j = 0 , k = 0;
	for(i = 0 ; i < 2 ; i++)
	{
		for(j = 0 ; j < 3 ; j++)
		{
			resultM[i][j] = 0.0f;
			for(k = 0 ; k < 3 ; k++)
			{
				resultM[i][j] = M1[i][k] * M2[k][j];
			}
		}
	}
}

int main(int argc, char *argv[])
{
	IplImage *src = cvLoadImage("morph1.jpg");
	g_src = src;
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

	int key = 1;
/*
		for(int i = 0 ; i < indexPoint1 ; i++)
		{
			cvCircle(src, cvPoint(pt1[i].x, pt1[i].y), 3, CV_RGB(255, 0, 0), -1);
		}
		for(int i = 0 ; i < indexPoint2 ; i++)
		{
			cvCircle(dst, cvPoint(pt2[i].x, pt2[i].y), 3, CV_RGB(0, 0, 255), -1);
		}*/
	DelaunayLoop(src, pt1);
	// For image2(==dst), I use the index acquired from the first DelaunayLoop.


	DetermineTriangles(g_delaunayData);

/*
	for(int i = 0 ; i < g_trianglesWithoutDuplicates.nTriangles ; i++)
	{
		CvPoint iorg1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexA];
		CvPoint idst1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexB];
		cvLine( dst, iorg1, idst1, cvScalar(100, 100, 100), 1, CV_AA, 0);
		iorg1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexB];
		idst1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexC];
		cvLine( dst, iorg1, idst1, cvScalar(100, 100, 100), 1, CV_AA, 0);
		iorg1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexC];
		idst1 = pt2[g_trianglesWithoutDuplicates.tri[i].indexA];
		cvLine( dst, iorg1, idst1, cvScalar(100, 100, 100), 1, CV_AA, 0);
	}
*/

	cvNamedWindow("src");
	cvNamedWindow("dst");
	cvShowImage("src", src);
	cvShowImage("dst", dst);
	cvSetMouseCallback("src", OnMouse1);
	cvSetMouseCallback("dst", OnMouse2);
//	cvWaitKey();

	IplImage *result = cvCreateImage(cvSize(500, 500), 8, 3);
	for(int i = 0 ; i < g_trianglesWithoutDuplicates.nTriangles ;i++)
	{
		Transform(src, result, g_trianglesWithoutDuplicates.tri[i]);
	}
	cvNamedWindow("result");
	cvShowImage("result", result);
	cvWaitKey();

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

void Transform(IplImage *src, IplImage *dst, triangle tri)
{
	// Get the points from the indices.
	Point t1_pt1(pt1[tri.indexA].x, pt1[tri.indexA].y);
	Point t1_pt2(pt1[tri.indexB].x, pt1[tri.indexB].y);
	Point t1_pt3(pt1[tri.indexC].x, pt1[tri.indexC].y);
	Point t2_pt1(pt2[tri.indexA].x, pt2[tri.indexA].y);
	Point t2_pt2(pt2[tri.indexB].x, pt2[tri.indexB].y);
	Point t2_pt3(pt2[tri.indexC].x, pt2[tri.indexC].y);
	int ix1[3]={t1_pt1.x, t1_pt2.x, t1_pt3.x};
	int iy1[3]={t1_pt1.y, t1_pt2.y, t1_pt3.y};
	int ix2[3]={t2_pt1.x, t2_pt2.x, t2_pt3.x};
	int iy2[3]={t2_pt1.y, t2_pt2.y, t2_pt3.y};
	CMyTriangle myTri;
// 	myTri.m_pt[0].x = ix2[0];
// 	myTri.m_pt[0].y = iy2[0];
// 	myTri.m_pt[1].x = ix2[1];
// 	myTri.m_pt[1].y = iy2[1];
// 	myTri.m_pt[2].x = ix2[2];
// 	myTri.m_pt[2].y = iy2[2];

	float a[3][3], ia[3][3];
	for(int i=0; i<3; i++)
	{
		a[i][0] = ix1[i];
		a[i][1] = iy1[i];
		a[i][2] = 1.0f;
	}

	InverseMatrixGJ(a, ia, 3);

	float m[3][3] = {{0,0,0},{0,0,0},{0,0,1}};

	for(int i=0; i<3; i++)
	{
		m[0][0] += ia[0][i]*ix2[i];
		m[0][1] += ia[1][i]*ix2[i];
		m[0][2] += ia[2][i]*ix2[i];
		m[1][0] += ia[0][i]*iy2[i];
		m[1][1] += ia[1][i]*iy2[i];
		m[1][2] += ia[2][i]*iy2[i];
	}
	int w_1 = src ->width;
	int h_1 = src ->height;
	int w_2 = dst ->width;
	int h_2 = dst ->height;

	float x1,y1, x2,y2 , w1=1, w2 =1;
	for(y1=0; y1<h_1; y1 += 0.5)
	{
		for(x1=0; x1<w_1; x1 += 0.5)
		{
			x2 = m[0][0]*x1 + m[0][1]*y1 + m[0][2]*w1;
			y2 = m[1][0]*x1 + m[1][1]*y1 + m[1][2]*w1;
			w2 = m[2][0]*x1 + m[2][1]*y1 + m[2][2]*w1;

			x2 = x2/w2;
			y2 = y2/w2;

// 			if(myTri.IsIn(x2, y2) == false)
// 				continue;
			if(myTri.PointInTriangle(Point(x2, y2), t2_pt1, t2_pt2, t2_pt3) == false)
				continue;


			CvScalar f = cvGet2D(g_dst, y2, x2);
			cvSet2D(dst, y2, x2, f);
		}
	}
}

void FindEdge(delaunayData &array, int index, int initialIndex, int N)
{
	if(N == 0)
	{
		if(initialIndex == index)
			//Triangle!
		{
			// Basically, in DetermineTriangle Function, I have to declare tree data structure. and then in FindEdge. save the data.
			// When Find a triangle, update the lowest level of node.
			// I changed my mind! R-tree the maximum node number is 8. Unstable! Simply
			// declare the data structure --> 
			// ex> 
			g_triangles.tri[g_triangles.nTriangles].foundTriangle = true;
			g_triangles.tri[g_triangles.nTriangles].indexA = nTmpInitIndex;
			g_triangles.tri[g_triangles.nTriangles].indexB = nTmpIndex2;
			g_triangles.tri[g_triangles.nTriangles].indexC = nTmpIndex3;
			g_triangles.nTriangles ++;
			return;
		}
		else // Not Triangle!
		{
			return;
		}
	}
	if(N == 3)
		nTmpInitIndex = index;
	else if(N == 2)
		nTmpIndex2 = index;
	else if(N == 1)
		nTmpIndex3 = index;
	delaunayData arrData;
	for(int i = 0 ; i < array.nEdges ;i++)
	{
		if(array.edges[i].nIndexOrg == index)
		{
			arrData.edges[arrData.nEdges].nIndexOrg = array.edges[i].nIndexOrg;
			arrData.edges[arrData.nEdges].a = array.edges[i].a;
			arrData.edges[arrData.nEdges].nIndexDst = array.edges[i].nIndexDst;
			arrData.edges[arrData.nEdges].b = array.edges[i].b;
			arrData.nEdges++;
		}
		else if(array.edges[i].nIndexDst == index)
		{
			arrData.edges[arrData.nEdges].nIndexOrg = array.edges[i].nIndexDst;
			arrData.edges[arrData.nEdges].a = array.edges[i].b;
			arrData.edges[arrData.nEdges].nIndexDst = array.edges[i].nIndexOrg;
			arrData.edges[arrData.nEdges].b = array.edges[i].a;
			arrData.nEdges++;
		}
	}
	// Now we have all the connecting edges from the point(index) ---> Example > 0 is the pivot index. ==> we have (0, 1), (0, 45), (0, 12),(0,8) ,,.. etc.
	// Now we recursively find the next connecting edges from the point B. ==>
	// For Example > (0, 1), (0, 45), (0, 8) ==> Find connecting edges from Point(1). Point(45), Point(8)
	for(int i = 0 ; i < arrData.nEdges ; i++)
	{
		FindEdge(array, arrData.edges[i].nIndexDst, initialIndex, N - 1);
	}
}



void DetermineTriangles(delaunayData delData)
{
	for(int i = 0 ; i < indexPoint1 ; i++)
	{
		FindEdge(delData, i, i, 3);
	}
	int tmp;
	for(int i = 0 ; i < g_triangles.nTriangles ; i++)
	{
		// Sort vertex index order.
		if(g_triangles.tri[i].indexA > g_triangles.tri[i].indexB)
		{
			tmp = g_triangles.tri[i].indexA;
			g_triangles.tri[i].indexA = g_triangles.tri[i].indexB;
			g_triangles.tri[i].indexB = tmp;
		}
		if(g_triangles.tri[i].indexB > g_triangles.tri[i].indexC)
		{
			tmp = g_triangles.tri[i].indexB;
			g_triangles.tri[i].indexB = g_triangles.tri[i].indexC;
			g_triangles.tri[i].indexC = tmp;
		}
		if(g_triangles.tri[i].indexA > g_triangles.tri[i].indexB)
		{
			tmp = g_triangles.tri[i].indexA;
			g_triangles.tri[i].indexA = g_triangles.tri[i].indexB;
			g_triangles.tri[i].indexB = tmp;
		}
	} // sorting done.
	for(int i = 0 ; i < g_triangles.nTriangles ; i++)
	{
		// If the newly formed list has the triangle g_triangles.tri[i], then do not copy. Otherwise, copy.
		bool bFoundInList = false;
		for(int j = 0 ; j < g_trianglesWithoutDuplicates.nTriangles ; j++)
		{
			if(g_trianglesWithoutDuplicates.tri[j] == g_triangles.tri[i])
				bFoundInList = true;
		}
		if(bFoundInList == false)
			// Copy
		{
			g_trianglesWithoutDuplicates.tri[g_trianglesWithoutDuplicates.nTriangles] = g_triangles.tri[i];
			g_trianglesWithoutDuplicates.nTriangles ++;
		}
		else
			;//Do not copy.
	}
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

//		if( cvWaitKey(1) >= 0 )
//			break;

		cvSubdivDelaunay2DInsert( subdiv, fp );
		cvCalcSubdivVoronoi2D( subdiv );
		//cvSet( img, bkgnd_color, 0 );
		//draw_subdiv( img, subdiv, delaunay_color, voronoi_color );

//		if( cvWaitKey(1) >= 0 )
//			break;
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

//	cvCircle(img, cvPoint(cvRound(fp.x), cvRound(fp.y)), 3, active_color, CV_FILLED, 8, 0 );
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

//		cvLine( img, iorg, idst, color, 1, CV_AA, 0 );
		// HEre I add the code to save point org and point dst.
//		cout << iorg.x << " " << iorg.y << " TO " << idst.x << " " << idst.y << endl;
		// Get Indices of the points from pt1. and then match it with pt2 later.
		int nOrgIndex = -1;
		int nDstIndex = -1;
		int count_org = 0;
		int count_dst = 0;
		for(int i = 0 ; i < indexPoint1 ; i++)
		{
			Point pt;
			pt = pt1[i];
			if(pt == iorg)
			{
				nOrgIndex = i;
				count_org++;
			}
			if(pt == idst)
			{
				nDstIndex = i;
				count_dst++;
			}
		}
		if(count_org == 1 && count_dst == 1)
		{
			// both are one. It forms a real edge.
			g_delaunayData.edges[g_delaunayData.nEdges].a = iorg;
			g_delaunayData.edges[g_delaunayData.nEdges].b = idst;
			g_delaunayData.edges[g_delaunayData.nEdges].nIndexOrg = nOrgIndex;
			g_delaunayData.edges[g_delaunayData.nEdges].nIndexDst = nDstIndex;
			g_delaunayData.nEdges ++;
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
	// Manipulate.. Add points on the edge too so that delaunay will naturally make the triangles for the side area of the image.
	int nIndex = indexPoint1;
	static int nNumOfPoints = 10;
	for(int j = 0 ; j < nNumOfPoints ; nIndex++, j++)
	{
		pt1[nIndex].x = float(g_src ->width) / (nNumOfPoints - 1) * j; // error!?
		if(pt1[nIndex].x == g_src ->width)
			pt1[nIndex].x --;
		pt1[nIndex].y = 0;
	} // i == indexPoint1 + 10
	indexPoint1 += nNumOfPoints;
	for(int j = 0 ; j < nNumOfPoints ; nIndex++, j++)
	{
		pt1[nIndex].x = float(g_src ->width) / (nNumOfPoints - 1) * j; // error?!
		if(pt1[nIndex].x == g_src ->width)
			pt1[nIndex].x --;
		pt1[nIndex].y = g_src ->height - 1;
	}
	indexPoint1 += nNumOfPoints;
	for(int j = 1 ; j < nNumOfPoints - 1 ; nIndex++, j++)
	{
		pt1[nIndex].x = 0;
		pt1[nIndex].y = float(g_src ->height) / (nNumOfPoints - 1) * j;
		if(pt1[nIndex].y == g_src ->height)
			pt1[nIndex].y --;
	}
	indexPoint1 += nNumOfPoints - 2;
	for(int j = 1 ; j < nNumOfPoints - 1 ; nIndex++, j++)
	{
		pt1[nIndex].x = g_src ->width - 1;
		pt1[nIndex].y = float(g_src ->height) / (nNumOfPoints - 1) * j;
		if(pt1[nIndex].y == g_src ->height)
			pt1[nIndex].y --;
	}
	indexPoint1 += nNumOfPoints - 2;
	inputfile >> indexPoint2;
	for(int i = 0 ; i < indexPoint2 ; i++)
	{
		inputfile >> pt2[i].x;
		inputfile >> pt2[i].y;
	}
	// Manipulate.. Add points on the edge too so that delaunay will naturally make the triangles for the side area of the image.
	nIndex = indexPoint2;
	// static int nNumOfPoints = 10;
	for(int j = 0 ; j < nNumOfPoints ; nIndex++, j++)
	{
		pt2[nIndex].x = float(g_dst ->width) / (nNumOfPoints - 1) * j; // error!?
		if(pt2[nIndex].x == g_dst ->width)
			pt2[nIndex].x --;
		pt2[nIndex].y = 0;
	} // i == indexPoint2 + 10
	indexPoint2 += nNumOfPoints;
	for(int j = 0 ; j < nNumOfPoints ; nIndex++, j++)
	{
		pt2[nIndex].x = float(g_dst ->width) / (nNumOfPoints - 1) * j; // error?!
		if(pt2[nIndex].x == g_dst ->width)
			pt2[nIndex].x --;
		pt2[nIndex].y = g_dst ->height - 1;
	}
	indexPoint2 += nNumOfPoints;
	for(int j = 1 ; j < nNumOfPoints - 1 ; nIndex++, j++)
	{
		pt2[nIndex].x = 0;
		pt2[nIndex].y = float(g_dst ->height) / (nNumOfPoints - 1) * j;
		if(pt2[nIndex].y == g_dst ->height)
			pt2[nIndex].y --;
	}
	indexPoint2 += nNumOfPoints - 2;
	for(int j = 1 ; j < nNumOfPoints - 1 ; nIndex++, j++)
	{
		pt2[nIndex].x = g_dst ->width - 1;
		pt2[nIndex].y = float(g_dst ->height) / (nNumOfPoints - 1) * j;
		if(pt2[nIndex].y == g_dst ->height)
			pt2[nIndex].y --;
	}
	indexPoint2 += nNumOfPoints - 2;
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
		cvCircle(g_src, pt1[indexPoint1], 3, CV_RGB(255,0,0), -1);
		cvShowImage("src", g_src);
		indexPoint1 ++;
		cout << x << " " << y << endl;
	}
	if(event == CV_EVENT_RBUTTONDOWN && flags == CV_EVENT_FLAG_RBUTTON)
	{
		cout << "Point clicked : " << x << " " << y << endl;
	}
}

void OnMouse2(int event, int x, int y, int flags, void * param)
{
	if(event == CV_EVENT_LBUTTONDOWN && flags == CV_EVENT_FLAG_LBUTTON)
	{
		pt2[indexPoint2] = cvPoint(x, y);
		cvCircle(g_dst, pt2[indexPoint2], 3, CV_RGB(0,0,255), -1);
		cvShowImage("dst", g_dst);
		indexPoint2 ++;
		cout << x << " " << y << endl;
	}
	if(event == CV_EVENT_RBUTTONDOWN && flags == CV_EVENT_FLAG_RBUTTON)
	{
		cout << "Point clicked : " << x << " " << y << endl;
	}
}

/*
CvPoint2D32f ptSource[4], ptPrespective[4];
ptSource[0] = cvPoint2D32f(0, 0);
ptSource[1] = cvPoint2D32f(img ->width, 100);
ptSource[2] = cvPoint2D32f(img ->width, img ->height -100);
ptSource[3] = cvPoint2D32f(0, img ->height -100);

// 변환할 네 점의 좌표는 앞에서 여러가지 방법으로 구해진 점들을 대입하는 형태로 구성하였다.
for(int i=0; i < 4; i++ )  ptSource[i] = cvPoint2D32f( ptSource[i].x, ptSource[i].y ); 

// 변환될 네 점의 좌표는 생성할 이미지의 꼭지점 좌표로 설정하였다.
ptPrespective[0]  = cvPoint2D32f(0, 0);
ptPrespective[1]  = cvPoint2D32f(dst->width, 0);
ptPrespective[2] = cvPoint2D32f(dst->width, dst->height);
ptPrespective[3] = cvPoint2D32f(0, dst->height);

float newm[9];            
CvMat mat = cvMat( 3, 3, CV_32F, newm );  // 변환행렬 초기화

cvWarpPerspectiveQMatrix(ptSource, ptPrespective, &mat);  // 변환행렬 구하기

// 이미지 와핑
cvWarpPerspective(img, dst, &mat,CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
cvShowImage("Morphing1", img);
cvShowImage("Morphing2", dst);
cvWaitKey();
return 1;*/


/*


class MyMat4x4
{
public:
float p[4][4];		//	00 01 02 03
//  10 11 12 13
//  20 21 22 23
//  30 31 32 33
MyMat4x4()
{
SetIdentity();
}
void SetIdentity()
{
for(int i=0; i<4; i++)
for(int j=0; j<4; j++)
p[i][j] = 0.0;
p[0][0] = 1;	p[1][1] = 1;
p[2][2] = 1;	p[3][3] = 1;
}
void RotateZ(float theta)	
{
SetIdentity();
float rad = theta * 3.141592/180.0f;
p[0][0] = cos(rad);
p[0][1] = -sin(rad);
p[1][0] = sin(rad);
p[1][1] = cos(rad);
}
void Translate(float x, float y, float z)
{
SetIdentity();
p[0][3] = x;
p[1][3] = y;
p[2][3] = z;
}
MyVector4 operator * (MyVector4 in)
{
MyVector4 out;
for(int j=0; j<4; j++)
{
out.p[j] = 0.0;
for(int i=0; i<4; i++)
out.p[j] += p[j][i]*in.p[i]; 
}
return out;
}

MyMat4x4 operator * (MyMat4x4 in)
{
MyMat4x4 out;
for(int i=0; i<4; i++)
for(int j=0; j<4; j++)
{
out.p[i][j] = 0;
for(int k=0; k<4; k++)
out.p[i][j]+=
p[i][k]*in.p[k][j];
}

return out;
}

};


*/