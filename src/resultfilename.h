#pragma once

#include "machine.h"

#ifndef MACHINE
#define MACHINE "unknown"
#endif

#if defined(_WIN64)
#	define OS "win64"
#elif defined(_WIN32)
#	define OS "win32"
#elif defined(__CYGWIN__) && defined(__x86_64)
#	define OS "cygwin64"
#elif defined(__CYGWIN__)
#	define OS "cygwin32"
#elif defined(__APPLE__)
#	include "TargetConditionals.h"
#	if TARGET_OS_IPHONE
#     	ifdef __LP64__
#			define OS "ios64"
#		else
#			define OS "ios32"
#		endif
#	elif TARGET_OS_MAC
#       ifdef __amd64__
#	    	define OS "mac64"
#		else
#			define OS "mac32"
#		endif
#   endif
#elif defined(__linux__) || defined(__linux) || defined(linux)
#   if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__)
#       define OS "linux64"
#   elif defined(__i386__) || defined(__i386)
#       define OS "linux32"
#   else
#       define OS "linux"
#   endif
#endif

#ifndef OS
#define OS "unknown"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#if defined(_MSC_VER)
#	if _MSC_VER >= 1920
#		define COMPILER "vc2019"
#	elif _MSC_VER >= 1910
#		define COMPILER "vc2017"
#	elif _MSC_VER >= 1900
#		define COMPILER "vc2015"
#	elif _MSC_VER >= 1800
#		define COMPILER "vc2013"
#	elif _MSC_VER >= 1700
#		define COMPILER "vc2012"
#	elif _MSC_VER >= 1600
#		define COMPILER "vc2010"
#	elif _MSC_VER >= 1500
#		define COMPILER "vc2008"
#	elif _MSC_VER >= 1400
#		define COMPILER "vc2005"
#   else
#		define COMPILER "vc"
#	endif
#elif defined(__clang__)
#	define COMPILER "clang" STR(__clang_major__) "." STR(__clang_minor__)
#elif defined(__GNUC__)
#	define COMPILER "gcc" STR(__GNUC__) "." STR(__GNUC_MINOR__)
#else
#	define COMPILER "unknown"
#endif

#define RESULT_FILENAME MACHINE "_" OS "_" COMPILER ".csv"
