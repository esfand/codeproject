#ifndef	TVT_CLIENT_H
#define	TVT_CLIENT_H

#ifndef	null
#define	null	0
#endif

class	Client
{
public:
	Client() : clientId_(null)
	{
		clientEvent_ [0] = TCHAR(0);
	}
	~Client()	{}	//	Delete session?
private:
	HANDLE		clientId_;
	TCHAR		clientEvent_[MAX_PATH];
	TEST_OBJECT	root;
//	TEST_SESSION	session;
};

class ClientAction
{
private:
	DWORD	ident_;
	TCHAR*	entity_;
public:
	ClientAction	(DWORD ident,TCHAR* entity) : ident_(ident), entity_(entity) {}
	~ClientAction	()
	{
		if (entity_)
			delete	entity_
		;
	}

	DWORD	Ident()		{ return ident_; }
	TCHAR*	Entity()	{ return entity_; }
};

#endif
