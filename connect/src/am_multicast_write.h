#ifndef __AM_MULTI_CAST_WRITE_H__
#define __AM_MULTI_CAST_WRITE_H__

#include <netdb.h>

class amString;

class amMulticastWrite
{

    private:

        int                diagnostics;
        int                debugOut;
        int                pause;
        int                bufferSize;
        int                socketID;
        bool               connectionUp;
        struct sockaddr_in groupSock;
        amString           ipAddress;
        int                portNo;
        int                errorCode;
        amString           errorMessage;
        amString           exitString;

        int                connect( void );


    public:

        amMulticastWrite( void );
        amMulticastWrite( const amString &curIPAddress, int curPortNo );
        ~amMulticastWrite( void ) {};

        int connect( const amString &curIPAddress, int curPortNo );

        inline bool     isUp           ( void ) const { return connectionUp; }
        inline amString getErrorMessage( void ) const { return errorMessage; }
        inline int      getErrorCode   ( void ) const { return errorCode; }

        inline void setDebugOut   ( int value ) { debugOut = value; }
        inline void setPause      ( int value ) { pause = value; }
        inline void setBufferSize ( int value ) { bufferSize = value; }
        inline void setDiagnostics( int value ) { diagnostics = value; }
        inline void setExitString ( const amString &value ) { exitString = value; }

        void   close( void );
        size_t write( const amString &buffer, int length = -1 );
        int    stream2mc( std::istream &inStream );

};

#endif // __AM_MULTI_CAST_WRITE_H__

