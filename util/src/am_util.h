#ifndef __AM_UTIL_H__
#define __AM_UTIL_H__

class amString;

extern void msleep( int ms );
extern void readCinLine( amString &buffer, int bufferSize );

extern double getUnixTime( void );

extern bool isInterface( const amString &buffer );
extern bool isIPAddress( const amString &buffer );
extern bool isMulticastIPAddress( const amString &buffer );
extern bool isPortNumber( int portNo );

#endif // __AM_UTIL_H__

