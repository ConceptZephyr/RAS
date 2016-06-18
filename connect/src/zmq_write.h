#ifndef __ZMQ_WRITE_H__
#define __ZMQ_WRITE_H__

#include <vector>
#include <zmq.hpp>

#include <am_string.h>
#include "connect_util.h"

class vector;

class zmqWrite
{
    public:

        zmqWrite( void );
        zmqWrite( const amString &ipAddress, int portNo, int waitTime = C_ZMQ_WAIT );
        zmqWrite( const std::vector<amString> &ipAddresses, const std::vector<int> &portNumbers, int waitTime = C_ZMQ_WAIT );
        ~zmqWrite();

        int zmqConnectPUB( const amString &ipAddress, int portNo, int waitTime = C_ZMQ_WAIT );
        int zmqConnectMultiPUB( const std::vector<amString> &ipAddresses, const std::vector<int> &portNumbers, int waitTime = C_ZMQ_WAIT );
        int zmqWriteMessage( const amString &outputString );
        int zmqClose( void );
        int stream2zmq( std::istream &inStream );
    
        void setOutputExitString( bool value ) { outputExitString = value; }
        void setExitString( const amString &value ) { exitString = value; }
        amString getExitString( void ) const { return exitString; }

        void setErrorCode( int value ) { errorCode = value; }
        int getErrorCode( void ) const { return errorCode; }

        void setErrorMessage( const amString &value ) { errorMessage = value; }
        amString getErrorMessage( void ) const { return errorMessage; }

        void setPause( int value ) { pause = value; }
        int getPause( void ) const { return pause; }

        void setDebugOut( int value ) { debugOut = value; }
        int getDebugOut( void ) const { return debugOut; }

        void setDiagnostics( bool value ) { diagnostics = value; }
        bool getDiagnostics( void ) const { return diagnostics; }

        void setZMQBufferSize( int value ) { zmqBufferSize = value; }
        int getZMQBufferSize( void ) const { return zmqBufferSize; }

        void setTimePrecision( size_t value ) { timePrecision = value; }
        size_t getTimePrecision( void ) const { return timePrecision; }

        void setTimeStampMode( amTimeStampMode value ) { timeStampMode = value; }
        amTimeStampMode getTimeStampMode( void ) const { return timeStampMode; }
 
        inline bool isUp( void ) const { return connectionUp; }

    private:
    
        zmq::socket_t  *zmqSocketPtr;
        zmq::context_t *ctxPtr;

        amString exitString;
        amString errorMessage;
        int      errorCode;
        int      pause;
        bool     diagnostics;
        bool     connectionUp;
        bool     debugOut;
        bool     outputExitString;
        size_t   zmqBufferSize;
        int      timePrecision;

        amTimeStampMode timeStampMode;

        void overwriteTimeStamp( zmq::message_t &message, double curTime );
        void appendTimeStamp   ( zmq::message_t &message, double curTime );

        void initialize( void );

};

#endif  // __ZMQ_WRITE_H__

