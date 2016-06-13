#ifndef __CONNECT_UTIL_H__
#define __CONNECT_UTIL_H__

#include <am_string.h>

enum amTimeStampMode{ KEEP_AS_IS, OVERWRITE, APPEND };

const int  C_ZMQ_WAIT                  =   500;
const int  C_PAUSE                     =     0;
const int  C_MAX_PORT_NUMBER           = 65535;

const int E_BAD_NUMBER_OF_SUBADDRESSES = 50001;
const int E_BAD_SUBADDRESS             = 50002;
const int E_MC_NO_INTERFACE            = 51001;
const int E_MC_NO_IP_ADDRESS           = 51002;
const int E_MC_NO_PORT_NUMBER          = 51003;
const int E_NO_IP_ADDRESS_IF           = 51004;
const int E_READ_TIMEOUT               = 51005;
const int E_MC_WRITE                   = 51006;
const int E_MC_NO_CONNECTION           = 51007;
const int E_SOCKET_CREATE              = 51101;
const int E_SOCKET_SET_OPT             = 51102;
const int E_SOCKET_BIND                = 51103;
const int E_LOOP_BACK_IP_ADDRESS       = 51104;
const int E_ZMQ_NO_SOCKET              = 52001;
const int E_ZMQ_NO_CONTEXT             = 52002;
const int E_ZMQ_NO_IP_ADDRESS          = 52003;
const int E_ZMQ_NO_PORT_NUMBER         = 52004;
const int E_ZMQ_NO_CONNECTION          = 52005;
const int E_ZMQ_BIND                   = 52006;
const int E_ZMQ_CONNECT                = 52007;
const int E_ZMQ_SUBSCRIBE              = 52008;
const int E_ZMQ_LINGER                 = 52009;
const int E_ZMQ_SUBSCRIBE_FAIL         = 52010;
const int E_ZMQ_RECEIVE                = 52011;
const int E_ZMQ_SEND                   = 52012;
const int E_BAD_PAUSE_VALUE            = 52021;
const int E_NO_CONNECTION_IN           = 52051;
const int E_NO_CONNECTION_OUT          = 52052;
const int E_NO_CONNECTION_IN_OUT       = 52053;
const int E_TOO_FEW_PORT_NOS           = 52054;
const int E_TOO_MANY_PORT_NOS          = 52055;
const int E_ZMQ_CLOSE                  = 52080;
const int E_EMPTY_MESSAGE              = 52090;
const int E_ZMQ_UNKNOWN                = 52099;
const int E_BAD_PORT_NUMBER            = 53001;
const int E_BAD_PRECISION_VALUE        = 53002;
const int E_NO_TIMESTAMP_VALUE         = 53003;


extern int verifyPortNumber( int portNo );
extern int verifyIPAddress( const amString &ipAddress );

extern amString createTCPAddress( const amString &ipAddress, int portNo );
extern int outputConnectError( const amString &programName, const amString &errorMessage, int errorCode, const amString &additionalInfo );


#endif // __CONNECT_UTIL_H__
