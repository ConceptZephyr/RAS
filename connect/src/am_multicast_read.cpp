#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <new>

#include <am_constants.h>
#include <am_util.h>

#include "am_multicast_read.h"

amMulticastRead::amMulticastRead
(
    void
)
{
    timeOut = C_TIME_OUT_SEC_DEFAULT;
    initialize();
}

amMulticastRead::amMulticastRead
(
    const amString &curInterface,
    const amString &curIPAddress,
    int             curPortNo,
    int             curTimeOut
) : interface( curInterface ),
    ipAddress( curIPAddress ),
    portNo( curPortNo ),
    timeOut( curTimeOut )
{
    initialize();
    connect();
}

void amMulticastRead::initialize
(
    void
)
{
    connectionUp     = false;
    outputExitString = false;
    binary           = false;
    socketID         = -1;
    errorCode        = 0;
    errorMessage     = "";
}

void amMulticastRead::close
(
    void
)
{
    if ( connectionUp )
    {
        if ( socketID > 0 )
        {
            ::close( socketID );
            socketID = -1;
        }
        connectionUp = false;
    }
}

size_t amMulticastRead::read
(
    BYTE   *curBuffer,
    size_t  curBufferSize
)
{
    size_t nbBytes = 0;
    if ( connectionUp && ( socketID > 0 ) && ( curBufferSize > 0 ) && ( curBuffer != NULL ) )
    {
        bzero( curBuffer, curBufferSize );
        nbBytes = ::read( socketID, ( void * ) curBuffer, curBufferSize );
    }
    return nbBytes;
}

bool amMulticastRead::determineIPAddress
(
    amString       &ipAddress,
    const amString &interface
)
{
    bool success = false;
    char szBuffer[ C_BUFFER_SIZE ];

    if ( gethostname( szBuffer, sizeof( szBuffer ) ) == 0 )
    {
        struct hostent *host = gethostbyname( szBuffer );
        if ( host != NULL )
        {
            if ( interface.empty() )
            {
                ipAddress = C_LOOP_BACK_ADDRESS;
            }
            else
            {
                ipAddress = inet_ntoa( *( ( struct in_addr * ) host->h_addr ) );
                int ii = 0;
                while ( ipAddress.startsWith( C_LOOP_BACK_START ) && ( host->h_addr_list[ ii ] != NULL ) )
                {
                    ipAddress = inet_ntoa( *( ( struct in_addr * ) host->h_addr_list[ ii ] ) );
                    ++ii;
                }
            }

            if ( ipAddress.startsWith( C_LOOP_BACK_START ) )
            {
                if ( interface.empty() )
                {
                    sprintf( szBuffer, "ifconfig %s | grep \"inet \"", interface.c_str() );
                }
                else
                {
                    strcpy( szBuffer, "ifconfig | grep \"inet \"" );
                }
                FILE *pPtr = popen( szBuffer, "r" );

                if ( pPtr != NULL )
                {
                    while ( fgets( szBuffer, C_BUFFER_SIZE, pPtr ) )
                    {
                        char *bPtr = szBuffer;
                        while ( ( *bPtr != 0 ) && ( *bPtr != 'i' ) )
                        {
                            ++bPtr;
                        }
                        if ( *bPtr == 'i' )
                        {
                            ++bPtr;
                            while ( ( *bPtr != 0 ) && ( *bPtr != ' ' ) )
                            {
                                ++bPtr;
                            }
                            while ( *bPtr == ' ' )
                            {
                                ++bPtr;
                            }
                            while ( ( *bPtr != 0 ) && ( ( *bPtr < '1' ) || ( *bPtr > '9' ) ) )
                            {
                                ++bPtr;
                            }
                            ipAddress = bPtr;

                            size_t count = 0;
                            while ( ( ipAddress[ count ] != 0 ) && ( ipAddress[ count ] != ' ' ) )
                            {
                                ++count;
                            }
                            ipAddress.resize( count );
                            success = true;
                        }
                        if ( ipAddress.startsWith( C_LOOP_BACK_START ) )
                        {
                            break;
                        }
                    }
                    pclose( pPtr );
                }
            }
            else
            {
                success = true;
            }
        }
    }
    return success;
}

int amMulticastRead::connect
(
    const amString &curInterface,
    const amString &curIpAddress,
    int             curPortNo,
    int             curTimeOut
)
{
    if ( connectionUp )
    {
        errorCode = E_ALREADY_CONNECTED;
        errorMessage += "\namMulticastRead::connect: Already connected on interface \"";
        errorMessage += interface;
        errorMessage += "\" and connection \"";
        errorMessage += ipAddress;
        errorMessage += ":";
        errorMessage += amString( portNo );
        errorMessage += "\".";
    }
    else
    {
	interface = curInterface;
	ipAddress = curIpAddress;
        portNo    = curPortNo;
        timeOut   = curTimeOut;
        connect();
    }
    return errorCode;
}

