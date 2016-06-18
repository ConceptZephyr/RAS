#include <iostream>
#include <zmq.hpp>
#include <am_constants.h>
#include <am_split_string.h>
#include <am_util.h>

#include "zmq_write.h"

zmqWrite::zmqWrite
(
    void
)
{
    initialize();
}

zmqWrite::zmqWrite
(
    const amString &ipAddress,
    int             portNo,
    int             waitTime
)
{
    initialize();
    zmqConnectPUB( ipAddress, portNo, waitTime );
}

zmqWrite::zmqWrite
(
    const std::vector<amString> &ipAddresses,
    const std::vector<int>      &portNumbers,
    int                          waitTime
)
{
    initialize();
    zmqConnectMultiPUB( ipAddresses, portNumbers, waitTime );
}

zmqWrite::~zmqWrite
(
    void
)
{
    zmqClose();

    delete ctxPtr;
    ctxPtr = NULL;

    delete zmqSocketPtr;
    zmqSocketPtr = NULL;
}

void zmqWrite::initialize
(
    void
)
{
    connectionUp     = false;
    debugOut         = false;
    errorCode        = 0;
    pause            = C_PAUSE;
    zmqBufferSize    = C_ZMQ_BUFFER_SIZE;
    timePrecision    = C_TIME_PRECISION;
    outputExitString = false;
    ctxPtr           = new zmq::context_t( 1 );
    zmqSocketPtr     = new zmq::socket_t( *ctxPtr, ZMQ_PUB );
    errorMessage.clear();
}

