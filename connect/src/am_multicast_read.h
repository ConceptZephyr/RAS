#ifndef __AM_MULTI_CAST_READ_H__
#define __AM_MULTI_CAST_READ_H__

#include <am_string.h>
#include "connect_util.h"

class amMulticastRead
{

    private:

        bool     connectionUp;
        bool     binary;
        int      debugOut;
        int      diagnostics;
        int      socketID;
        int      bufferSize;
        int      errorCode;
        amString interface;
        amString ipAddress;
        amString errorMessage;
        amString exitString;
        int      portNo;
        int      timeOut;

        bool determineIPAddress( amString &ipAddress, const amString &interface );
        void initialize( void );


    public:

        amMulticastRead( void );
        amMulticastRead( const amString &curInterface, const amString &curIpAddress, int curPortNo, int curTimeOut = C_TIME_OUT_SEC_DEFAULT );
        ~amMulticastRead( void ) {};

        inline bool isUp( void ) const { return connectionUp; }

        inline void setBinary     ( bool value ) { binary  = value; }
        inline void setBufferSize ( int value ) { bufferSize  = value; }
        inline void setDebugOut   ( int value ) { debugOut    = value; }
        inline void setDiagnostics( int value ) { diagnostics = value; }
        inline void setExitString ( const amString &value ) { exitString = value; }

        void close( void );
        void mc2Stream( std::ostream &outStream );

        size_t read( unsigned char *curBuffer, size_t curBufferSize );
        int connect( const amString &curInterface, const amString &curIpAddress, int curPortNo, int curTimeOut = C_TIME_OUT_SEC_DEFAULT );
        int connect( void );

        inline amString getErrorMessage( void ) const { return errorMessage; }
        inline int      getErrorCode   ( void ) const { return errorCode;    }

};

#endif // __AM_MULTI_CAST_READ_H__

