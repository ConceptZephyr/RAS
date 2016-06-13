int zmqClose( void );
#ifndef __ZMQ_HUB_CLASS_H__
#define __ZMQ_HUB_CLASS_H__

#include <vector>
#include <zmq.hpp>

#include <am_string.h>

class vector;

class zmqHub
{
    public:

        zmqHub( void );
        zmqHub
        (
            const std::vector<amString> &inIPAddresses,
            const std::vector<int>      &inPortNumbers,
            const std::vector<amString> &outIPAddresses,
            const std::vector<int>      &outPortNumbers,
            int                          waitTime = C_ZMQ_WAIT
        );
        ~zmqHub();

        int zmqConnectHUB
            (
                const std::vector<amString> &inIPAddresses,
                const std::vector<int>      &inPortNumbers,
                const std::vector<amString> &outIPAddresses,
                const std::vector<int>      &outPortNumbers,
                int                          waitTime = C_ZMQ_WAIT
            );

        int zmqClose( void );
        int run( void );
    
        void setZMQExitString( const amString &value ) { zmqExitString = value; }
        amString getZMQExitString( void ) const { return zmqExitString; }

        void setErrorCode( int value ) { errorCode = value; }
        int getErrorCode( void ) const { return errorCode; }

        void setErrorMessage( const amString &value ) { errorMessage = value; }
        amString getErrorMessage( void ) const { return errorMessage; }

        void setPause( int value ) { pause = value; }
        int getPause( void ) const { return pause; }

        void setTimePrecision( size_t value ) { timePrecision = value; }
        size_t getTimePrecision( void ) const { return timePrecision; }

        void setDiagnostics( bool value ) { diagnostics = value; }
        bool getDiagnostics( void ) const { return diagnostics; }

        void setZMQBufferSize( int value ) { zmqBufferSize = value; }
        int getZMQBufferSize( void ) const { return zmqBufferSize; }

        void setTimeStampMode( amTimeStampMode value ) { timeStampMode = value; }
        amTimeStampMode getTimeStampMode( void ) const { return timeStampMode; }


    private:
    
        zmq::context_t *ctxPtr;
        zmq::socket_t  *inSocketPtr;
        zmq::socket_t  *outSocketPtr;

        amString zmqExitString;
        amString errorMessage;
        int      errorCode;
        int      pause;
        bool     diagnostics;
        bool     connectionUp;
        size_t   timePrecision;
        size_t   zmqBufferSize;

        amTimeStampMode timeStampMode;

        void initialize( void );

        int zmqConnectMultiSUB( const std::vector<amString> &inIPAddresses,  const std::vector<int> &inPortNumbers,  int waitTime );
        int zmqConnectMultiPUB( const std::vector<amString> &outIPAddresses, const std::vector<int> &outPortNumbers, int waitTime );

        void overwriteTimeStamp( zmq::message_t &message, double curTime );
        void appendTimeStamp   ( zmq::message_t &message, double curTime );

};

#endif  // __ZMQ_HUB_CLASS_H__

