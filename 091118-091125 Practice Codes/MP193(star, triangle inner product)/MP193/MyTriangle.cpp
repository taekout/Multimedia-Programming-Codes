#include "StdAfx.h"
#include "MyTriangle.h"

CMyTriangle::CMyTriangle(void)
{
}

CMyTriangle::~CMyTriangle(void)
{
}

float compCross(CvPoint a, CvPoint b, CvPoint c)
{
	float px = a.x - b.x;
	float py = a.y - b.y;
	float qx = c.x - b.x;
	float qy = c.y - b.y;
	return px*qy - py*qx;
}

bool CMyTriangle::IsIn(CvPoint pt)
{
	if(compCross(m_pt[1], m_pt[0], pt)>0  
		&& compCross(m_pt[2], m_pt[1], pt)>0
		&& compCross(m_pt[0], m_pt[2], pt)>0) 
		return true;

	return false;
}

void CMyTriangle::Draw(IplImage * img)
{
	CvScalar color = cvScalar(255,0,0);
	cvLine(img, m_pt[0], m_pt[1], color, 3);
	cvLine(img, m_pt[1], m_pt[2], color, 3);
	cvLine(img, m_pt[2], m_pt[0], color, 3);
}