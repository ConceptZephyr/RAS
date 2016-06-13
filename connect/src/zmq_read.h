#ifndef __ZMQ_READ_H__
#define __ZMQ_READ_H__

#include <zmq.hpp>
#include <vector>

#include <am_string.h>

#include "connect_util.h"

class zmqRead
{

    public:

        zmqRead( void );
        zmqRead( const amString &zmqIPAddress, int zmqPortNo, int waitTime = C_ZMQ_WAIT );
        zmqRead( const std::vector<amString> &ipAddresses, const std::vector<int> &portNumbers, int waitTime = C_ZMQ_WAIT );
        ~zmqRead();

        int zmqReadMessage( amString &buffer, int bufferSize, amString &errorMessage );
        int zmqReadMessage( amString &buffer,                 amString &errorMessage );
        int zmqConnectSUB( const amString &zmqIPAddress, int zmqPortNo, int waitTime = C_ZMQ_WAIT );
        int zmqConnectMultiSUB( const std::vector<amString> &ipAddresses, const std::vector<int> &portNumbers, int waitTime = C_ZMQ_WAIT );
        int zmqClose( void );
        int zmq2stream( std::ostream &outStream );

        void setZMQExitString( const amString &value ) { zmqExitString = value; }
        amString getZMQExitString( void ) const { return zmqExitString; }

        void setErrorCode( int value ) { errorCode = value; }
        int getErrorCode( void ) const { return errorCode; }

        void setErrorMessage( const amString &value ) { errorMessage = value; }
        amString getErrorMessage( void ) const { return errorMessage; }

        void setPause( int value ) { pause = value; }
        int getPause( void ) const { return pause; }

        void setDiagnostics( bool value ) { diagnostics = value; }
        bool getDiagnostics( void ) const { return diagnostics; }

        void setZMQBufferSize( int value ) { zmqBufferSize = value; }
        int getZMQBufferSize( void ) const { return zmqBufferSize; }

        inline bool isUp( void ) const { return connectionUp; }

 
    private:

        zmq::socket_t  *zmqSocketPtr;
        zmq::context_t *ctxPtr;

        amString zmqExitString;
        amString errorMessage;
        bool     diagnostics;
        bool     connectionUp;
        int      errorCode;
        int      pause;
        size_t   zmqBufferSize;

        void initialize( void );

        int zmqReadMessageBasic( amString &buffer, int bufferSize, amString &errorMessage );

};

#endif // __ZMQ_READ_H__

