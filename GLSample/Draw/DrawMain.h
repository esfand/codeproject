#if !defined (DRAWMAIN_H_E20F17B9_0045_4F33_81F7_6BF65092DE9A)
#define DRAWMAIN_H_E20F17B9_0045_4F33_81F7_6BF65092DE9A

// needed to pass multiple args when creating a worker thread
typedef struct
{
	VSYNC Status;		// flag to indicate if we enable or disable or leave alone vsync
	HWND hWnd;			// handle to the calling window
	HDC hDC;			// handle to the dc of the client area of the calling window
	BYTE nBPP;			// bits per pixel the application is trying to use (ignored if windowed)
	BYTE nRefresh;		// vertical refresh rate of the display in hertz (ignored if windowed)
	bool bFullscreen;	// flag to indicate to the thread if we are in fullscreen mode
	bool bZoomed;		// flag to indicate to the thread if we are to maximize the main window

}  RENDERARGS, *PRENDERARGS;

// function prototypes
extern unsigned int __stdcall InitScene (const PRENDERARGS pArgList);

// user defined window messages the render thread uses to communicate
#define UWM_PAUSE	(WM_APP + 1)
#define UWM_RESIZE	(WM_APP + 2)
#define UWM_SHOW	(WM_APP + 3)
#define UWM_STOP	(WM_APP + 4)

#endif  // DRAWMAIN_H
