///////////////////////////////////////////////////////////////////////////////
// EASYSTRING.H
// One of the design goals of CppUnitLite is to compilation with very old C++
// compilers.  For that reason, I've added a simple string class that provides
// only the operations needed in CppUnitLite.
///////////////////////////////////////////////////////////////////////////////
#ifndef EASY_STRING_H
#define EASY_STRING_H

#include <windows.h>
#include <string>

class EasyString
{
	friend bool	operator== (const EasyString& left, const EasyString& right);
	friend EasyString operator+ (const EasyString& left, const EasyString& right);
public:
	EasyString ();
	EasyString (const char *value);
	EasyString (const EasyString& other);
   ~EasyString ();

	EasyString operator= (const EasyString& other);

	char*	AsCharString () const;
	int		Length() const;

private:
	char	*buffer_;
};

EasyString StringFrom (bool					value);
EasyString StringFrom (const char*			value);
EasyString StringFrom (long					value);
EasyString StringFrom (int					value);
EasyString StringFrom (double				value);
EasyString StringFrom (const EasyString&	other);
EasyString StringFrom (const std::string&	value);
EasyString StringFrom (LPTSTR*				value);

#endif