int zmqWrite::zmqConnectPUB
(
    const amString &ipAddress,
    int             portNo,
    int             waitTime
)
{
    amString zmqConnection;

    if ( ( errorCode == 0 ) && ( ctxPtr == NULL ) ) 
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( zmqSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        errorCode = verifyIPAddress( ipAddress );
    }

    if ( errorCode == 0 )
    {
        errorCode = verifyPortNumber( portNo );
    }

    if ( errorCode == 0 )
    {
        zmqConnection = createTCPAddress( ipAddress, portNo );
    }

    try
    {
        zmqSocketPtr->bind( zmqConnection.c_str() );
    }
    catch ( zmq::error_t& e )
    {
        errorCode     = E_ZMQ_BIND;
        errorMessage += "ZMQ Binding of Socket Failed.\n    ";
        errorMessage += e.what();
    }


    if ( errorCode == 0 )
    {
        try
        {
            zmqSocketPtr->connect( zmqConnection.c_str() );
            if ( waitTime > 0 )
            {
                msleep( waitTime );
            }
        }
        catch ( zmq::error_t& e )
        {
            errorCode     = E_ZMQ_CONNECT;
            errorMessage += "ZMQ Connection Failed.\n    ";
            errorMessage += e.what();
        }
    }

    if ( errorCode == 0 )
    {
        int linger = 0;
        try
        {
            zmqSocketPtr->setsockopt( ZMQ_LINGER, &linger, sizeof( linger ) );
            if ( waitTime > 0 )
            {
                msleep( waitTime );
            }
        }
        catch ( zmq::error_t& e )
        {
            errorCode     = E_ZMQ_LINGER;
            errorMessage += "ZMQ Set Option 'ZMQ_LINGER' Failed.\n    ";
            errorMessage += e.what();
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int zmqWrite::zmqConnectMultiPUB
(
    const std::vector<amString> &ipAddresses,
    const std::vector<int>      &portNumbers,
    int                          waitTime
)
{
    int                   counter       = 0;
    int                   nbConnections = 0;
    amString              connectionString;
    std::vector<amString> zmqConnections;

    if ( ( errorCode == 0 ) && ( ctxPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( zmqSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        if ( ipAddresses.size() == 0 )
        {
            errorCode = E_ZMQ_NO_IP_ADDRESS;
        }
        else if ( portNumbers.size() == 0 )
        {
            errorCode = E_ZMQ_NO_PORT_NUMBER;
        }
    }

    if ( errorCode == 0 )
    {
        nbConnections = std::min( ipAddresses.size(), portNumbers.size() );
        for ( counter = 0; ( errorCode == 0 ) && ( counter < nbConnections ); ++counter )
        {
            errorCode = verifyIPAddress( ipAddresses[ counter ] );

            if ( errorCode == 0 )
            {
                errorCode = verifyPortNumber( portNumbers[ counter ] );
            }

            if ( errorCode == 0 )
            {
                connectionString = createTCPAddress( ipAddresses[ counter ], portNumbers[ counter ] );
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
            zmqSocketPtr->bind( zmqConnections[ counter ].c_str() );
        }
        catch ( zmq::error_t& e )
        {
            errorCode     = E_ZMQ_BIND;
            errorMessage += "ZMQ Binding of Socket Failed.\n    ";
            errorMessage += e.what();
        }
        try
        {
            zmqSocketPtr->connect( zmqConnections[ counter ].c_str() );
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
            zmqSocketPtr->setsockopt( ZMQ_LINGER, &linger, sizeof( linger ) );
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

int zmqWrite::zmqClose
(
    void
)
{
    if ( ( ctxPtr != NULL ) && ( zmqSocketPtr != NULL ) )
    {
        try
        {
            zmqSocketPtr->close();
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
    connectionUp = false;

    return errorCode;
}

void zmqWrite::overwriteTimeStamp
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

void zmqWrite::appendTimeStamp
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

int zmqWrite::zmqWriteMessage
(
    const amString &outputString
)
{
    if ( ( errorCode == 0 ) && !connectionUp )
    {
        errorCode = E_ZMQ_NO_CONNECTION;
    }

    if ( ( errorCode == 0 ) && ( ctxPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( zmqSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        int length = zmqBufferSize;
        if ( length < outputString.size() + 2 )
        {
            length = outputString.size() + 2;
        }
        zmq::message_t message( length );
        memset( ( void * ) message.data(), 0, length - 1 );
        memcpy( message.data(), ( void * ) outputString.c_str(), length - 2 );

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

        if ( debugOut )
        {
            std::cerr << "ZMQ SEND: "<< ( ( const char * ) message.data() ) << std::endl;
        }

        try
        {
            zmqSocketPtr->send( message );
        }
        catch ( zmq::error_t &e )
        {
            errorCode = E_ZMQ_SEND;
            errorMessage += "Caught C++ exception of type or derived from 'zmq::error_t':\n    ";
            errorMessage += e.what();
        }
    }
    return errorCode;
}

int zmqWrite::stream2zmq
(
    std::istream &inStream
)
{
    amString      message;
    bool          running = true;

    if ( ( errorCode == 0 ) && !connectionUp )
    {
        errorCode = E_ZMQ_NO_CONNECTION;
    }

    if ( ( errorCode == 0 ) && ( ctxPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_CONTEXT;
    }

    if ( ( errorCode == 0 ) && ( zmqSocketPtr == NULL ) )
    {
        errorCode = E_ZMQ_NO_SOCKET;
    }

    if ( errorCode == 0 )
    {
        if ( !inStream.good() )
        {
            errorCode = E_READ_FILE_NOT_OPEN;
        }
    }

    if ( errorCode == 0 )
    {
        message.reserve( zmqBufferSize );

        while ( running )
        {
            if ( inStream.eof() || inStream.fail() )
            {
                running = false;
                if ( ( exitString.size() > 0 ) && outputExitString )
                {
                    try
                    {
                        zmqWriteMessage( exitString );
                    }
                    catch ( zmq::error_t &e )
                    {
                        running = false;
                    }
                }
            }
            else
            {
                std::getline( inStream, message );
                if ( ( debugOut > 1 ) && ( debugOut & 1 ) )
                {
                    std::cerr << std::endl << "READ " << message << std::endl;
                }
                if ( message.size() > 0 )
                {
                    if ( message.back() == '\r' )
                    {
                        message.back() = 0;
                    }

                    if ( ( exitString.size() > 0 ) && ( message == exitString ) )
                    {
                        running = false;
                    }

                    if ( running || outputExitString )
                    {
                        if ( ( debugOut > 1 ) && ( ( debugOut & 1 ) == 0 ) )
                        {
                            std::cerr << "SEND " << message << std::endl;
                        }
                        try
                        {
                            zmqWriteMessage( message );
                        }
                        catch ( zmq::error_t &e )
                        {
                            running = false;
                        }

                        if ( pause > 0 )
                        {
                            msleep( pause );
                        }
                    }
                }
            }
        }
    }

    if ( debugOut > 1 )
    {
        std::cerr << std::endl << "EXIT" << std::endl;
    }
    return errorCode;
}

