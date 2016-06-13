#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>

#include "am_string.h"
#include "am_util.h"


void msleep
(
    int ms
)
{
    struct timespec time;
    time.tv_sec  = ms / 1000;
    time.tv_nsec = ( ms - 1000 * time.tv_sec ) * ( 1000 * 1000 );
    nanosleep( &time, NULL );
}

void readCinLine
(
    amString &buffer,
    int       bufferSize
)
{
    buffer.reserve( bufferSize );
    std::getline( std::cin, buffer );
}


// ------------------------------------------------------------------------------------------------------
//
// Get the number of elapsed seconds since 1970 (with micro-second presision).
//
// ------------------------------------------------------------------------------------------------------
double getUnixTime
(
    void 
)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    double result = ( double ) tv.tv_sec + ( ( double ) tv.tv_usec ) / 1.0E6;

    return result;
}


