#include <iostream>

#include <am_constants.h>
#include <am_split_string.h>
#include <am_util.h>

#include <zmq.hpp>

#include "connect_util.h"
#include "zmq_hub_class.h"

zmqHub::zmqHub
(
    void
)
{
    initialize();
}

zmqHub::zmqHub
(
    const std::vector<amString> &inIPAddresses,
    const std::vector<int>      &inPortNumbers,
    const std::vector<amString> &outIPAddresses,
    const std::vector<int>      &outPortNumbers,
    int                          waitTime
)
{
    ctxPtr       = NULL;
    inSocketPtr  = NULL;
    outSocketPtr = NULL;

    initialize();
    zmqConnectHUB( inIPAddresses, inPortNumbers, outIPAddresses, outPortNumbers, waitTime );
}

zmqHub::~zmqHub
(
    void
)
{
    zmqClose();

    delete ctxPtr;
    ctxPtr = NULL;

    delete inSocketPtr;
    inSocketPtr = NULL;

    delete outSocketPtr;
    outSocketPtr = NULL;
}

void zmqHub::initialize
(
    void
)
{
    connectionUp  = false;
    errorCode     = 0;
    pause         = C_PAUSE;
    timePrecision = C_TIME_PRECISION;
    zmqBufferSize = C_ZMQ_BUFFER_SIZE;
    ctxPtr        = new zmq::context_t( 1 );
    inSocketPtr   = new zmq::socket_t( *ctxPtr, ZMQ_SUB );
    outSocketPtr  = new zmq::socket_t( *ctxPtr, ZMQ_PUB );
    errorMessage.clear();
}

int zmqHub::zmqConnectHUB
(
    const std::vector<amString> &inIPAddresses,
    const std::vector<int>      &inPortNumbers,
    const std::vector<amString> &outIPAddresses,
    const std::vector<int>      &outPortNumbers,
    int                          waitTime
)
{
    zmqConnectMultiSUB( inIPAddresses, inPortNumbers, waitTime );
    if ( errorCode == 0 )
    {
        zmqConnectMultiPUB( outIPAddresses, outPortNumbers, waitTime );
    }
    return errorCode;
}

