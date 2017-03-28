#include "StdAfx.h"
#include "MyButton.h"

CMyButton::CMyButton(void)
{
	m_pt1 = cvPoint(0,0);
	m_pt2 = cvPoint(0,0);
	m_index = 0;
}
CMyButton::CMyButton(int ind, CvPoint p1, CvPoint p2)
{
	m_pt1 = p1;
	m_pt2 = p2;
	m_index = ind;
}

void CMyButton::Set(int ind, CvPoint p1, CvPoint p2)
{
	m_pt1 = p1;
	m_pt2 = p2;
	m_index = ind;
}

void CMyButton::DoIt(int x, int y)
{
	func(x,y);
}
CMyButton::~CMyButton(void)
{
}


bool CMyButton::HitTest(CvPoint pt)
{
	if(m_pt1.x <pt.x && m_pt2.x>pt.x 
		&& m_pt1.y <pt.y && m_pt2.y>pt.y)
		return true;
	else
		return false;
}
