#include "SimpleString.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable : 4996)

static const int DEFAULT_SIZE = 256;

EasyString::EasyString ()
: buffer_(new char [1])
{
	buffer_ [0] = '\0';
}

EasyString::EasyString (const char *otherBuffer)
: buffer_ (new char [strlen (otherBuffer) + 1])
{
	strcpy (buffer_, otherBuffer);
}

EasyString::EasyString (const EasyString& other)
{
	buffer_ = new char [other.Length() + 1];
	strcpy(buffer_, other.buffer_);
}

EasyString EasyString::operator= (const EasyString& other)
{
	delete buffer_;
	buffer_ = new char [other.Length() + 1];
	strcpy(buffer_, other.buffer_);	
	return *this;
}

char *EasyString::AsCharString () const
{
	return buffer_;
}

int EasyString::Length() const
{
	return (int)strlen (buffer_);
}

EasyString::~EasyString ()
{
	delete [] buffer_;
}

bool operator== (const EasyString& left, const EasyString& right)
{
	return !strcmp (left.AsCharString(),right.AsCharString());
}

EasyString operator+ (const EasyString& left, const EasyString& right)
{
	EasyString	result;
	char* buffer = new char [left.Length() + right.Length() + 1];
	strcpy (buffer,left.AsCharString());
	strcpy (&buffer[strlen(buffer)],right.AsCharString());
	result = buffer;
	delete [] buffer;
	return result;
}

EasyString StringFrom (bool value)
{
	char buffer [sizeof ("false") + 1];
	sprintf (buffer, "%s", value ? "true" : "false");
	return EasyString(buffer);
}

EasyString StringFrom (const char *value)
{
	return EasyString(value);
}

EasyString StringFrom (long value)
{
	char buffer [DEFAULT_SIZE];
	sprintf (buffer, "%ld", value);
	return EasyString(buffer);
}

EasyString StringFrom (int value)
{
	char buffer [DEFAULT_SIZE];
	sprintf (buffer, "%d", value);
	return EasyString(buffer);
}

EasyString StringFrom (double value)
{
	char buffer [DEFAULT_SIZE];
	sprintf (buffer, "%lf", value);
	return EasyString(buffer);
}

EasyString StringFrom (const EasyString& value)
{
	return EasyString(value);
}

EasyString StringFrom(const std::string& value)
{
	return EasyString(value.c_str());
}

EasyString StringFrom (LPTSTR*	value)
{
	return EasyString((const char*)value);	
}