int zmqHub::zmqConnectMultiSUB
(
    const std::vector<amString> &inIPAddresses,
    const std::vector<int>      &inPortNumbers,
    int                          waitTime
)
{
    amString              connectionString;
    std::vector<amString> zmqConnections;
    int                   counter       = 0;
    int                   nbConnections = 0;

    if ( ctxPtr == NULL )
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( inSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        if ( inIPAddresses.size() == 0 )
        {
            errorCode = E_ZMQ_NO_IP_ADDRESS;
        }
        else if ( inPortNumbers.size() == 0 )
        {
            errorCode = E_ZMQ_NO_PORT_NUMBER;
        }
    }

    if ( errorCode == 0 )
    {
        nbConnections = std::min( inIPAddresses.size(), inPortNumbers.size() );

        for ( counter = 0; ( errorCode == 0 ) && ( counter < nbConnections ); ++counter )
        {
            errorCode = verifyIPAddress( inIPAddresses[ counter ] );

            if ( errorCode == 0 )
            {
                errorCode = verifyPortNumber( inPortNumbers[ counter ] );
            }

            if ( errorCode == 0 )
            {
                connectionString = createTCPAddress( inIPAddresses[ counter ], inPortNumbers[ counter ] );
                zmqConnections.push_back( connectionString );
            }
        }

        if ( zmqConnections.size() == 0 )
        {
            errorCode = E_ZMQ_NO_CONNECTION;
        }
    }

    for ( counter = 0; ( errorCode == 0 ) && ( counter < nbConnections ); ++counter )
    {
        try
        {
            inSocketPtr->connect( zmqConnections[ counter ].c_str() );
        }
        catch ( zmq::error_t& e )
        {
            errorCode = E_ZMQ_CONNECT;
            errorMessage += e.what();
        }
    }

    if ( errorCode == 0 )
    {
        try
        {
            inSocketPtr->setsockopt( ZMQ_SUBSCRIBE, "", 0 );
        }
        catch ( zmq::error_t& e )
        {
            errorMessage += "zmqConnectMultiSUB: Set Socket Option 'ZMQ_SUBSCRIBE' failed.\n    ";
            errorCode = E_ZMQ_SUBSCRIBE_FAIL;
            errorMessage += e.what();
        }
    }

    if ( errorCode == 0 )
    {
        int linger = 0;
        try
        {
            inSocketPtr->setsockopt( ZMQ_LINGER, &linger, sizeof( linger ) );
            if ( waitTime > 0 )
            {
                msleep( waitTime );
            }
        }
        catch ( zmq::error_t& e )
        {
            errorCode = E_ZMQ_LINGER;
            errorMessage += e.what();
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int zmqHub::zmqConnectMultiPUB
(
    const std::vector<amString> &outIPAddresses,
    const std::vector<int>      &outPortNumbers,
    int                          waitTime
)
{
    int                   counter       = 0;
    int                   nbConnections = 0;
    amString              connectionString;
    std::vector<amString> zmqConnections;

    if ( ctxPtr == NULL )
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( outSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        if ( outIPAddresses.size() == 0 )
        {
            errorCode = E_ZMQ_NO_IP_ADDRESS;
        }
        else if ( outPortNumbers.size() == 0 )
        {
            errorCode = E_ZMQ_NO_PORT_NUMBER;
        }
    }

    if ( errorCode == 0 )
    {
        nbConnections = std::min( outIPAddresses.size(), outPortNumbers.size() );
        for ( counter = 0; ( errorCode == 0 ) && ( counter < nbConnections ); ++counter )
        {
            errorCode = verifyIPAddress( outIPAddresses[ counter ] );

            if ( errorCode == 0 )
            {
                errorCode = verifyPortNumber( outPortNumbers[ counter ] );
            }

            if ( errorCode == 0 )
            {
                connectionString = createTCPAddress( outIPAddresses[ counter ], outPortNumbers[ counter ] );
                zmqConnections.push_back( connectionString );
            }
        }

        if ( zmqConnections.size() == 0 )
        {
            errorCode = E_ZMQ_NO_CONNECTION;
        }
    }

    for ( counter = 0; ( errorCode == 0 ) && ( counter < nbConnections ); ++counter )
    {
        try
        {
            outSocketPtr->bind( zmqConnections[ counter ].c_str() );
        }
        catch ( zmq::error_t& e )
        {
            errorCode = E_ZMQ_BIND;
            errorMessage += e.what();
        }

        try
        {
            outSocketPtr->connect( zmqConnections[ counter ].c_str() );
        }
        catch ( zmq::error_t& e )
        {
            errorCode = E_ZMQ_CONNECT;
            errorMessage += e.what();
        }
    }

    if ( errorCode == 0 )
    {
        int linger = 0;
        try
        {
            outSocketPtr->setsockopt( ZMQ_LINGER, &linger, sizeof( linger ) );
            if ( waitTime > 0 )
            {
                msleep( waitTime );
            }
        }
        catch ( zmq::error_t& e )
        {
            errorCode     = E_ZMQ_LINGER;
            errorMessage += e.what();
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int zmqHub::run
(
    void
)
{
    bool           running = false;
    zmq::message_t message( zmqBufferSize + 1 );

    if ( ( errorCode == 0 ) && !connectionUp )
    {   
        errorCode = E_ZMQ_NO_CONNECTION;
    }   

    if ( ( errorCode == 0 ) && ( ctxPtr == NULL ) ) 
    {   
        errorCode = E_ZMQ_NO_CONTEXT;
    }   

    if ( ( errorCode == 0 ) && ( inSocketPtr == NULL ) ) 
    {   
        errorCode = E_ZMQ_NO_SOCKET;
    }   

    if ( ( errorCode == 0 ) && ( outSocketPtr == NULL ) ) 
    {   
        errorCode = E_ZMQ_NO_SOCKET;
    }   

    running = ( errorCode == 0 );

    if ( pause > 0 )
    {
        msleep( pause );
    }

    while ( running )
    {
        try
        {
            inSocketPtr->recv( &message, ZMQ_NOBLOCK );
        }
        catch ( zmq::error_t& e )
        {
            errorCode     = E_ZMQ_RECEIVE;
            running       = false;
            errorMessage += "ZMQ_HUB: ";
            errorMessage += e.what();
        }
        catch ( ... )
        {
            running       = false;
            errorCode     = E_ZMQ_RECEIVE;
            errorMessage += "ZMQ_HUB: Unknown Error while reading.\n";
        }

        if ( running )
        {
            if ( strlen( ( const char * ) message.data() ) > 0 )
            {
                // Don't send empty messages
                if ( ( zmqExitString.size() > 0 ) && ( zmqExitString == ( const char * ) message.data() ) )
                {
                    running = false;
                }
                else
                {
                    if ( timeStampMode != KEEP_AS_IS )
                    {
                        double currentTime = getUnixTime();
                        if ( timeStampMode == OVERWRITE )
                        {
                            overwriteTimeStamp( message, currentTime );
                        }
                        if ( timeStampMode == APPEND )
                        {
                            appendTimeStamp( message, currentTime );
                        }
                    }

                    try
                    {
                        outSocketPtr->send( message );
                    }
                    catch ( zmq::error_t &e )
                    {
                        errorCode     = E_ZMQ_SEND;
                        running       = false;
                        errorMessage += "ZMQ_HUB: ";
                        errorMessage += e.what();
                    }
                    catch ( ... )
                    {
                        running       = false;
                        errorCode     = E_ZMQ_SEND;
                        errorMessage += "ZMQ_HUB: Unknown Error while writing.\n";
                    }
                }
            }
        }
    }

    return errorCode;
}

void zmqHub::overwriteTimeStamp
(
    zmq::message_t &message,
    double          curTime
)
{
    // --------------------------------------------------------------
    //
    // ANT+ message format:
    //
    //    DEVICE_ID        TIMESTAMP       DATA
    //
    //   frontOfMessage    timeStamp       backOfMessage
    //
    // --------------------------------------------------------------
    amSplitString words;
    int nbWords = words.split( ( const char * ) message.data() );

    bzero( message.data(), message.size() );
    if ( nbWords > 2 )
    {
        amString fullMessage = words[ 0 ];
        fullMessage += C_SEPARATOR_AS_STRING;
        fullMessage += amString( curTime, timePrecision );
        for ( size_t counter = 2; counter < words.size(); ++counter )
        {
            fullMessage += C_SEPARATOR_AS_STRING;
            fullMessage += words[ counter ];
        }
        memcpy( message.data(), fullMessage.c_str(), fullMessage.size() );
    }
}

void zmqHub::appendTimeStamp
(
    zmq::message_t &message,
    double          curTime
)
{
    if ( *( ( const char * ) message.data() ) != 0 )
    {
        amString fullMessage( ( const char * ) message.data() );
        fullMessage += C_SEPARATOR_AS_STRING;
        fullMessage += amString( curTime, timePrecision );
        memcpy( message.data(), fullMessage.c_str(), fullMessage.size() );
    }
}

int zmqHub::zmqClose
(
    void
)
{
    if ( ctxPtr != NULL )
    {
        if ( inSocketPtr != NULL )
        {
            try
            {
                inSocketPtr->close();
            }
            catch ( zmq::error_t &e )
            {
                errorCode     = E_ZMQ_CLOSE;
                errorMessage += "Caught C++ exception of type or derived from 'zmq::error_t':\n    ";
                errorMessage += e.what();
            }
            catch ( ... )
            {
                errorCode     = E_ZMQ_UNKNOWN;
                errorMessage += "Caught C++ exception. Unknown error.";
            }
        }

        if ( outSocketPtr != NULL )
        {
            try
            {
                outSocketPtr->close();
            }
            catch ( zmq::error_t &e )
            {
                errorCode     = E_ZMQ_CLOSE;
                errorMessage += "Caught C++ exception of type or derived from 'zmq::error_t':\n    ";
                errorMessage += e.what();
            }
            catch ( ... )
            {
                errorCode     = E_ZMQ_UNKNOWN;
                errorMessage += "Caught C++ exception. Unknown error.";
            }
        }
    }

    connectionUp = false;

    return errorCode;
}

