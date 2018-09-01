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


const char* NegativeReleasePermitsException_msg = "Exception : release method can not have -ve permits. Please check the value.";

struct NegativeReleasePermitsException : public std::exception
{
	const char * what () const throw ()
	{
		return(NegativeReleasePermitsException_msg);
	}
};


const char* ExcessReleasePermitsException_msg = "Exception : An attempt was made to release permits greater than the current number of permits [in strict mode] held by the Thread id =";

struct ExcessReleasePermitsException : public std::exception
{
	const char * what () const throw ()
	{
		return(ExcessReleasePermitsException_msg);
	}
};


const char* IllegalPermitsReleaseException_msg = "Exception : An attempt was made to release permits in strict mode where it has no currently acquired permits.Please investigate. Thread id =";

struct IllegalPermitsReleaseException : public std::exception
{
	const char * what () const throw ()
	{
		return(IllegalPermitsReleaseException_msg);
	}
};


#endif
