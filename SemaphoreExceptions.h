#include<iostream>
#include<string>
#include<exception>

#ifndef SemaphoreExceptions_H
#define SemaphoreExceptions_H

const char* IllegalArgumentException_msg = "Exception : Initialization not allowed with -ve permits in strict mode. Use relaxed mode instead.";

struct IllegalArgumentException : public std::exception
{
	const char * what () const throw ()
	{
		return(IllegalArgumentException_msg);
	}
};


const char* IllegalReleasePermitsArgumentException_msg = "Exception : release method can not have -ve permits. Please check the value.";

struct IllegalReleasePermitsArgumentException : public std::exception
{
	const char * what () const throw ()
	{
		return(IllegalReleasePermitsArgumentException_msg);
	}
};


const char* IllegalReleasePermitsReleaseException_msg = "Exception : An attempt was made to release permits greater than the current number of permits [in strict mode] held by the thread id =";

struct IllegalReleasePermitsReleaseException : public std::exception
{
	const char * what () const throw ()
	{
		return(IllegalReleasePermitsReleaseException_msg);
	}
};


const char* IllegalRelasePermitsException_msg = "Exception : An attempt was made by thread id = to release permits in strict mode where it has no currently acquired permits.Please investigate.";

struct IllegalRelasePermitsException : public std::exception
{
	const char * what () const throw ()
	{
		return(IllegalRelasePermitsException_msg);
	}
};


#endif
