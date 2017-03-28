 #pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class CMyTriangle
{
public:
	CMyTriangle(void);
public:
	~CMyTriangle(void);

	CvPoint m_pt[3];

	bool IsIn(CvPoint pt);
	void Draw(IplImage * img);

};
