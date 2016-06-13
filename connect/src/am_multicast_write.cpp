#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>

#include <am_util.h>
#include <am_string.h>
#include <am_constants.h>

#include "connect_util.h"
#include "am_multicast_write.h"

amMulticastWrite::amMulticastWrite
(
    void
)
{
    connectionUp = false;
    socketID     = -1;
    portNo       = 0;
    pause        = C_PAUSE;
    bufferSize   = C_BUFFER_SIZE;
    ipAddress.clear();
    bzero( ( char * ) &groupSock, sizeof( groupSock ) );
}

amMulticastWrite::amMulticastWrite
(
    const amString &curIPAddress,
    int             curPortNo
) : ipAddress( curIPAddress ),
    portNo( curPortNo )
{
    connectionUp = false;
    socketID     = -1;
    pause        = C_PAUSE;
    bufferSize   = C_BUFFER_SIZE;
    bzero( ( char * ) &groupSock, sizeof( groupSock ) );
    connect();
}

void amMulticastWrite::close
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
        bzero( ( char * ) &groupSock, sizeof( groupSock ) );
    }
}

size_t amMulticastWrite::write
(
    const amString &buffer,
    int             length
)
{
    if ( connectionUp && ( socketID > 0 ) && !errorCode )
    {
        if ( !buffer.empty() )
        {
            size_t bufferSize = ( length < 0 ) ? buffer.size() : length;
            if ( sendto( socketID, buffer.c_str(), bufferSize, 0, ( struct sockaddr * ) &groupSock, sizeof( groupSock ) ) < 0 )
            {
                errorCode     = E_MC_WRITE;
                errorMessage += "Sending message via multicast failed.\n";
            }
        }
    }
    return errorCode;
}

int amMulticastWrite::connect
(
    const amString &curIPAddress,
    int             curPortNo
)
{
    if ( connectionUp )
    {
        errorCode = E_ALREADY_CONNECTED;
        errorMessage += "\namMulticastWrite::connect: Already connected on connection \"";
        errorMessage += ipAddress;
        errorMessage += ":";
        errorMessage += amString( portNo );
        errorMessage += "\".";
    }
    else
    {
        ipAddress = curIPAddress;
        portNo    = curPortNo;
        connect();
    }
    return errorCode;
}

int amMulticastWrite::connect
(
    void
)
{
    if ( !ipAddress.empty() )
    {   
        errorCode     = E_MC_NO_IP_ADDRESS;
        errorMessage += "Creating multicast connection for writing failed. No IP Address.\n";
    }   
    else if ( portNo <= 0 ) 
    {   
        errorCode     = E_MC_NO_PORT_NUMBER;
        errorMessage += "Creating multicast connection for writing failed. No Port Number.\n";
    }   
    else
    {   
        socketID = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( socketID < 0 )
        {
            errorCode     = E_SOCKET_CREATE;
            errorMessage += "Creating socket option for multicast writing failed. Opening datagram socket error.\n";
        }

        bzero( ( char * ) &groupSock, sizeof( groupSock ) );
        groupSock.sin_family = AF_INET;
        groupSock.sin_addr.s_addr = htonl( INADDR_ANY );
        groupSock.sin_port = htons( portNo );
        groupSock.sin_addr.s_addr = inet_addr( ipAddress.c_str() );
    }

    connectionUp = ( errorCode == 0 );

    return errorCode;
}

int amMulticastWrite::stream2mc
(
    std::istream &inStream
)
{
    amString      message;
    bool          running = true;

    if ( ( errorCode == 0 ) && !connectionUp )
    {
        errorCode = E_MC_NO_CONNECTION;
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
        while ( running )
        {
            message.reserve( bufferSize );

            if ( inStream.eof() || inStream.fail() )
            {
                running = false;
            }
            else
            {
                std::getline( inStream, message );
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

                    write( message, message.size() );
                    if ( errorCode != 0 )
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
    return errorCode;
}

