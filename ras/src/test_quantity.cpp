#include "quantity.h"
#include <am_split_string.h>
#include <am_string.h>
#include <am_constants.h>
#include <ant_constants.h>

#include <iostream>
#include <fstream>
#include <map>


const char C_END_MESSAGE[] = "xxx";

static int debugLevel = 1;

int main
(
    int   argc,
    char *argv[]
)
{
    if ( argc <= 1 )
    {
        std::cerr << argv[ 0 ] << ": Error. Too few arguments." << std::endl;
        std::cerr << "Call: " << argv[ 0 ] << " <in_file> [<out_file>]" << std::endl;
    }
    else
    {
        std::map<amString, quantity>  speedQuantity;
        std::map<amString, quantity>  distanceQuantity;
        std::map<amString, quantity>  airSpeedQuantity;
        std::map<amString, quantity>  yawAngleQuantity;
        std::map<amString, quantity>  powerQuantity;
        std::map<amString, quantity>  hrmQuantity;
        std::map<amString, quantity>  cadenceQuantity;
        std::map<amString, quantity>  torqueQuantity;
        std::ifstream                 inStream;
        std::ofstream                 outStream;
        const char                   *inFile        = argv[ 1 ];
        const char                   *outFile       = "";
        int                           nbWords       = 0;
        int                           lineCount     = 0;
        double                        firstTime     = 0;
        double                        distance      = 0;
        double                        avgSpeed      = 0;
        double                        deltaTime     = 0;
        double                        lastDistannce = 0;
        double                        timeValue     = 0;
        double                        value[ 4 ]    = { 0, 0, 0, 0 };
        amQuantityType                type[ 4 ]     = { UNDEFINED_QUANTITY, UNDEFINED_QUANTITY, UNDEFINED_QUANTITY, UNDEFINED_QUANTITY };
        amString                      deviceName;
        amSplitString                 words;
        bool                          inStreamIsFile  = false;
        bool                          outStreamIsFile = false;
        char                          line[ C_BUFFER_SIZE ];

        if ( strcmp( inFile, "1" ) != 0 )
        {
            inStream.open( inFile );
            inStreamIsFile = true;
        }
        if ( argc > 2 )
        {
            outFile = argv[ 2 ];
            outStream.open( outFile );
            outStreamIsFile = true;
        }
        std::istream &iStream = inStreamIsFile  ?  inStream : std::cin;
        std::ostream &oStream = outStreamIsFile ? outStream : std::cout;
        while ( iStream.good() )
        {
            iStream.getline( line, C_BUFFER_SIZE );
            if ( iStream.eof() || iStream.bad() )
            {
                break;
            }
            if ( debugLevel > 0 )
            {
                ++lineCount;
            }
            if ( ( strlen( line ) > 0 ) && ( line[ 0 ] != C_COMMENT_SYMBOL ) )
            {
                if ( strcmp( line, C_END_MESSAGE ) == 0 )
                {
                    break;
                }
                nbWords = words.split( line );
                if ( debugLevel > 0 )
                {
                    std::cerr << "Line " << ++lineCount << ": " << line << std::endl;
                }
                if ( nbWords > 3 )
                {
                    deviceName = words[ 0 ];
                    timeValue  = words[ 1 ].toDouble();
                    if ( firstTime == 0 )
                    {
                        firstTime = timeValue;
                    }

                    type[ 0 ] = type[ 1 ] = type[ 2 ] = type[ 3 ] = UNDEFINED_QUANTITY;

                    if ( deviceName.startsWith( "SPB7_" ) )
                    {
                        type[ 0 ]  = SPEED;
                        value[ 0 ] = words[ 2 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "SPCAD790_" ) )
                    {
                        type[ 0 ]  = SPEED;
                        type[ 1 ]  = CADENCE;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "PWRB10_" ) )
                    {
                        type[ 0 ] = POWER;
                        type[ 1 ] = CADENCE;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "PWRB11_" ) )
                    {
                        type[ 0 ] = POWER;
                        type[ 1 ] = CADENCE;
                        type[ 2 ] = TORQUE;
                        type[ 3 ] = SPEED;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                        value[ 2 ] = words[ 4 ].toDouble();
                        value[ 3 ] = words[ 5 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "PWRB12_" ) )
                    {
                        type[ 0 ] = POWER;
                        type[ 1 ] = CADENCE;
                        type[ 2 ] = TORQUE;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                        value[ 2 ] = words[ 4 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "PWRB20_" ) )
                    {
                        type[ 0 ]  = POWER;
                        type[ 1 ]  = CADENCE;
                        type[ 2 ]  = TORQUE;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                        value[ 2 ] = words[ 4 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "HRM_" ) )
                    {
                        type[ 0 ]  = HEART_RATE;
                        value[ 0 ] = words[ 2 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "AERO_" ) )
                    {
                        type[ 0 ]  = AIR_SPEED;
                        type[ 1 ]  = YAW_ANGLE;
                        value[ 0 ] = words[ 2 ].toDouble();
                        value[ 1 ] = words[ 3 ].toDouble();
                    }
                    else if ( deviceName.startsWith( "CAD7A_" ) )
                    {
                        type[ 0 ]  = CADENCE;
                        value[ 0 ] = words[ 2 ].toDouble();
                    }
                }
                for ( int counter = 0; counter < 4; ++counter )
                {
                    switch ( type[ counter ] )
                    {
			case POWER:
                             powerQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
			case CADENCE:
                             cadenceQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        case TORQUE:
                             torqueQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        case SPEED:
                             distance = 0;
                             if ( speedQuantity[ deviceName ].getEndIndex() > 0 )
                             {
                                 avgSpeed      = ( speedQuantity[ deviceName ].getLastValue() + value[ counter ] ) / 2.0;
                                 deltaTime     =timeValue - speedQuantity[ deviceName ].getLastTime();
                                 lastDistannce = distanceQuantity[ deviceName ].getLastValue();
                                 distance      = lastDistannce + avgSpeed * deltaTime;  // m/s * s = m
                             }
                             distanceQuantity[ deviceName ].newValue( timeValue, distance );
                             speedQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        case YAW_ANGLE:
                             yawAngleQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        case AIR_SPEED:
                             airSpeedQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        case HEART_RATE:
                             hrmQuantity[ deviceName ].newValue( timeValue, value[ counter ] );
                             break;
                        default:
                             break;
                    }
                }
            }
        }
        if ( inStreamIsFile )
        {
            inStream.close();
        }

        std::map<amString, quantity>::iterator iter;

        oStream << std::endl;
        oStream << "SPEED" << std::endl;
        for ( iter = speedQuantity.begin(); iter != speedQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "DISTANCE" << std::endl;
        for ( iter = distanceQuantity.begin(); iter != distanceQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "POWER" << std::endl;
        for ( iter = powerQuantity.begin(); iter != powerQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "CADENCE" << std::endl;
        for ( iter = cadenceQuantity.begin(); iter != cadenceQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "TORQUE" << std::endl;
        for ( iter = torqueQuantity.begin(); iter != torqueQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "HRM" << std::endl;
        for ( iter = hrmQuantity.begin(); iter != hrmQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "AIR_SPEED" << std::endl;
        for ( iter = airSpeedQuantity.begin(); iter != airSpeedQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        oStream << std::endl;
        oStream << "YAW_ANGLE" << std::endl;
        for ( iter = yawAngleQuantity.begin(); iter != yawAngleQuantity.end(); ++iter )
        {
            oStream << std::endl;
            oStream << "key " << iter->first << std::endl;
            iter->second.stopCollecting();
            iter->second.outputArrays( oStream );
        }

        if ( outStreamIsFile )
        {
            outStream.close();
        }
    }
}

