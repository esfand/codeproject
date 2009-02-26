#ifndef	TVT_SERVER_H
#define	TVT_SERVER_H

#include "TDDOwner.h"

class TestViewer
{
public:

	TestViewer()
	{
		hActivityEvent = CreateEvent(NULL, TRUE, FALSE, TDD_EVENT_VALUE);
	}

	~TestViewer()
	{
		if (hActivityEvent)
			CloseHandle(hActivityEvent)
		;
	}
private:
//	Data
	HANDLE	hActivityEvent;
};


#endif
