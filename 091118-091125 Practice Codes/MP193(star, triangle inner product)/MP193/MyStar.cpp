  #include "StdAfx.h"
#include "MyStar.h"

CMyStar::CMyStar(void)
{
}

CMyStar::~CMyStar(void)
{
}

CMyStar::CMyStar(float l1, float l2, CvPoint center)
{
	m_l1 = l1;
	m_l2 = l2;
	m_pt[0] = center;

	float dt = 36.0f;
	for(int i=0; i<10; i++)
	{
		float theta = (dt*i-90)*3.141592/180.0f ;
		float length = m_l1;
		if(i%2 == 1) length = m_l2;

		m_pt[i+1].x = length*(cos(theta)) + m_pt[0].x;
		m_pt[i+1].y = length*(sin(theta)) + m_pt[0].y;
	}
	for(int i=0; i<9; i++)
	{
		m_tri[i].m_pt[0] = m_pt[0];
		m_tri[i].m_pt[1] = m_pt[i+1];
		m_tri[i].m_pt[2] = m_pt[i+2];
	}
	m_tri[9].m_pt[0] = m_pt[0];
	m_tri[9].m_pt[1] = m_pt[10];
	m_tri[9].m_pt[2] = m_pt[1];

}

void CMyStar::Draw(IplImage * img)
{
	
	for(int i=0; i<10; i++)
		m_tri[i].Draw(img);
}

bool CMyStar::IsIn(CvPoint pt)
{
	for(int i=0; i<10; i++)
		if (m_tri[i].IsIn(pt)) return true;
	
	return false;	
}