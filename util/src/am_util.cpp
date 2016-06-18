#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>

#include "am_constants.h"
#include "am_string.h"
#include "am_split_string.h"
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

bool isInterface
(
    const amString &buffer
)
{
    bool result      = false;
    int  interfaceNo = -1;

    if ( buffer == C_AUTO_INTERFACE )
    {
        result = true;
    }

    if ( !result )
    {
#       ifdef __MAC_OS__
std::cerr << "MAC interface = " << buffer << std::endl;
        if ( ( buffer.size() == 3 ) && ( buffer[ 0 ] == 'e' ) && ( buffer[ 1 ] == 'n' ) )
        {
            interfaceNo = buffer[ 2 ] - '0';
        }
#       else // __MAC_OS__
std::cerr << "MO MAC interface = " << buffer << std::endl;
        if ( ( buffer.size() == 5 ) && ( buffer[ 0 ] == 'w' ) && ( buffer[ 1 ] == 'l' ) && ( buffer[ 2 ] == 'a' ) && ( buffer[ 3 ] == 'n' ) )
        {
            interfaceNo = buffer[ 4 ] - '0';
        }
#       endif // __MAC_OS__

        if ( ( interfaceNo >= 0 ) && ( interfaceNo <= 9 ) )
        {
            result = true;
        }
    }

    return result;
}

bool isIPAddress
(
    const amString &buffer
)
{
    bool result = false;

    if ( buffer.size() >= 7 )
    {
        // Need at least 7 letters "1.1.1.1"
        amSplitString words;
        words.removeAllSeparators();
        words.split( buffer, "." );
        if ( words.size() == 4 )
        {
            result = true;
            for ( int counter = 0; result && ( counter < words.size() ); ++counter )
            {
                int subNumber = words[ counter ].toInt();
                if ( ( subNumber < 0 ) || ( subNumber > 255 ) || ( ( subNumber == 0 ) && ( counter == 0 ) ) )
                {
                    result = false;
                }
            }
        }
    }
    return result;
}

bool isMulticastIPAddress
(
    const amString &buffer
)
{
    bool result = false;

    if ( buffer.size() >= 7 )
    {
        // Need at least 7 letters "1.1.1.1"
        amSplitString words;
        words.removeAllSeparators();
        words.split( buffer, "." );
        if ( words.size() == 4 )
        {
            result = true;
            int counter   = 0;
            int subNumber = words[ counter ].toInt();
            if ( ( subNumber < 224 ) || ( subNumber > 239 ) )
            {
                result = false;
            }
            for ( ++counter; result && ( counter < words.size() ); ++counter )
            {
                subNumber = words[ counter ].toInt();
                if ( ( subNumber < 0 ) || ( subNumber > 255 ) )
                {
                    result = false;
                }
            }
        }
    }
    return result;
}

bool isPortNumber
(
    int portNo
)
{
    bool result = ( portNo > 0 ) && ( portNo < 65536 );
    return result;
}


