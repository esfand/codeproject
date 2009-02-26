#include "..\WinMain.h"			// standard application include
#include "..\Util\Misc.h"		// miscellaneous routines
#include "DrawMain.h"			// include for this file

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// MAIN OPENGL DRAWING ROUTINES ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// local state variables, used to signal the worker thread (terminate, resize, etc.)
// if another thread wishes to set these they must do so via the message queue
static bool bStopRenderThread = false;
static bool bResizeFrame = false;
static bool bPaused = false;

// local function prototypes
static double GetCPUTicks	(void);
static void   OnPreprocess	(const PRENDERARGS pArgList);
static void   OnResizeFrame	(HWND hWnd, unsigned int nWidth, unsigned int nHeight);
static void   RenderFrame	(const double dElapsed, const unsigned int nWidth, const unsigned int nHeight);
static void   ThreadProc	(UINT uMsg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / double
/ /		GetCPUTicks (void)
/ /
/ / PURPOSE:
/ /		Returns the number of successive CPU cycles in hertz (per
/ /		second) since the last time this routine was called.
/ /
/ / CREDITS:
/ /		This routine was based on the work of Jeffry J. Brickley.
/*/

static double
GetCPUTicks (void)
{
	static LARGE_INTEGER nFreq = {0}, nCount = {0};
	static double dReturn = 0.0, dCheckTime = 5.0;

	// check for a new frequency once every 5 seconds
	// note: this is in case ACPI, etc. alters it
	if((nFreq.QuadPart == 0) || (dCheckTime < dReturn))
	{
		dCheckTime = dReturn + 5.0;

		// avoid a division by zero by returning zero on error
		if(!QueryPerformanceFrequency(&nFreq)) return 0.0;
	}

	// use the ratio of tick amount divided by frequency to find the hertz
	QueryPerformanceCounter(&nCount);
	dReturn = ((double)nCount.QuadPart / (double)nFreq.QuadPart);
	return dReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / unsigned int __stdcall
/ /		InitScene (const PRENDERARGS pArgList)
/ /
/ /		pArgList->hWnd = handle to the window with the rc to use
/ /		pArgList->nWidth = width in units for the perseictive
/ /		pArgList->nHeight = height in units for the perseictive
/ /
/ / PURPOSE:
/ /		Wraps the process of calling initialization routines
/ /		and contains the main loop for the drawing functions.
/ /
/ / NOTE:
/ /		The function must be declaed as __stdcall. Also, take
/ /		note, this function is in a seperate worker thread!
/*/

unsigned int __stdcall
InitScene (const PRENDERARGS pArgList)
{
	#ifdef _DEBUG
		DWORD dwFPSCurrent = 0, dwFPSLast = 0;	// used to help calc the fps
		unsigned short nFPS = 0;				// current framerate/FPS for the main loop
	#endif

	static double dLastTime = 0, dCurTime = 0;	// used to calc CPU cycles during a render
	static double dElapsed = 0;					// used to calc CPU cycles during a render
	HGLRC hRC = NULL;							// handle to the GLs render context
	RECT rcClient = {0};						// coords of the area safe to draw on
	MSG msg	= {0};								// message struct for the queue

	// create and activate (in OGL) the render context
	hRC = wglCreateContext(pArgList->hDC);
	if(wglMakeCurrent(pArgList->hDC, hRC))
	{
		#ifdef _DEBUG
			ENTER_GL
		#endif

		// call the preprocess event handler to take care of initialization routines specific to OGL
		OnPreprocess(pArgList);

		#ifdef _DEBUG
			// test to see if we got an initialization error, if so then stop the render thread
			bStopRenderThread = LEAVE_GL(_T("OnPreprocess()"))
		#endif

		// if no previous error exists, let the main thread know it's ok to display the main window
		if(!bStopRenderThread) bStopRenderThread = (bool)!SendMessage(pArgList->hWnd, UWM_SHOW, pArgList->bZoomed, 0);
	}

	////////////////////// START OF OGL DRAWING/PROCESSING ////////////////////

	// since we are in another thread, start a separate endless loop for rendering
	while(!bStopRenderThread)
	{
		// NOTE: it is imparative that PeekMessage() is used rather than GetMessage()
		// listen to the queue in case this thread receives a message
		if(PeekMessage(&msg, NULL, UWM_PAUSE, UWM_STOP, PM_REMOVE))
			ThreadProc(msg.message, msg.wParam, msg.lParam);

		// do not waste processing time if the window is minimized
		// note: even in fullscreen a window can end up minimized
		if(!IsIconic(pArgList->hWnd))
		{
			// if we need to resize the window then do so, but only once per request
			// this will be called on startup
			if(bResizeFrame)
			{
				// set-up the perspective screen to be the size of the client area
				// NOTE: to avoid clipping, use the client area size, not the window size
				GetClientRect(pArgList->hWnd, &rcClient);

				// call handler and set flag that it's been processed
				OnResizeFrame(pArgList->hWnd, rcClient.right, rcClient.bottom);
				bResizeFrame = false;
			}

			// only perform this action if the rendering isn't paused
			if(!bPaused)
			{
				#ifdef _DEBUG
					// use a low resolution for the FPS count
					dwFPSCurrent = GetTickCount();

					ENTER_GL
				#endif

				// call the main drawing routine, if it's to be seen, this routine must show it
				// note: we use time-based rendering, so the time arg should be used as a factor
				dCurTime = GetCPUTicks();
				dElapsed = dCurTime - dLastTime;
				RenderFrame(dElapsed, rcClient.right, rcClient.bottom);
				dLastTime = dCurTime;

				#ifdef _DEBUG
					// if we had OGL errors during the render, let's find out about them
					bPaused = LEAVE_GL(_T("RenderFrame()"))
				#endif

				// swap the buffers (double buffering)
				SwapBuffers(pArgList->hDC);

				#ifdef _DEBUG

					if(!pArgList->bFullscreen)
					{
						// display FPS on the titlebar of the main window if not in fullscreen mode
						if(nFPS == 0)
						{
							dwFPSLast = dwFPSCurrent;
							nFPS = 1;
						}
						else
						{
							// check for overflow (as a precaution)
							if(nFPS < 0xFFFF) nFPS++;

							// reset the FPS counter if we reach a new second
							if((dwFPSCurrent - dwFPSLast) >= 1000)
							{
								TCHAR szBuff[MAX_LOADSTRING] = {0};

								// to save performance, only update the window title once a second
								// also, place the OGL version information in the titlebar
								wsprintf(szBuff, _T("OpenGL %s - %hu FPS"), glGetString(GL_VERSION), nFPS);
								SetWindowText(pArgList->hWnd, szBuff);

								nFPS = 0;
							}
						}
					}
				#endif
			}
			else
			{
				// in order to keep the timed-based counter current, call this if we're paused
				dLastTime = GetCPUTicks();
			}
		}
	}

	////////////////////// END OF OGL DRAWING/PROCESSING //////////////////////

	// clean-up (OGL and wiggle specific items)
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);

	// kill this thread and its resources (CRT allocates them)
	_endthreadex(0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		OnPreprocess (const PRENDERARGS pArgList)
/ /
/ /		pArgList = pointer to argument structure that the render thread received
/ /
/ / PURPOSE:
/ /		This routine is meant to setup OGL specifics before the
/ /		rendering loop starts. This will be only called once.
/ /
/ / NOTES:
/ /		This must be called in the context of the render thread.
/*/

static void
OnPreprocess (const PRENDERARGS pArgList)
{
	if(pArgList != NULL)
	{
		GLfloat LightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};

		// do we modify vsync?
		SetVSync(pArgList->Status);

		glShadeModel(GL_SMOOTH);					// enable smooth shading
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);		// black background/clear color

		// set-up the depth buffer
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
		glDepthFunc(GL_LEQUAL);

		// set up one-byte alignment for pixel storage (saves memory)
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// use backface culling (this is 2D, so we'll never see the back faces anyway)
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);

		// perspective calculations
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		// use lighting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		OnResizeFrame (HWND hWnd, unsigned int nWidth, unsigned int nHeight)
/ /
/ /		hWnd = handle to the window who's client area is attach to the rc
/ /		nWidth = width to resize to (passed to save calcs)
/ /		nHeight = height to resize to (passed to save calcs)
/ /
/ / PURPOSE:
/ /		This routine sets up the perspective and viewport used by OGL.
/ /		It should be called as needed when its parent window is sized.
/ /
/ / NOTES:
/ /		This must be called in the context of the render thread.
/*/

static void
OnResizeFrame (HWND hWnd, unsigned int nWidth, unsigned int nHeight)
{
	// prevent division by zero
	if(nHeight <= 0) nHeight = 1;

	// (re)size the viewport to consume the entire client area
	glViewport(0, 0, nWidth, nHeight);

	// reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// (re)calculate the aspect ratio of the viewport (0,0 is bottom left)
	//glOrtho(0.0f, nWidth, 0.0f, nHeight, 0.0f, 1.0f);
	gluPerspective(45.0f, (float)nWidth / (float)nHeight, 1.0f, 100.0f);

	// lastly, reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		RenderFrame (const double dElapsed, const unsigned int nWidth, const unsigned int nHeight)
/ /
/ /		dElapsed = amount of time it took the last frame to render (in hertz)
/ /		nWidth = width of the rect/view that's our drawing area
/ /		nHeight = height of the rect/view that's our drawing area
/ /
/ / PURPOSE:
/ /		IMPORTANT: This routine is the starting point for the drawing routines!
/ /
/ / COMMENTS:
/ /		It would be best to start all drawing here.  This will allow
/ /		the developer to keep the drawing independant of the app and
/ /		easier to put code in and pull code out as needed.
/*/

static void
RenderFrame (const double dElapsed, const unsigned int nWidth, const unsigned int nHeight)
{
	static long double x = 0.0;
	const static GLfloat MatYellowDiffuse[] = {0.86f, 0.74f, 0.14f, 1.0f};
	const static GLfloat MatOrangeDiffuse[] = {0.78f, 0.59f, 0.0f, 1.0f};

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffers
	glLoadIdentity();									// reset modelview matrix

	// get the angle we wish to use
	if(x >= 360.0f) x = 0.0f;

	// move and rotate the cube
	glPushMatrix();
		glTranslatef(-5, -5, -35);
		glRotated(x, 0.0f, 1.0f, 0.0f);

		glColor3ub(220, 190, 35);		// yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatYellowDiffuse);

		// draw the cube
		glBegin(GL_TRIANGLES);
			glNormal3f(0, 0, -1);
			glVertex3f(-5, -5, -1);			// left
			glVertex3f(0, 5, -1);			// top
			glVertex3f(5, -5, -1);			// right

			glNormal3f(0, 0, 1);
			glVertex3f(-5, -5, 1);			// left
			glVertex3f(0, 5, 1);			// top
			glVertex3f(5, -5, 1);			// right
		glEnd();

		glColor3ub(200, 150, 0);			// dark yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatOrangeDiffuse);

		glBegin(GL_QUADS);
			glNormal3f(-1, 0, 0);
			glVertex3f(-5, -5, -1);			// bottom left
			glVertex3f(0, 5, -1);			// top left
			glVertex3f(0, 5, 1);			// top right
			glVertex3f(-5, -5, 1);			// bottom right

			glNormal3f(1, 0, 0);
			glVertex3f(5, -5, 1);			// bottom left
			glVertex3f(0, 5, 1);			// top left
			glVertex3f(0, 5, -1);			// top right
			glVertex3f(5, -5, -1);			// bottom right
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(5, -5, -35);
		glRotated(x, 0.0f, 1.0f, 0.0f);

		glColor3ub(220, 190, 35);		// yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatYellowDiffuse);

		// draw the cube
		glBegin(GL_TRIANGLES);
			glNormal3f(0, 0, -1);
			glVertex3f(-5, -5, -1);			// left
			glVertex3f(0, 5, -1);			// top
			glVertex3f(5, -5, -1);			// right

			glNormal3f(0, 0, 1);
			glVertex3f(-5, -5, 1);			// left
			glVertex3f(0, 5, 1);			// top
			glVertex3f(5, -5, 1);			// right
		glEnd();

		glColor3ub(200, 150, 0);			// dark yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatOrangeDiffuse);

		glBegin(GL_QUADS);
			glNormal3f(-1, 0, 0);
			glVertex3f(-5, -5, -1);			// bottom left
			glVertex3f(0, 5, -1);			// top left
			glVertex3f(0, 5, 1);			// top right
			glVertex3f(-5, -5, 1);			// bottom right

			glNormal3f(1, 0, 0);
			glVertex3f(5, -5, 1);			// bottom left
			glVertex3f(0, 5, 1);			// top left
			glVertex3f(0, 5, -1);			// top right
			glVertex3f(5, -5, -1);			// bottom right
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 5, -35);
		glRotated(x, 0.0f, 1.0f, 0.0f);

		glColor3ub(220, 190, 35);		// yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatYellowDiffuse);

		// draw the cube
		glBegin(GL_TRIANGLES);
			glNormal3f(0, 0, -1);
			glVertex3f(-5, -5, -1);			// left
			glVertex3f(0, 5, -1);			// top
			glVertex3f(5, -5, -1);			// right

			glNormal3f(0, 0, 1);
			glVertex3f(-5, -5, 1);			// left
			glVertex3f(0, 5, 1);			// top
			glVertex3f(5, -5, 1);			// right
		glEnd();

		glColor3ub(200, 150, 0);			// dark yellow
		glMaterialfv(GL_FRONT, GL_DIFFUSE, MatOrangeDiffuse);

		glBegin(GL_QUADS);
			glNormal3f(-1, 0, 0);
			glVertex3f(-5, -5, -1);			// bottom left
			glVertex3f(0, 5, -1);			// top left
			glVertex3f(0, 5, 1);			// top right
			glVertex3f(-5, -5, 1);			// bottom right

			glNormal3f(1, 0, 0);
			glVertex3f(5, -5, 1);			// bottom left
			glVertex3f(0, 5, 1);			// top left
			glVertex3f(0, 5, -1);			// top right
			glVertex3f(5, -5, -1);			// bottom right
		glEnd();
	glPopMatrix();

	// rotate 45 degrees every second
	x += (double)45.0 * dElapsed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		ThreadProc (UINT uMsg, WPARAM wParam, LPARAM lParam)
/ /
/ /		uMsg = message to process
/ /		wParam = word sized param who's value depends on the message
/ /		lParam = long sized param who's value depends on the message
/ /
/ / PURPOSE:
/ /		Procedure to handle messages for this thread.
/*/

static void
ThreadProc (UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case UWM_PAUSE:

			// notification that we should either pause or resume rendering
			bPaused = (bool)wParam;
			break;

		case UWM_RESIZE:

			// notification that viewport(s) need(s) to be resized
			bResizeFrame = true;
			break;

		case UWM_STOP:

			// notification that we should stop the render thread from executing
			bStopRenderThread = true;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////