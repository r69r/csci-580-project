// Application.h: interface for the Application class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATION_H__3387B79A_B69F_491D_B782_81D9CAFAAB0F__INCLUDED_)
#define AFX_APPLICATION_H__3387B79A_B69F_491D_B782_81D9CAFAAB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Gz.h"
#include "disp.h"
#include "rend.h"

class Application  
{
public:
	Application();
	virtual ~Application();
	
public:
	GzDisplay* m_pDisplay;		// the display

	GzDisplay* m_pDisplay1;		// the display
	GzRender*  m_pRender1;		// the renderer

	GzDisplay* m_pDisplay2;		// the display
	GzRender*  m_pRender2;		// the renderer

	GzDisplay* m_pDisplay3;		// the display
	GzRender*  m_pRender3;		// the renderer

	GzDisplay* m_pDisplay4;		// the display
	GzRender*  m_pRender4;		// the renderer

	GzDisplay* m_pDisplay5;		// the display
	GzRender*  m_pRender5;		// the renderer

	GzDisplay* m_pDisplay6;		// the display
	GzRender*  m_pRender6;		// the renderer

	GzInput*   m_pUserInput;
	char* m_pFrameBuffer;	// Frame Buffer
	int   m_nWidth;			// width of Frame Buffer
	int   m_nHeight;		// height of Frame Buffer

	double AAFilter[AAKERNEL_SIZE][3] 		/* X-shift, Y-shift, weight */
	{
		-0.52, 0.38, 0.128, 		0.41, 0.56, 0.119,		0.27, 0.08, 0.294,
		-0.17, -0.29, 0.249,		0.58, -0.55, 0.104,		-0.31, -0.71, 0.106
	};

	virtual int Render()=0; // Pass user input data and call renderer
};

#endif // !defined(AFX_APPLICATION_H__3387B79A_B69F_491D_B782_81D9CAFAAB0F__INCLUDED_)
