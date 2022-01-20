#pragma once

// redefine this to a value that makes sense for your platform
#ifndef FTH_UNDEFINED
#	define FTH_UNDEFINED 0xCDCDCDCD
#endif

// Firth is case insensitive by default
#ifndef FTH_CASE_SENSITIVE
#	define FTH_CASE_SENSITIVE 0
#endif

// Set this to zero to remove all float support
#ifndef FTH_INCLUDE_FLOAT
#	define FTH_INCLUDE_FLOAT 1
#endif

const int FTH_MAX_WORD_NAME = 256;
const int FTH_DEFAULT_DATA_SEGMENT_SIZE = 1024;
const int FTH_MAX_PRINTF_SIZE = 512;

typedef int FirthNumber;

#if FTH_INCLUDE_FLOAT == 1
	typedef float FirthFloat;
#endif

const FirthFloat FTH_EPSILON = 1e-5;
