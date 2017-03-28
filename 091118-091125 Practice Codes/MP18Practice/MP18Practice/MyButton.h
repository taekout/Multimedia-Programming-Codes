#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class CMyButton
{
public:
	CMyButton(void);

	CMyButton(int ind, CvPoint p1, CvPoint p2);
	
public:
	~CMyButton(void);

	void Set(int ind, CvPoint p1, CvPoint p2);
	void DoIt(int x, int y);

	CvPoint m_pt1, m_pt2;
	int m_index;

	void (*func)(int x, int y);

	bool HitTest(CvPoint pt);
};

