#ifndef __AM_CONSTANTS_H__
#define __AM_CONSTANTS_H__

#include <stdlib.h>

const char C_VERSION[]           = "1.0.1";
const char C_BUILD_NUMBER[]      = "1";

const char C_MODIFICATION_SYMBOL                            = '%';

const char C_START_OF_MESSAGES[]                            = "AM_DASHBOARD_START_OF_RECORDING";
const char C_END_OF_MESSAGES[]                              = "AM_DASHBOARD_END_OF_RECORDING";
const char C_START_OF_DEFINITIONS[]                         = "AM_DASHBOARD_START_OF_DEFINITION";
const char C_END_OF_DEFINITIONS[]                           = "AM_DASHBOARD_END_OF_DEFINITION";
const char C_INDIVIDUAL_DEVICE[]                            = "INDIVIDUAL_DEVICE";
const char C_GLOBAL_DEVICE_ID[]                             = "GLOBAL_DEVICE";
const char C_ADD_DEVICE_ID[]                                = "+";
const char C_MODIFICATION_STRING[]                          = { C_MODIFICATION_SYMBOL, 0 };
const char C_SPEED_SENSOR_ID[]                              = "SPEED";
const char C_POWER_SENSOR_ID[]                              = "POWER";
const char C_AERO_SENSOR_ID[]                               = "AERO";
const char C_ALTITUDE_SENSOR_ID[]                           = "ALTITUDE";
const char C_CDA_CRR_SENSOR_ID[]                            = "CDA_CRR";

const char C_RIDER_NAME_ID[]                                = "NAME";
const char C_WHEEL_CIRCUMFERENCE_ID[]                       = "WHEEL_CIRCUMFERENCE";
const char C_NUMBER_OF_MAGNETS_ID[]                         = "NUMBER_OF_MAGNETS";
const char C_GEAR_RATIO_ID[]                                = "GEAR_RATIO";
const char C_DRIVE_TRAIN_EFFICIENCY_ID[]                    = "ETA";
const char C_POWER_METER_OFFSET_ID[]                        = "POWER_METER_OFFSET";
const char C_POWER_METER_SLOPE_ID[]                         = "POWER_METER_SLOPE";
const char C_CURRENT_AIR_DENSITY_ID[]                       = "RHO";
const char C_CALIBRATION_AIR_DENSITY_ID[]                   = "CALIBRATION_AIR_DENSITY";
const char C_AIR_SPEED_MULTIPLIER_ID[]                      = "AIR_SPEED_MULTIPLIER";
const char C_RIDER_AND_BIKE_MASS_ID[]                       = "MASS";
const char C_DEFAULT_CDA_ID[]                               = "CDA_DEFAULT_VALUE";
const char C_DEFAULT_CRR_ID[]                               = "CRR_DEFAULT_VALUE";
const char C_INTERVAL_LENGTH_ID[]                           = "INTERVAL_LENGTH";
const char C_COMP_INTERVAL_ID  []                           = "COMP_INTERVAL";
const char C_COMPUTE_CDA_ID[]                               = "COMPUTE_CDA";
const char C_COMPUTE_CRR_ID[]                               = "COMPUTE_CRR";


const char C_COPYRIGHT[] = "Copyright (c) Alphamantis Technologies Inc. 2016";

#define IS_WHITE_CHAR( _CCC_ ) ( ( ( _CCC_ ) == ' ' ) || ( ( _CCC_ ) == '\t' ) )

#define NEGATE_BINARY_INT( _NNN_, _PPP_ ) ( ( ( _NNN_ ) >= ( 1 << ( (_PPP_ ) - 1 ) ) ) ?  ( ( _NNN_ ) - ( 1 << ( _PPP_ ) ) ) : ( _NNN_ ) )

#define HEX_DIGIT_2_INT( _HHH_ ) ( ( ( ( _HHH_ ) >= '0' ) && ( ( _HHH_ ) <= '9' ) ) ? ( ( _HHH_ ) - '0' ) : \
                                   ( ( ( toupper( _HHH_ ) >= 'A' ) && ( toupper( _HHH_ ) <= 'F' ) ) ? ( toupper( _HHH_ ) - 'A' + 10 ) : 0 ) )


// Buffer sizes
#define C_BUFFER_SIZE         ( 1 << 10 )
#define C_ZMQ_BUFFER_SIZE     ( 1 << 10 )
#define C_MEDIUM_BUFFER_SIZE  ( 1 <<  9 )
#define C_SMALL_BUFFER_SIZE   ( 1 <<  8 )
#define C_TINY_BUFFER_SIZE    ( 1 <<  6 )
#define C_ANT_PAYLOAD_LENGTH  8

#define C_SEPARATOR           '\t'
#define C_SEPARATOR_AS_STRING "\t"

enum amOperatingSystem
{
    MAC_OSX,
    LINUX,
    CYGWIN,
    UNKNOWN
};


const int E_READ_ERROR            = 90001;
const int E_READ_FILE_NOT_OPEN    = 90002;
const int E_EMPTY_FILE_NAME       = 90003;
const int E_NO_FILE_NAME          = 90004;
const int E_WRITE_FILE_NOT_OPEN   = 90005;
const int E_END_OF_FILE           = 90099;

const int E_MISSING_ARG           = 91001;
const int E_UNKNOWN_OPTION        = 91099;

const int E_BAD_BUFFER_SIZE       = 92001;

const int E_BAD_OPTION            = 93001;
const int E_FREE_PORT_NO          = 93002;
const int E_ALREADY_CONNECTED     = 93003;

const int E_CREATE_THREAD         = 94001;

const size_t C_TIME_PRECISION     =     6;
const size_t C_MAX_TIME_PRECISION =    20;

const double C_MIN_DBL_VALUE      = -1E10;
const double C_MAX_DBL_VALUE      =  1E10;
const double C_DBL_UNDEFINED      =  1E20;
const double C_DBL_UNDEFINED_TEST =  9E19;

#define isDefined( __AAA__ ) ( ( __AAA__ ) <= C_DBL_UNDEFINED_TEST )

const char C_MAC_OS_SYS_NAME[]                              = "Darwin";
const char C_LINUX_SYS_NAME[]                               = "Linux";
const char C_CYGWIN_SYS_NAME[]                              = "Cygwin";
const char C_INTERFACE_LINUX[]                              = "wlan0";
const char C_INTERFACE_MACOSX[]                             = "en0";
const char C_INTERFACE_CYGWIN[]                             = "eth0";
#if __APPLE__
const char C_INTERFACE_ROOT[]                               = "en";
#else
const char C_INTERFACE_ROOT[]                               = "wlan";
#endif

const char C_LOOP_BACK_START[]   ="127.0.";
const char C_LOOP_BACK_ADDRESS[] = "127.0.0.1";


const double C_DEFAULT_CRR             =    0.003;
const double C_DEFAULT_CDA             =    0.2;
const double C_DEFAULT_ETA             =    0.98;
const double C_DEFAULT_MASS            =   80.0;
const double C_DEFAULT_AIR_DENISTY     =    1.18;
const double C_DEFAULT_INTERVAL_LENGTH = 1000;

#endif // __ANT_CONSTANTS_H__