int amMulticastRead::connect
(
    void
)
{
    char     auxErrorMessage[ C_BUFFER_SIZE ];
    int      errorCode    = 0;
    struct   sockaddr_in localSock;
    struct   ip_mreq     group;
    amString localIPAddress;

    if ( !isInterface( interface ) )
    {   
        errorCode     = E_MC_NO_INTERFACE;
        errorMessage += "Creating multicast connection for reading failed. No Interface.\n";
    }   

    if ( ( errorCode == 0 ) && !isMulticastIPAddress( ipAddress ) )
    {   
        errorCode     = E_MC_NO_IP_ADDRESS;
        errorMessage += "Creating multicast connection for writing failed. No IP Address.\n";
    }   

    if ( ( errorCode == 0 ) && !isPortNumber( portNo ) )
    {   
        errorCode     = E_MC_NO_PORT_NUMBER;
        errorMessage += "Creating multicast connection for writing failed. No Port Number.\n";
    }   

    if ( errorCode == 0 )
    {
        // Create a datagram socket on which to receive.
        socketID = socket( AF_INET, SOCK_DGRAM, 0 );
        if( socketID < 0 )
        {
            errorCode     = E_SOCKET_CREATE;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Creating socket option for multicast reading failed. Opening datagram socket error:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Enable SO_REUSEPORT to allow multiple instances of this
        // application to receive copies of the multicast datagrams.
        long flags = fcntl( socketID, F_GETFL, 0 );
        fcntl( socketID, F_SETFL, flags );

        int reuse = 1;

        int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_REUSEPORT, ( char * ) &reuse, sizeof( reuse ) );
        if ( resSetSockopt < 0 )
        {
            errorCode     = E_SOCKET_SET_OPT;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Setting of socket option failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        if ( timeOut > 0 )
        {
            struct timeval tv;

            tv.tv_sec  = timeOut;
            tv.tv_usec = 0;  // Not init'ing this can cause strange errors

            int resSetSockopt = setsockopt( socketID, SOL_SOCKET, SO_RCVTIMEO, ( char * ) &tv, sizeof( struct timeval ) );
            if ( resSetSockopt < 0 )
            {
                errorCode     = E_SOCKET_SET_OPT;
                strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
                errorMessage += "Setting of socket option failed:\n    ";
                errorMessage += auxErrorMessage;
                errorMessage += ".\n";
            }
        }
    }

    if ( errorCode == 0 )
    {
        // Bind to the proper port number with the IP address
        // specified as INADDR_ANY.
        bzero( ( void * ) &localSock, sizeof( localSock ) );

        localSock.sin_family = AF_INET;
        localSock.sin_port = htons( portNo );
        localSock.sin_addr.s_addr = INADDR_ANY;

        int bindResult = bind( socketID, ( struct sockaddr * ) &localSock, sizeof( localSock ) );
        if ( bindResult )
        {
            errorCode     = E_SOCKET_BIND;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Binding the socket failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Join the multicast group 226.1.1.1 on the local 203.106.93.94
        // interface. Note that this IP_ADD_MEMBERSHIP option must be
        // called for each local interface over which the multicast
        // datagrams are to be received.

        int getIPResult = determineIPAddress( localIPAddress, interface );
        if ( !getIPResult )
        {
            errorCode     = E_NO_IP_ADDRESS_IF;
            errorMessage += "No IP Address found on interface \"";
            errorMessage += interface;
            errorMessage += "\".\n";
        }
    }

    if ( errorCode == 0 )
    {
        // Check if the IP Address is not the loopback address
        if ( localIPAddress == C_LOOP_BACK_ADDRESS )
        {
            errorCode     = E_LOOP_BACK_IP_ADDRESS;
            errorMessage += "IP Address on interface \"";
            errorMessage += interface;
            errorMessage += "\" is loopback address.\n";
        }
    }

    if ( errorCode == 0 )
    {
        group.imr_multiaddr.s_addr = inet_addr( ipAddress.c_str() );
        group.imr_interface.s_addr = inet_addr( localIPAddress.c_str() );

        int resSetSockopt = setsockopt( socketID, IPPROTO_IP, IP_ADD_MEMBERSHIP, ( char * ) &group, sizeof( group ) );
        if( resSetSockopt < 0 )
        {
            errorCode     = E_SOCKET_SET_OPT;
            strerror_r( errno, auxErrorMessage, C_BUFFER_SIZE );
            errorMessage += "Setting of socket option failed:\n    ";
            errorMessage += auxErrorMessage;
            errorMessage += ".\n";
        }
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

void amMulticastRead::mc2Stream
(
    std::ostream &outStream
)
{
    bool    running = true;
    size_t  nbBytes = 0;
    BYTE   *buffer  = NULL;

    if ( bufferSize == 0 )
    {
        bufferSize = C_BUFFER_SIZE;
    }

    buffer = new BYTE( bufferSize );

    while ( running )
    {
        nbBytes = read( buffer, bufferSize );
        if ( exitString == ( const char * ) buffer )
        {
            running = false;
        }
        if ( running || outputExitString )
        {
            outStream << ( const char * ) buffer;
            if ( !binary )
            {
                outStream << std::endl;
            }
        }
    }
    delete[] buffer;
    buffer = NULL;
}

