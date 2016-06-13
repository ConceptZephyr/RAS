#include <iostream>
#include <am_split_string.h>

#include "connect_util.h"


const size_t C_NB_SUB_ADDRESSES_IPV_4 = 4;
const size_t C_NB_SUB_ADDRESSES_IPV_6 = 6;

amString createTCPAddress
(
    const amString &ipAddress,
    int             portNo
)
{
    amString result( "tcp://" );
    result += ipAddress;
    result += ":";
    result += amString( portNo );

    return result;
}

int verifyIPAddress
(
    const amString &ipAddress
)
{
    int           errorCode = 0;
    amSplitString words;
    size_t        nbWords   = words.split( ipAddress, "." );

    if ( ( nbWords != C_NB_SUB_ADDRESSES_IPV_4 ) && ( nbWords != C_NB_SUB_ADDRESSES_IPV_6 ) )
    {
        errorCode = E_BAD_NUMBER_OF_SUBADDRESSES;
    }

    for ( size_t counter = 0; ( errorCode == 0 ) && ( counter < nbWords ); ++counter )
    {
        unsigned int subAddress = words[ counter ].toUInt();
        if ( ( subAddress > 255 ) || ( ( counter == 0 ) && ( subAddress == 0 ) ) )
        {
            errorCode = E_BAD_SUBADDRESS;
        }
    }

    return errorCode;
}

int verifyPortNumber
(
    int portNo
)
{
    int errorCode = ( ( portNo > 0 ) && ( portNo < C_MAX_PORT_NUMBER ) ) ? 0 : E_BAD_PORT_NUMBER;
    return errorCode;
}

int outputConnectError
(
    const amString &programName,
    const amString &errorMessage,
    int            errorCode,
    const amString &additionalInfo
)
{
    amString finalErrorMessage = errorMessage;
std::cerr << "errorCode = " << errorCode << std::endl;
std::cerr << "errorMessage = " << errorMessage << std::endl;
    switch ( errorCode )
    {
        case E_BAD_NUMBER_OF_SUBADDRESSES:
             finalErrorMessage += "Connection Error: IP Address is incorrect";
             break;
        case E_BAD_SUBADDRESS            :
             finalErrorMessage += "Connection Error: IP Address is incorrect";
             break;
        case E_MC_NO_INTERFACE           :
             finalErrorMessage += "Connection Error: No interface";
             break;
        case E_MC_NO_IP_ADDRESS          :
             finalErrorMessage += "Connection Error: No IP Address";
             break;
        case E_MC_NO_PORT_NUMBER         :
             finalErrorMessage += "Connection Error: No Port Number";
             break;
        case E_NO_IP_ADDRESS_IF          :
             finalErrorMessage += "Connection Error: No interface and no IP Address";
             break;
        case E_READ_TIMEOUT              :
             finalErrorMessage += "Connection Error: Time out while reading";
             break;
        case E_MC_WRITE                  :
             finalErrorMessage += "Error while writing to multicast";
             break;
        case E_SOCKET_CREATE             :
             finalErrorMessage += "Error while creating a socket";
             break;
        case E_SOCKET_SET_OPT            :
             finalErrorMessage += "Error while setting a socket option";
             break;
        case E_SOCKET_BIND               :
             finalErrorMessage += "Error while binding a socket";
             break;
        case E_LOOP_BACK_IP_ADDRESS      :
             finalErrorMessage += "Connection Error: IP Address is loopback address";
             break;
        case E_ZMQ_NO_SOCKET             :
             finalErrorMessage += "ZMQ Error: ZMQ Socket does not exist";
             break;
        case E_ZMQ_NO_CONTEXT            :
             finalErrorMessage += "ZMQ Error: ZMQ Context does not exist";
             break;
        case E_ZMQ_NO_IP_ADDRESS         :
             finalErrorMessage += "ZMQ Error: No IP Address";
             break;
        case E_ZMQ_NO_PORT_NUMBER        :
             finalErrorMessage += "ZMQ Error: No Port Number";
             break;
        case E_ZMQ_NO_CONNECTION         :
             finalErrorMessage += "ZMQ Error: Connection could not be created";
             break;
        case E_ZMQ_BIND                  :
             finalErrorMessage += "ZMQ Error: Error while binding the socket";
             break;
        case E_ZMQ_CONNECT               :
             finalErrorMessage += "ZMQ Error: Error while connecting";
             break;
        case E_ZMQ_SUBSCRIBE             :
             finalErrorMessage += "ZMQ Error: Error while subscribing";
             break;
        case E_ZMQ_LINGER                :
             finalErrorMessage += "ZMQ Error: Error while setting ZMQ LINGER option";
             break;
        case E_ZMQ_SUBSCRIBE_FAIL                :
             finalErrorMessage += "ZMQ Error: Error while setting ZMQ SUBSCIBE option";
             break;
        case E_ZMQ_RECEIVE               :
             finalErrorMessage += "ZMQ Error: Error while receiving data";
             break;
        case E_ZMQ_SEND                  :
             finalErrorMessage += "ZMQ Error: Sending of ZMQ Message failed";
             break;
        case E_BAD_PAUSE_VALUE           :
             finalErrorMessage += "Bad value for parameter 'pause'";
             break;
        case E_NO_CONNECTION_IN          :
             finalErrorMessage += "ZMQ Error: No Input Connection";
             break;
        case E_NO_CONNECTION_OUT         :
             finalErrorMessage += "ZMQ Error: No Output Connection";
             break;
        case E_NO_CONNECTION_IN_OUT      :
             finalErrorMessage += "ZMQ Error: No Input or Output Connection";
             break;
        case E_TOO_FEW_PORT_NOS          :
             finalErrorMessage += "ZMQ Error: Not enough Port Numbers";
             break;
        case E_TOO_MANY_PORT_NOS         :
             finalErrorMessage += "ZMQ Error: Too many Port Numbers";
             break;
        case E_ZMQ_CLOSE                 :
             finalErrorMessage += "ZMQ Error: Error while Closing the Connection";
             break;
        case E_EMPTY_MESSAGE             :
             finalErrorMessage += "ZMQ Error: Empty Message";
             break;
        case E_ZMQ_UNKNOWN               :
             finalErrorMessage += "Unknown ZMQ Error";
             break;
        case E_BAD_PORT_NUMBER           :
             finalErrorMessage += "Bad value for parameter 'port number'";
             break;
        case E_BAD_PRECISION_VALUE       :
             finalErrorMessage += "Bad value for parameter 'ti,e precision'";
             break;
        case E_NO_TIMESTAMP_VALUE        :
             finalErrorMessage += "Bad value for parameter 'time stamp'";
             break;
        default:
             break;
    }

    if ( errorCode != 0 )
    {
        std::cerr << std::endl << "ERROR " << finalErrorMessage << " (" << errorCode << ")." << std::endl;
    }
    else if ( finalErrorMessage.size() > 0 )
    {
        std::cerr << std::endl << "WARNING " << finalErrorMessage << " (" << errorCode << ")." << std::endl;
    }
    std::cerr << std::endl;

    return errorCode;
}

