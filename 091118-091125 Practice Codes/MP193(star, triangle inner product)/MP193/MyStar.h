#pragma once

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "MyTriangle.h";

class CMyStar
{
public:
	CMyStar(void);
	CMyStar(float l1, float l2, CvPoint center);

public:
	~CMyStar(void);

	CvPoint m_pt[11];
	CMyTriangle m_tri[10];

	float m_l1, m_l2;

	void Draw(IplImage * img);

	bool IsIn(CvPoint pt);
};
