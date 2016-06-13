#ifndef __AM_UTIL_H__
#define __AM_UTIL_H__

class amString;


extern void msleep( int ms );
extern void readCinLine ( amString &buffer, int bufferSize );

extern double getUnixTime( void );

#endif // __AM_UTIL_H__

