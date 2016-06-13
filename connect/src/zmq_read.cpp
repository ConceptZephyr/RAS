#include <iostream>
#include <zmq.hpp>
#include <am_constants.h>
#include <am_split_string.h>
#include <am_util.h>

#include "zmq_read.h"

zmqRead::zmqRead
(
    void
)
{
    initialize();
}

zmqRead::zmqRead
(
    const amString &ipAddress,
    int             portNo,
    int             waitTime
)
{
    initialize();
    zmqConnectSUB( ipAddress, portNo, waitTime );
}

zmqRead::zmqRead
(
    const std::vector<amString> &ipAddresses,
    const std::vector<int>      &portNumbers,
    int                          waitTime
)
{
    initialize();
    zmqConnectMultiSUB( ipAddresses, portNumbers, waitTime );
}

void zmqRead::initialize
(
    void
)
{
    connectionUp  = false;
    errorCode     = 0;
    pause         = C_PAUSE;
    zmqBufferSize = C_ZMQ_BUFFER_SIZE;
    ctxPtr        = new zmq::context_t( 1 );
    zmqSocketPtr  = new zmq::socket_t( *ctxPtr, ZMQ_SUB );
    errorMessage.clear();
}

zmqRead::~zmqRead
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

int zmqRead::zmqConnectSUB
(
    const amString &ipAddress,
    int             portNo,
    int             waitTime
)
{
    amString zmqConnection;

    if ( ctxPtr == NULL )
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

    if ( errorCode == 0 )
    {
        try
        {
            zmqSocketPtr->connect( zmqConnection.c_str() );
        }
        catch ( zmq::error_t& e )
        {
            errorMessage += "zmqConnectSUB: Establishing ZMQ connection failed.\n    ";
            errorCode = E_ZMQ_CONNECT;
            errorMessage += e.what();
        }
    }

    try  
    {    
        zmqSocketPtr->setsockopt( ZMQ_SUBSCRIBE, "", 0 ); 
    }    
    catch ( zmq::error_t& e )
    {    
        errorMessage += "zmqConnectSUB: Set Socket Option 'ZMQ_SUBSCRIBE' failed.\n    ";
        errorCode = E_ZMQ_SUBSCRIBE_FAIL;
        errorMessage += e.what();
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
            errorCode = E_ZMQ_LINGER;
            errorMessage += "zmqConnectSUB: Setting option 'ZMQ_LINGER' failed.\n    ";
            errorMessage += e.what();
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int zmqRead::zmqConnectMultiSUB
(
    const std::vector<amString> &ipAddresses,
    const std::vector<int>      &portNumbers,
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
            errorCode = E_ZMQ_LINGER;
            errorMessage += e.what();
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int zmqRead::zmqClose
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
            errorCode = E_ZMQ_CLOSE;
            errorMessage += "Caught C++ exception of type or derived from 'zmq::error_t':\n    ";
            errorMessage += e.what();
        }
        catch ( ... )
        {
            errorCode = E_ZMQ_UNKNOWN;
            errorMessage += "Caught C++ exception. Unknown error.";
        }
    }
    connectionUp = false;

    return errorCode;
}

int zmqRead::zmqReadMessage
(
    amString &buffer,
    amString &errorMessage
)
{
    buffer.reserve( zmqBufferSize );
    errorMessage.reserve( C_MEDIUM_BUFFER_SIZE );
    int stringLength = zmqReadMessage( buffer, zmqBufferSize, errorMessage );
    return stringLength;
}

int zmqRead::zmqReadMessage
(
    amString &buffer,
    int       bufferSize,
    amString &errorMessage
)
{
    int stringLength  = 0;

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
        buffer.clear();
        if ( pause > 0 )
        {
            msleep( pause );
        }
        stringLength = zmqReadMessageBasic( buffer, bufferSize, errorMessage );
    }
    return stringLength;
}

int zmqRead::zmqReadMessageBasic
(
    amString &buffer,
    int       bufferSize,
    amString &errorMessage
)
{
    int  stringLength  = 0;
    bool success       = false;

    zmq::message_t message( bufferSize );
    memset( message.data(), 0, bufferSize );

    try
    {
        success = zmqSocketPtr->recv( &message, ZMQ_NOBLOCK );
        if ( success )
        {
            stringLength = strlen( ( const char * ) message.data() );
            buffer = ( const char * ) message.data();
            buffer[ bufferSize - 1 ] = 0;
        }
    }
    catch ( zmq::error_t& e )
    {
        errorCode     = E_ZMQ_RECEIVE;
        errorMessage += e.what();
    }
    catch ( ... )
    {
        errorCode     = E_ZMQ_RECEIVE;
        errorMessage += "ZMQ_READ: Unknown Error while reading.\n";
    }
    return stringLength;
}

int zmqRead::zmq2stream
(
    std::ostream &outStream
)
{
    if ( errorCode == 0 )
    {
        bool     running      = true;
        int      bufferSize   = 0;
        int      stringLength = 0;
        amString buffer;

        buffer.reserve( zmqBufferSize );

        if ( pause > 0 )
        {
            msleep( pause );
        }

        while ( running )
        {
            buffer.clear();
            stringLength = zmqReadMessageBasic( buffer, bufferSize, errorMessage );
            if ( stringLength > 0 )
            {
                if ( ( zmqExitString.size() > 0 ) && ( zmqExitString == buffer ) )
                {
                    running = false;
                }
                else
                {
                    outStream << buffer << std::endl;
                }
            }
        }
    }
    return errorCode;
}

