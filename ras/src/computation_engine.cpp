#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <ant_common.h>
#include <ant_constants.h>
#include <am_split_string.h>
#include <am_util.h>

#include "ras_constants.h"
#include "computation_engine.h"

const int C_MAX_LINE_COUNT = 20;

// ----------------------------------------------------------------------------------------------------
//
// Constructor
//
// Create object and initialize all 'fixed' and 'variable' parameters.
//
// ----------------------------------------------------------------------------------------------------
computationEngine::computationEngine
(
    void
)
{
    debugStream.open( "debug" );
    riderName.clear();
    speedSensorID.clear();
    powerSensorID.clear();
    aeroSensorID.clear();
    altitudeSensorID.clear();
    crrSensorID.clear();
    cdaSensorID.clear();

    inIPAddress        = C_RAS_INPUT_IP_ADDRESS;
    outIPAddress       = C_RAS_OUTPUT_IP_ADDRESS;
    inPortNo           = C_RAS_INPUT_PORT_NUMBER;
    outPortNo          = C_RAS_OUTPUT_PORT_NUMBER;

    computeCda         = true;
    computeCrr         = false;
    etaSet             = false;
    bufferSize         = C_ZMQ_BUFFER_SIZE;
    maxSimpsonRule     = C_MAX_SIMPSON_RULE;
    minSimpsonRule     = C_MIN_SIMPSON_RULE;
    simpsonRuleEpsilon = C_EPS_SIMPSON_RULE;
    rho                = C_DEFAULT_AIR_DENISTY;
    eta                = C_CRANK_TORQUE_ETA;
    mass               = C_DEFAULT_MASS;
    intervalLength     = C_DEFAULT_INTERVAL_LENGTH;
    defaultCda         = C_DEFAULT_CDA;
    defaultCrr         = C_DEFAULT_CRR;
    sleepTime          = C_SLEEP_TIME;

    cdaMaxLineCount    = C_MAX_LINE_COUNT;
    crrMaxLineCount    = C_MAX_LINE_COUNT;

    readActive         = false;
    debugLevel         = 0;

    crrQuantity.setQuantityType( CRR );
    cdaQuantity.setQuantityType( CDA );
    speedQuantity.setQuantityType( SPEED );
    powerQuantity.setQuantityType( POWER );
    distanceQuantity.setQuantityType( DISTANCE );
    airSpeedQuantity.setQuantityType( AIR_SPEED );
    yawAngleQuantity.setQuantityType( YAW_ANGLE );
    altitudeQuantity.setQuantityType( ALTITUDE );

    powPrecision   = 3;
    potPrecision   = 3;
    kinPrecision   = 3;
    jrrPrecision   = 3;
    aeroPrecision  = 3;
    cdaPrecision   = 5;
    time1Precision = 2;
    timePrecision  = 6;
    distPrecision  = 3;
    friPrecision   = 3;
    jdaPrecision   = 3;
    crrPrecision   = 7;
    rhoPrecision   = 4;

    reset();
}

// ----------------------------------------------------------------------------------------------------
//
// Reset
//
// Reset all 'variable' parameters to their defaults.
//
// ----------------------------------------------------------------------------------------------------
void computationEngine::reset
(
    void
)
{
    timeAirSpeed       = C_DBL_UNDEFINED;
    timeYawAngle       = C_DBL_UNDEFINED;
    timeAltitude       = C_DBL_UNDEFINED;
    timeSpeed          = C_DBL_UNDEFINED;
    timePower          = C_DBL_UNDEFINED;
    timeDistance       = C_DBL_UNDEFINED;
    JAero              = 0;
    JPow               = 0;
    prevIntervalStart  = 0;
    prevIntervalEnd    = 0;
    initialTime        = -1;
    definitionCode     = -1;

    writeLock          = false;
    running            = true;
    startCompute       = false;
    cdaLineCounter     = cdaMaxLineCount;
    crrLineCounter     = crrMaxLineCount;

    crrQuantity.reset();
    cdaQuantity.reset();
    speedQuantity.reset();
    powerQuantity.reset();
    distanceQuantity.reset();
    airSpeedQuantity.reset();
    yawAngleQuantity.reset();
    altitudeQuantity.reset();

    crrQuantity.insert     ( 0, C_DBL_UNDEFINED );
    cdaQuantity.insert     ( 0, C_DBL_UNDEFINED );
    speedQuantity.insert   ( 0, C_DBL_UNDEFINED );
    powerQuantity.insert   ( 0, C_DBL_UNDEFINED );
    distanceQuantity.insert( 0, C_DBL_UNDEFINED );
    airSpeedQuantity.insert( 0, C_DBL_UNDEFINED );
    yawAngleQuantity.insert( 0, C_DBL_UNDEFINED );
    altitudeQuantity.insert( 0, C_DBL_UNDEFINED );

    powWidth       = 9;
    potWidth       = 8;
    kinWidth       = 8;
    jrrWidth       = 8;
    aeroWidth      = 9;
    cdaWidth       = 7;
    timeWidth      = 7;
    distWidth      = 9;
    friWidth       = 8;
    jdaWidth       = 9;
    crrWidth       = 9;
}

// ----------------------------------------------------------------------------------------------------
//
// outputCda
//
// Debug function: Output CdA, time, distance and all 5 enegry intervals
//
// ----------------------------------------------------------------------------------------------------
void computationEngine::outputCda
(
    double JJPow,
    double JJPot,
    double JJKin,
    double JJrr,
    double JJAero,
    double curCda,
    double time0,
    double time1,
    double dist0,
    double dist1
)
{
    std::stringstream auxStream;
    int curWidth = 0;

    if ( powWidth != 0 )
    {
        curWidth = powPrecision + 2 + ( int ) round( log( fabs( JJPow ) ) / log( 10.0 ) );
        if ( powWidth < curWidth )
        {
            powWidth = curWidth;
        }
    }
    if ( potWidth != 0 )
    {
        curWidth = potPrecision + 2 + ( int ) round( log( fabs( JJPot ) ) / log( 10.0 ) );
        if ( potWidth < curWidth )
        {
            potWidth = curWidth;
        }
    }
    if ( kinWidth != 0 )
    {
        curWidth = kinPrecision + 2 + ( int ) round( log( fabs( JJKin ) ) / log( 10.0 ) );
        if ( kinWidth < curWidth )
        {
            kinWidth = curWidth;
        }
    }
    if ( jrrWidth != 0 )
    {
        curWidth = jrrPrecision + 2 + ( int ) round( log( fabs( JJrr ) ) / log( 10.0 ) );
        if ( jrrWidth < curWidth )
        {
            jrrWidth = curWidth;
        }
    }
    if ( aeroWidth != 0 )
    {
        curWidth = aeroPrecision + 2 + ( int ) round( log( fabs( JJAero ) ) / log( 10.0 ) );
        if ( aeroWidth < curWidth )
        {
            aeroWidth = curWidth;
        }
    }
    if ( cdaWidth != 0 )
    {
        curWidth = cdaPrecision + 2 + ( int ) round( log( fabs( curCda ) ) / log( 10.0 ) );
        if ( cdaWidth < curWidth )
        {
            cdaWidth = curWidth;
        }
    }
    if ( timeWidth != 0 )
    {
        curWidth = time1Precision + 2 + ( int ) round( log( fabs( time1 ) ) / log( 10.0 ) );
        if ( timeWidth < curWidth )
        {
            timeWidth = curWidth;
        }
    }
    if ( distWidth != 0 )
    {
        curWidth = distPrecision + 2 + ( int ) round( log( fabs( dist1 ) ) / log( 10.0 ) );
        if ( distWidth < curWidth )
        {
            distWidth = curWidth;
        }
    }

    if ( ++cdaLineCounter > cdaMaxLineCount )
    {
        cdaLineCounter = 0;

        std::stringstream divStream;
        divStream << "-----+-";
        divStream << std::setw( powWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( potWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( kinWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( jrrWidth ) << std::setfill( '-' ) << "-";
        divStream << "---+-";
        divStream << std::setw( aeroWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( cdaWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( timeWidth ) << std::setfill( '-' ) << "-";
        divStream << "--";
        divStream << std::setw( timeWidth ) << std::setfill( '-' ) << "-";
        divStream << "-+-";
        divStream << std::setw( distWidth ) << std::setfill( '-' ) << "-";
        divStream << "--";
        divStream << std::setw( distWidth ) << std::setfill( '-' ) << "-";
        divStream << std::endl;
        divStream << std::setfill( ' ' );

        auxStream << divStream.str();
        auxStream << "CdA: ( ";
        auxStream << std::setw( powWidth ) << "Power";
        auxStream << " - ";
        auxStream << std::setw( potWidth ) << "PotEn";
        auxStream << " - ";
        auxStream << std::setw( kinWidth ) << "KinEn";
        auxStream << " - ";
        auxStream << std::setw( jrrWidth ) << "FriEn";
        auxStream << " ) / ";
        auxStream << std::setw( aeroWidth ) << "AeroEn";
        auxStream << " = ";
        auxStream << std::setw( cdaWidth ) << "cda";
        auxStream << " | ";
        auxStream << std::setw( timeWidth ) << "time0";
        auxStream << "  ";
        auxStream << std::setw( timeWidth ) << "time1";
        auxStream << " | ";
        auxStream << std::setw( distWidth ) << "dist0";
        auxStream << "  ";
        auxStream << std::setw( distWidth ) << "dist1";
        auxStream << std::endl;
        auxStream << divStream.str();
    }

    auxStream << "CdA: ( ";
    auxStream.precision( powPrecision );
    auxStream << std::fixed << std::setw( powWidth ) << fabs( JJPow );
    auxStream << ( ( JJPot >= 0 ) ? " - " : " + " );
    auxStream.precision( potPrecision );
    auxStream << std::fixed << std::setw( potWidth ) << fabs( JJPot );
    auxStream << ( ( JJKin >= 0 ) ? " - " : " + " );
    auxStream.precision( kinPrecision );
    auxStream << std::fixed << std::setw( kinWidth ) << fabs( JJKin );
    auxStream << ( ( JJrr >= 0 ) ? " - " : " + " );
    auxStream.precision( jrrPrecision );
    auxStream << std::fixed << std::setw( jrrWidth ) << fabs( JJrr );
    auxStream << " ) / ";
    auxStream.precision( aeroPrecision );
    auxStream << std::fixed << std::setw( aeroWidth ) << fabs( JJAero );
    auxStream << " = ";
    auxStream.precision( cdaPrecision );
    auxStream << std::fixed << std::setw( cdaWidth ) << curCda;
    auxStream << " | ";
    auxStream.precision( time1Precision );
    auxStream << std::fixed << std::setw( timeWidth ) << time0;
    auxStream << "  ";
    auxStream.precision( time1Precision );
    auxStream << std::fixed << std::setw( timeWidth ) << time1;
    auxStream << " | ";
    auxStream.precision( distPrecision );
    auxStream << std::fixed << std::setw( distWidth ) << dist0;
    auxStream << "  ";
    auxStream.precision( distPrecision );
    auxStream << std::fixed << std::setw( distWidth ) << dist1;
    auxStream << std::endl;

    if ( debugLevel > 0 )
    {
        debugStream << auxStream.str();
    }
    std::cerr << auxStream.str();
}

// ----------------------------------------------------------------------------------------------------
//
// outputCrr
//
// Debug function: Output Crr, time, distance and all 5 enegry intervals
//
// ----------------------------------------------------------------------------------------------------
void computationEngine::outputCrr
(
    double JJPow,
    double JJPot,
    double JJKin,
    double JJFri,
    double JJda,
    double curCrr,
    double time0,
    double time1,
    double dist0,
    double dist1
)
{
    std::stringstream auxStream;
    int curWidth = 0;

    if ( powWidth != 0 )
    {
        curWidth = powPrecision + 2 + ( int ) round( log( fabs( JJPow ) ) / log( 10.0 ) );
        if ( powWidth < curWidth )
        {
            powWidth = curWidth;
        }
    }
    if ( potWidth != 0 )
    {
        curWidth = potPrecision + 2 + ( int ) round( log( fabs( JJPot ) ) / log( 10.0 ) );
        if ( potWidth < curWidth )
        {
            potWidth = curWidth;
        }
    }
    if ( kinWidth != 0 )
    {
        curWidth = kinPrecision + 2 + ( int ) round( log( fabs( JJKin ) ) / log( 10.0 ) );
        if ( kinWidth < curWidth )
        {
            kinWidth = curWidth;
        }
    }
    if ( friWidth != 0 )
    {
        curWidth = friPrecision + 2 + ( int ) round( log( fabs( JJFri ) ) / log( 10.0 ) );
        if ( friWidth < curWidth )
        {
            friWidth = curWidth;
        }
    }
    if ( jdaWidth != 0 )
    {
        curWidth = jdaPrecision + 2 + ( int ) round( log( fabs( JJda ) ) / log( 10.0 ) );
        if ( jdaWidth < curWidth )
        {
            jdaWidth = curWidth;
        }
    }
    if ( crrWidth != 0 )
    {
        curWidth = crrPrecision + 2 + ( int ) round( log( fabs( curCrr ) ) / log( 10.0 ) );
        if ( crrWidth < curWidth )
        {
            crrWidth = curWidth;
        }
    }
    if ( timeWidth != 0 )
    {
        curWidth = time1Precision + 2 + ( int ) round( log( fabs( time1 ) ) / log( 10.0 ) );
        if ( timeWidth < curWidth )
        {
            timeWidth = curWidth;
        }
    }
    if ( distWidth != 0 )
    {
        curWidth = distPrecision + 2 + ( int ) round( log( fabs( dist1 ) ) / log( 10.0 ) );
        if ( distWidth < curWidth )
        {
            distWidth = curWidth;
        }
    }

    if ( ++crrLineCounter > crrMaxLineCount )
    {
        crrLineCounter = 0;
        auxStream << "crr: ( ";
        auxStream << std::setw( powWidth ) << "Power";
        auxStream << " - ";
        auxStream << std::setw( potWidth ) << "PotEn";
        auxStream << " - ";
        auxStream << std::setw( kinWidth ) << "KinEn";
        auxStream << " - ";
        auxStream << std::setw( friWidth ) << "FriEn";
        auxStream << " ) / ";
        auxStream << std::setw( jdaWidth ) << "JdaEn";
        auxStream << " = ";
        auxStream << std::setw( crrWidth ) << "crr";
        auxStream << " | ";
        auxStream << std::setw( timeWidth ) << "time0";
        auxStream << "  ";
        auxStream << std::setw( timeWidth ) << "time1";
        auxStream << " | ";
        auxStream << std::setw( distWidth ) << "dist0";
        auxStream << "  ";
        auxStream << std::setw( distWidth ) << "dist1";
        auxStream << std::endl;
    }

    auxStream << "crr: ( ";
    auxStream.precision( powPrecision );
    auxStream << std::fixed << std::setw( powWidth ) << fabs( JJPow );
    auxStream << ( ( JJPot >= 0 ) ? " - " : " + " );
    auxStream.precision( potPrecision );
    auxStream << std::fixed << std::setw( potWidth ) << fabs( JJPot );
    auxStream << ( ( JJKin >= 0 ) ? " - " : " + " );
    auxStream.precision( kinPrecision );
    auxStream << std::fixed << std::setw( kinWidth ) << fabs( JJKin );
    auxStream << ( ( JJFri >= 0 ) ? " - " : " + " );
    auxStream.precision( friPrecision );
    auxStream << std::fixed << std::setw( friWidth ) << fabs( JJFri );
    auxStream << " ) / ";
    auxStream.precision( jdaPrecision );
    auxStream << std::fixed << std::setw( jdaWidth ) << fabs( JJda );
    auxStream << " = ";
    auxStream.precision( crrPrecision );
    auxStream << std::fixed << std::setw( crrWidth ) << curCrr;
    auxStream << " | ";
    auxStream.precision( time1Precision );
    auxStream << std::fixed << std::setw( timeWidth ) << time0;
    auxStream << "  ";
    auxStream.precision( time1Precision );
    auxStream << std::fixed << std::setw( timeWidth ) << time1;
    auxStream << " | ";
    auxStream.precision( distPrecision );
    auxStream << std::fixed << std::setw( distWidth ) << dist0;
    auxStream << "  ";
    auxStream.precision( distPrecision );
    auxStream << std::fixed << std::setw( distWidth ) << dist1;
    auxStream << std::endl;

    if ( debugLevel > 0 )
    {
        debugStream << auxStream.str();
    }
    std::cerr << auxStream.str();
}

// ----------------------------------------------------------------------------------------------------
//
// isAeroMessageForThisRider
//
// Return 'true' is deviceID is the same as aeroSensorID, false otherrwise.
//
// ----------------------------------------------------------------------------------------------------
bool computationEngine::isAeroMessageForThisRider
(
    const amString &deviceID
)
{
    return ( deviceID == aeroSensorID );
}

// ----------------------------------------------------------------------------------------------------
//
// isAltitudeMessageForThisRider
//
// Return 'true' is deviceID is the same as altitudeSensorID, false otherrwise.
//
// ----------------------------------------------------------------------------------------------------
bool computationEngine::isAltitudeMessageForThisRider
(
    const amString &deviceID
)
{
    return ( deviceID == altitudeSensorID );
}

// ----------------------------------------------------------------------------------------------------
//
// isSpeedMessageForThisRider
//
// Return 'true' is deviceID is the same as speedSensorID, false otherrwise.
//
// ----------------------------------------------------------------------------------------------------
bool computationEngine::isSpeedMessageForThisRider
(
    const amString &deviceID
)
{
    return ( deviceID == speedSensorID );
}

// ----------------------------------------------------------------------------------------------------
//
// isPowerMessageForThisRider
//
// Return 'true' is deviceID is the same as PowerSensorID, false otherrwise.
//
// ----------------------------------------------------------------------------------------------------
bool computationEngine::isPowerMessageForThisRider
(
    const amString &deviceID
)
{
    return ( deviceID == powerSensorID );
}

// ----------------------------------------------------------------------------------------------------
//
// trapzoidAeroFunction
//
// Auxiliary function. Compute the area of the trapezoid definded by the x-axis between time0 and time1,
// the line between ( time0, 0 ) and ( time0, f(time0) ), the function path between ( time0, f(time0) )
// and ( time1, f(time1) ) and the line between ( time1, f(time1) ) and ( time1, 0 ), where
//     f( t ) = speed( t ) * airSpeed( t )^2
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::trapzoidAeroFunction
(
    double time0,
    double time1,
    double oldSum,
    int    maxIndex
)
{
    double newSum   = 0;
    double curVAir  = 0;
    double curVelo  = 0;
    double step     = 0;
    double timeX    = 0;
    double curSum   = 0;
    int    index    = 0;
    int    newIndex = 0;

    if ( debugLevel > 2 )
    {
        debugStream << "\n          trapzoidAeroFunction( " << std::fixed << time0 << ", " << std::fixed << time1;
        debugStream << ", " << std::fixed << oldSum << ", " << index << " )" << std::endl;
    }
    if ( maxIndex == 0 )
    {
        curVelo = speedQuantity.interpolate( time0 );
        if ( debugLevel > 2 )
        {
            debugStream << "              curVelo( " << std::fixed << time0 << " ) = " << std::fixed << curVelo << std::endl;
        }
        curVAir = airSpeedQuantity.interpolate( time0 );
        if ( debugLevel > 2 )
        {
            debugStream << "              curVAir( " << std::fixed << time0 << " ) = " << std::fixed << curVAir << std::endl;
        }
        newSum = curVAir * curVAir * curVelo;
        if ( debugLevel > 2 )
        {
            debugStream << "              sum_A  = " << std::fixed << newSum << std::endl;
        }

        curVelo = speedQuantity.interpolate( time1 );
        if ( debugLevel > 2 )
        {
            debugStream << "              curVelo( " << std::fixed << time0 << " ) = " << std::fixed << curVelo << std::endl;
        }
        curVAir = airSpeedQuantity.interpolate( time1 );
        if ( debugLevel > 2 )
        {
            debugStream << "              curVAir( " << std::fixed << time0 << " ) = " << std::fixed << curVAir << std::endl;
        }
        newSum += curVAir * curVAir * curVelo;
        if ( debugLevel > 2 )
        {
            debugStream << "              sum_B  = " << std::fixed << newSum << std::endl;
        }

        newSum *= 0.5 * ( time1 - time0 );
        if ( debugLevel > 2 )
        {
            debugStream << "              sum_C  = " << std::fixed << newSum << std::endl;
        }
    }
    else
    {
        newIndex = 1 << ( maxIndex - 1 );
        step     = ( time1 - time0 ) / ( double ) newIndex;
        timeX    = time0 + 0.5 * step;
        curSum   = 0;

        // This is the spacing of the points to be added.
        for ( index = 0; index < newIndex; ++index, timeX += step )
        {
            curVelo = speedQuantity.interpolate( timeX );
            curVAir = airSpeedQuantity.interpolate( timeX );
            curSum += curVAir * curVAir * curVelo;
        }

        // This replaces oldSum by its refined value.
        newSum = 0.5 * ( oldSum + step * curSum );
    }

    return newSum;
}


// ----------------------------------------------------------------------------------------------------
//
// computeAeroIntegralSimpson
//
// Auxiliary function. Use Simpson's Rule to compute the integral 
//
//          /t1
//          |   v(t) * av(t) dt    where v(t) is speed at time t, av(t) is air speed at time t.
//        t0/
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computeAeroIntegralSimpson
(
    double time0,
    double time1
)
{
    double result = 0;
    if ( time0 < time1 )
    {
//std::stringstream debStream;
//debStream << "\n          computeAeroIntegralSimpson( " << std::fixed << time0 << ", " << std::fixed << time1 << " )" << std::endl;
        int    index  = 0;
        double delta  = 0.0;
        double prev_s = 0.0;
        double prev_t = 0.0;
        double curr_t = trapzoidAeroFunction( time0, time1, prev_t, index );
        double curr_s = 4.0 * curr_t / 3.0;
//debStream << "              0 ( curr_s, curr_t ) = ( " << std::fixed << curr_s << ", " << std::fixed << curr_t << " )" << std::endl;

        for ( index = 1; index < maxSimpsonRule; ++index )
        {
            prev_t = curr_t;
            prev_s = curr_s;
            curr_t = trapzoidAeroFunction( time0, time1, prev_t, index );
            curr_s = ( 4.0 * curr_t - prev_t ) / 3.0;
            if ( index >= minSimpsonRule )
            {
                delta = fabs( curr_s - prev_s );
                if ( delta <= simpsonRuleEpsilon * fabs( prev_s ) )
                {
                    break;
                }
            }
        }
        if ( index >= maxSimpsonRule )
        {
            std::stringstream auxStream;
            auxStream << "computeAeroIntegralSimpson: Maximum number of step (" << index << ") exceeded." << std::endl;
            auxStream << "                            Time = ( " << std::fixed << time0 << ", " << std::fixed << time1 << " )" << std::endl;
            if ( debugLevel > 2 )
            {
                debugStream << auxStream.str();
            }
            if ( debugLevel > 0 )
            {
                std::cerr   << auxStream.str();
            }
        }
        result = curr_s;
    }
    return result;
}

// ----------------------------------------------------------------------------------------------------
//
// computeAeroIntegral
//
// Auxiliary function. Use Simpson's Rule to compute the integrals
//
//          /t_i+1
//          |   v(t) * av(t) dt    where v(t) is speed at time t, av(t) is air speed at time t.
//       t_i/                      and [t_0, t_1, ..., t_N] is a partition of the interval [t_0, t_N].
//                                 along the times output by the sensors.
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computeAeroIntegral
(
    double time0,
    double time1
)
{
    int    index0   = speedQuantity.findTimeIndexAbove( time0 );
    int    index1   = speedQuantity.findTimeIndexAbove( time1 );
    int    maxIndex = speedQuantity.getEndIndex();
    double t0       = time0;
    double t1       = speedQuantity.getTimeValue( index0 );
    double result   = 0;
    double integral = 0;

    while ( ( index1 < maxIndex) && ( speedQuantity.getTimeValue( index1 ) < time1 ) )
    {
        ++index1;
    }
    while ( ( index0 < maxIndex - 1 ) && ( t1 <= t0 ) )
    {
        t1 = speedQuantity.getTimeValue( ++index0 );
    }
    if ( t1 > time1 )
    {
        t1 = time1;
    }

    if ( t0 < t1 )
    {
        integral  = computeAeroIntegralSimpson( t0, t1 );
        result   += integral;
    }

    while ( index0 < index1 )
    {
        ++index0;
        t0 = t1;
        t1 = speedQuantity.getTimeValue( index0 );
        if ( !isDefined( t1 ) || ( t1 > time1 ) )
        {
            t1     = time1;
            index0 = index1;
        }
        if ( t0 < t1 )
        {
            integral  = computeAeroIntegralSimpson( t0, t1 );
            result   += integral;
        }
    }
    if ( t1 < time1 )
    {
        t0 = t1;
        t1 = time1;
        if ( t1 > t0 )
        {
            integral  = computeAeroIntegralSimpson( t0, t1 );
            result   += integral;
        }
    }
    return result;
}

// ----------------------------------------------------------------------------------------------------
//
// computeAeroEnergyIntegral
//
// Compute the Aero Energy Integral over the intrval [time0, time1].
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computeAeroEnergyIntegral
(
    double time0,
    double time1
)
{
    double curJAero = 0;
    if ( time0 != time1 )
    {
        if ( time0 > time1 )
        {
            curJAero = computeAeroEnergyIntegral( time1, time0 );
            if ( isDefined( curJAero ) )
            {
                curJAero *= -1;
            }
        }
        else if ( airSpeedQuantity.allValuesDefined( time0, time1 ) && speedQuantity.allValuesDefined( time0, time1 ) )
        {
            curJAero = computeAeroIntegral( time0, time1 );
            curJAero *= ( 0.5 * rho );
        }
        else
        {
            curJAero = C_DBL_UNDEFINED;
        }
    }
    return curJAero;
}

// ----------------------------------------------------------------------------------------------------
//
// computeKineticEnergyIntegral
//
// Compute the Kinetic Energy Integral over the intrval [time0, time1].
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computeKineticEnergyIntegral
(
    double time0,
    double time1
)
{
    double curJKin = 0;
    if ( time0 != time1 )
    {
        if ( time0 > time1 )
        {
            curJKin = computeKineticEnergyIntegral( time1, time0 );
            if ( isDefined( curJKin ) )
            {
                curJKin *= -1;
            }
        }
        else
        {
            curJKin = C_DBL_UNDEFINED;
            double speed0 = speedQuantity.interpolate( time0 );
            if ( isDefined( speed0 ) )
            {
                double speed1  = speedQuantity.interpolate( time1 );
                if ( isDefined( speed1 ) )
                {
                    curJKin = 0.5 * mass * ( ( speed1 * speed1 ) - ( speed0 * speed0 ) );
                }
            }
        }
    }
    return curJKin;
}

// ----------------------------------------------------------------------------------------------------
//
// computePotentialEnergyIntegral
//
// Compute the Potential Energy Integral over the intrval [time0, time1].
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computePotentialEnergyIntegral
(
    double time0,
    double time1
)
{
    double curJPot = 0;
    if ( time0 != time1 )
    {
        if ( time0 > time1 )
        {
            curJPot = computePotentialEnergyIntegral( time1, time0 );
            if ( isDefined( curJPot ) )
            {
                curJPot *= -1;
            }
        }
        else
        {
            curJPot = C_DBL_UNDEFINED;
            double height0 = altitudeQuantity.interpolate( time0 );
            if ( isDefined( height0 ) )
            {
                double height1 = altitudeQuantity.interpolate( time1 );
                if ( isDefined( height1 ) )
                {
                    curJPot = mass * C_GRAVITY * ( height1 - height0 );
                }
            }
        }
    }
    return curJPot;
}

// ----------------------------------------------------------------------------------------------------
//
// integratePower
//
// Auxiliary function: Compute the integrals
//
//          /t_i+1
//          |   p(t) dt    where p(t) is power at time t and [t_0, t_1, ..., t_N] is a partition
//       t_i/              of the interval [t_0, t_N] along the times output by the sensors.
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::integratePower
(
    double time0,
    double time1
)
{
    double result = C_DBL_UNDEFINED;
    if ( powerQuantity.allValuesDefined( time0, time1 ) )
    {
        result       = 0;
        int index0   = powerQuantity.findTimeIndexAbove( time0 );
        int index1   = powerQuantity.findTimeIndexAbove( time1 );
        int maxIndex = powerQuantity.getEndIndex();
        while ( ( index1 < maxIndex) && ( powerQuantity.getTimeValue( index1 ) < time1 ) )
        {
            ++index1;
        }
        double t0       = time0;
        double t1       = powerQuantity.getTimeValue( index0 );
        double p0       = powerQuantity.interpolate( t0 );
        double p1       = 0;
        double integral = 0;
        while ( ( index0 < maxIndex - 1 ) && ( t1 <= t0 ) )
        {
            t1 = powerQuantity.getTimeValue( ++index0 );
        }
        if ( t1 > time1 )
        {
            t1 = time1;
            p1 = powerQuantity.interpolate( t1 );
        }
        else
        {
            p1 = powerQuantity.getQuantityValue( index0 );
        }

        if ( t0 < t1 )
        {
            integral  = ( t1 - t0 ) * ( p0 + p1 );
            result   += integral;
        }
        while ( index0 < index1 )
        {
            t0 = t1;
            p0 = p1;
            ++index0;
            t1 = powerQuantity.getTimeValue( index0 );
            if ( t1 > time1 )
            {
                t1     = t0;
                p1     = p0;
                index0 = index1;
                break;
            }
            if ( t0 < t1 )
            {
                p1        = powerQuantity.getQuantityValue( index0 );
                integral  = ( t1 - t0 ) * ( p0 + p1 );
                result   += integral;
            }
        }

        if ( t1 < time1 )
        {
            t0        = t1;
            p0        = p1;
            t1        = time1;
            p1        = powerQuantity.interpolate( t1 );
            integral  = ( t1 - t0 ) * ( p0 + p1 );
            result   += integral;
        }
        result /= 2.0;
    }

    return result;
}

// ----------------------------------------------------------------------------------------------------
//
// computePowerEnergyIntegral
//
// Compute the Power Energy Integral over the intrval [time0, time1].
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computePowerEnergyIntegral
(
    double time0,
    double time1
)
{
    double curJPow = 0;
    if ( time0 != time1 )
    {
        if ( time0 > time1 )
        {
            curJPow = computePowerEnergyIntegral( time1, time0 );
            if ( isDefined( curJPow ) )
            {
                curJPow *= -1;
            }
        }
        else
        {
            curJPow = integratePower( time0, time1 );
            if ( isDefined( curJPow ) )
            {
                curJPow *= eta;
            }
        }
    }
    return curJPow;
}

// ----------------------------------------------------------------------------------------------------
//
// computeFrictionEnergyIntegral
//
// Compute the Friction Loss Energy Integral over the intrval [time0, time1].
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::computeFrictionEnergyIntegral
(
    double dist0,
    double dist1
)
{
    double curJFri = 0;
    if ( dist0 > dist1 )
    {
        curJFri = ( dist0 - dist1 ) * C_GRAVITY * mass;
    }
    else
    {
        curJFri = ( dist1 - dist0 ) * C_GRAVITY * mass;
    }

    return curJFri;
}

// ----------------------------------------------------------------------------------------------------
//
// computeCdaOrCrrInterval
//
// Compute the CdA and/or crr over the intergal [time0, time1].
// Compute 5 Energy intergrals
//     JPot : Potential Energy Integral 
//     JKin : Kinetic Energy Integral 
//     JFri : Friction Loss Energy Integral 
//     JAero: Aero Energy Integral 
//     JPow : Power Energy Integral 
// The 3 intergals JPot, JKin and JFri can be solved and are directly computed.
// The 2 intergals JPow and JAero must be intergated using numeric methods.
// Since the integration integral is a moving integral [ time( dist - LENGTH ), time( dist ) ]
// the 2 integrals over
//     [ t_Start_i, t_End_i ]
// can be computed from the previous intergal over
//     [ t_Start_i-1, t_End_i-1 ]
// computing the much smaller intergrals over
//     [ t_Start_i-1, t_Start_i ]
// and
//     [ t_End_i-1, t_End_i ]
// by subtracting and, repsectively adding, the smaller interals from the previous integral
//     [ t_Start_i, t_End_i ] = [ t_Start_i-1, t_End_i-1 ]
//                            - [ t_Start_i-1, t_Start_i ]
//                            + [ t_End_i-1,   t_End_i ]
// 
//
// ----------------------------------------------------------------------------------------------------
int computationEngine::computeCdaOrCrrInterval
(
    double time0,
    double time1,
    double dist0,
    double dist1
)
{
    int    result = 0;
    double JPot   = 0;
    double JKin   = 0;
    double JFri   = 0;
    double JAero1 = 0;
    double JAero2 = 0;
    double JPow1  = 0;
    double JPow2  = 0;
    double Jrr    = 0;
    double curCda = 0;
    double Jda    = 0;
    double curCrr = 0;
    if ( isDefined( time0 ) && isDefined( time1 ) )
    {
        JPot = computePotentialEnergyIntegral( time0, time1 );
        if ( isDefined( JPot ) )
        {
            JKin = computeKineticEnergyIntegral( time0, time1 );
            if ( isDefined( JKin ) )
            {
                JFri = computeFrictionEnergyIntegral( dist0, dist1 );
                if ( isDefined( JFri ) )
                {
                    JAero1 = computeAeroEnergyIntegral( prevIntervalStart, time0 );
                    if ( isDefined( JAero1 ) )
                    {
                        JAero2 = computeAeroEnergyIntegral( prevIntervalEnd, time1 );
                        if ( isDefined( JAero2 ) )
                        {
                            JPow1  = computePowerEnergyIntegral( prevIntervalStart, time0 );
                            if ( isDefined( JPow1 ) )
                            {
                                prevIntervalStart = time0;
                                JPow2  = computePowerEnergyIntegral( prevIntervalEnd, time1 );
                                if ( isDefined( JPow2 ) )
                                {
                                    prevIntervalEnd = time1;
                                    JAero -= JAero1;
                                    JAero += JAero2;
                                    JPow  -= JPow1;
                                    JPow  += JPow2;
                                    if ( debugLevel > 1 )
                                    {
                                        debugStream << "    JPot   = " << JPot << std::endl;
                                        debugStream << "    JKin   = " << JKin << std::endl;
                                        debugStream << "    JFri   = " << JFri << std::endl;
                                        debugStream << "    Jrr    = " << ( JFri * defaultCrr ) << std::endl;
                                        debugStream << "    JAero1 = " << JAero1 << std::endl;
                                        debugStream << "    JAero2 = " << JAero2 << std::endl;
                                        debugStream << "    JAero  = " << JAero << std::endl;
                                        debugStream << "    JPow1  = " << JPow1 << std::endl;
                                        debugStream << "    JPow2  = " << JPow2 << std::endl;
                                        debugStream << "    JPow   = " << JPow << std::endl;

                                        debugStream << "    computeCda = " << computeCda << std::endl;
                                        debugStream << "    computeCrr = " << computeCrr << std::endl;
                                    }
                                    if ( computeCda )
                                    {
                                        if ( debugLevel > 1 )
                                        {
                                            debugStream << "computeCdaOrCrrInterval( " << std::fixed << time0 << ", " << std::fixed << time1 << " )" << std::endl;
                                        }
                                        Jrr    = JFri  * defaultCrr;
                                        curCda = ( JPow - JPot - JKin - Jrr ) / JAero;
                                        if ( debugLevel > 1 )
                                        {
                                            debugStream << "    Jrr    = " << Jrr << std::endl;
                                            debugStream << "    CdA    = " << curCda << std::endl;
                                        }
                                        cdaQuantity.newValue( time1, curCda );
                                        outputValue( cdaSensorID, time1, curCda );
                                        outputCda( JPow, JPot, JKin, Jrr, JAero, curCda, time0, time1, dist0, dist1 );
                                    }
                                    if ( computeCrr )
                                    {
                                        Jda    = JAero * defaultCda;
                                        curCrr = ( JPow - JPot - JKin - Jda ) / JFri;
                                        crrQuantity.newValue( time1, curCrr );
                                        outputValue( crrSensorID, time1, curCrr );
                                        outputCrr( JPow, JPot, JKin, JFri, Jda, curCrr, time0, time1, dist0, dist1 );
                                    }
                                }
                                else
                                {
                                    if ( debugLevel > 1 )
                                    {
                                        debugStream << "    JPow2  = " << "<undefined>" << std::endl;
                                    }
                                    result = 256;
                                }
                            }
                            else
                            {
                                if ( debugLevel > 1 )
                                {
                                    debugStream << "    JPow1  = " << "<undefined>" << std::endl;
                                }
                                result = 128;
                            }
                        }
                        else
                        {
                            if ( debugLevel > 1 )
                            {
                                debugStream << "    JAero2 = " << "<undefined>" << std::endl;
                            }
                            result = 64;
                        }
                    }
                    else
                    {
                        if ( debugLevel > 1 )
                        {
                            debugStream << "    JAero1 = " << "<undefined>" << std::endl;
                        }
                        result = 32;
                    }
                }
                else
                {
                    if ( debugLevel > 1 )
                    {
                        debugStream << "    JFri   = " << "<undefined>" << std::endl;
                    }
                    result = 16;
                }
            }
            else
            {
                if ( debugLevel > 1 )
                {
                    debugStream << "    JKin   = " << "<undefined>" << std::endl;
                }
                result = 8;
            }
        }
        else
        {
            if ( debugLevel > 1 )
            {
                debugStream << "    JPot   = " << "<undefined>" << std::endl;
            }
            result = 4;
        }
    }
    else
    {
        if ( !isDefined( time0 ) )
        {
            result = 1;
            if ( debugLevel > 1 )
            {
                debugStream << "    time0  = " << "<undefined>" << std::endl;
            }
        }
        if ( !isDefined( time1 ) )
        {
            result |= 2;
            if ( debugLevel > 1 )
            {
                debugStream << "    time1  = " << "<undefined>" << std::endl;
            }
        }
    }
    return result;
}

// ----------------------------------------------------------------------------------------------------
//
// outputValue
//
// Output the CdA and/or crr over the intergal [time0, time1] via ZMQ.
//
// ----------------------------------------------------------------------------------------------------
void computationEngine::outputValue
(
    const amString &sensorID,
    double          timeValue,
    double          quantityValue
)
{
    if ( zmqWriter.isUp() )
    {
        amString messageHead( sensorID );
        messageHead += C_SEPARATOR_AS_STRING;
        messageHead += amString( initialTime + timeValue, timePrecision );
        messageHead += C_SEPARATOR_AS_STRING;

        amString messageFoot( C_SEPARATOR_AS_STRING );
        messageFoot += amString( rho, rhoPrecision );
        messageFoot += C_SEPARATOR_AS_STRING;
        messageFoot += C_VERSION;

        if ( sensorID.startsWith( "CDA_" ) )
        {
            amString messageCda( messageHead );
            messageCda += amString( quantityValue, cdaPrecision );
            messageCda += C_SEPARATOR_AS_STRING;
            messageCda += amString( defaultCrr, crrPrecision );
            messageCda += messageFoot;
            zmqWriter.zmqWriteMessage( messageCda );
            if ( debugLevel > 0 )
            {
                debugStream << messageCda << std::endl;
            }
        }
        if ( sensorID.startsWith( "CRR_" ) )
        {
            amString messageCrr( messageHead );
            messageCrr += amString( quantityValue, crrPrecision );
            messageCrr += C_SEPARATOR_AS_STRING;
            messageCrr += amString( defaultCda, cdaPrecision );
            messageCrr += messageFoot;
            zmqWriter.zmqWriteMessage( messageCrr );
            if ( debugLevel > 0 )
            {
                debugStream << messageCrr << std::endl;
            }
        }

    }
}

// ----------------------------------------------------------------------------------------------------
//
// processModificationCommand
//
// Syntax of the messages that are evaluated:
//   "%   <definition_code>   START_OF_DEFINITIONS
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   NAME                <name>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   ETA                 <eta>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   MASS                <mass>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   CDA_DEFAULT_VALUE   <cda>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   CRR_DEFAULT_VALUE   <crr>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   COMP_INTERVAL       3             <interval_length>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   COMPUTE_CDA         <yes_or_no>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   COMPUTE_CRR         <yes_or_no>"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   SPEED               <device_id>   [<wheel_cirumference>   [<nb_magnets>]]"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   POWER               <device_id>   [<pm_offset>            [<pm_slope>]]"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   AERO                <device_id>   [<calibration_rho>      [<multiplier>]]"
//   "%   <definition_code>   INDIVIDUAL_DEVICE   0   +   ALTITUDE            <device_id>"
//   "%   <definition_code>   GLOBAL_DEVICE       RHO                 <value>
// All other messages are ignored.
//
// Notes:
//     1. The syntax of the commands has been chosen this way to be compatible with the TAS Replay Tool.
//     2. The values for
//            <wheel_cirumference>,
//            <nb_magnets>,
//            <pm_offset>,
//            <pm_slope>,
//            <calibration_rho>
//        and
//            <multiplier>
//        are ignored (since the RAS Engine reads fully cooked data).
// ----------------------------------------------------------------------------------------------------
void computationEngine::processModificationCommand
(
    const amString &inputBuffer
)
{
    amSplitString words;
    amString      deviceID;
    amString      parameterType;
    amString      transactionType;
    int           nbWords           = words.split( inputBuffer );
    int           wordNo            = 0;
    int           curDefinitionCode = 0;
    bool          yesOrNo           = false;
    double        iValue            = 0;
    double        dValue            = 0;
    if ( debugLevel > 1 )
    {
        debugStream << "INPUT: " << inputBuffer << std::endl;
        std::cerr << "INPUT: " << inputBuffer << std::endl;
    }
    if ( ( words[ wordNo++ ] == C_MODIFICATION_STRING ) && ( nbWords > 2 ) )
    {
        curDefinitionCode = words[ wordNo++ ].toInt();
        transactionType = words[ wordNo++ ];
        if ( transactionType == C_START_OF_DEFINITIONS )
        {
            if ( definitionCode <= 0 )
            {
                definitionCode = curDefinitionCode;
            }
        }
        else if ( definitionCode == curDefinitionCode )
        {
            if ( transactionType == C_END_OF_DEFINITIONS )
            {
                definitionCode = -1;
            }
            else if ( ( definitionCode == curDefinitionCode ) && ( nbWords > 4 ) )
            {
                if ( transactionType == C_GLOBAL_DEVICE_ID )
                {
                    parameterType = words[ wordNo++ ];
                    if ( parameterType == C_CURRENT_AIR_DENSITY_ID )
                    {
                        dValue = words[ wordNo++ ].toDouble();
                        if ( dValue > 0 )
                        {
                            setCurrentAirDensity( dValue );
                            if ( debugLevel > 0 )
                            {
                                debugStream << "SET RHO         : " << rho << std::endl;
                                std::cerr << "SET RHO         : " << rho << std::endl;
                            }
                        }
                    }
                }
                else if ( ( transactionType == C_INDIVIDUAL_DEVICE ) && ( nbWords > 6 ) )
                {
                    iValue = words[ wordNo++ ].toInt();
                    if ( iValue == 0 )
                    {
                        if ( words[ wordNo++ ] == C_ADD_DEVICE_ID )
                        {
                            parameterType = words[ wordNo++ ];
                            if ( parameterType == C_RIDER_NAME_ID )
                            {
                                deviceID = words[ wordNo++ ];
                                while ( wordNo < nbWords )
                                {
                                    deviceID += " ";
                                    deviceID += words[ wordNo++ ];
                                }
                                setRiderName( deviceID );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET RIDER NAME  : " << riderName << std::endl;
                                    std::cerr << "SET RIDER NAME  : " << riderName << std::endl;
                                }
                            }
                            else if ( parameterType == C_DRIVE_TRAIN_EFFICIENCY_ID )
                            {
                                setDriveTrainEfficiency( words[ wordNo ].toDouble() );
                                etaSet = true;
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET ETA         : " << eta << std::endl;
                                    std::cerr << "SET ETA         : " << eta << std::endl;
                                }
                            }
                            else if ( parameterType == C_RIDER_AND_BIKE_MASS_ID )
                            {
                                setRiderAndBikeMass( words[ wordNo ].toDouble() );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET MASS        : " << mass << std::endl;
                                    std::cerr << "SET MASS        : " << mass << std::endl;
                                }
                            }
                            else if ( parameterType == C_DEFAULT_CDA_ID )
                            {
                                setDefaultCda( words[ wordNo ].toDouble() );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET DEFAULT CDA : " << defaultCda << std::endl;
                                    std::cerr << "SET DEFAULT CDA : " << defaultCda << std::endl;
                                }
                            }
                            else if ( parameterType == C_DEFAULT_CRR_ID )
                            {
                                setDefaultCrr( words[ wordNo ].toDouble() );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET DEFAULT CRR : " << defaultCrr << std::endl;
                                    std::cerr << "SET DEFAULT CRR : " << defaultCrr << std::endl;
                                }
                            }
                            else if ( ( parameterType == C_COMPUTE_CDA_ID ) || ( parameterType == C_COMPUTE_CRR_ID ) )
                            {
                                yesOrNo = words[ wordNo ].toBool();
                                if ( parameterType == C_COMPUTE_CDA_ID )
                                {
                                    setComputeCda( yesOrNo );
                                    if ( debugLevel > 0 )
                                    {
                                        debugStream << "SET COMPUTE CDA : " << yesOrNo << std::endl;
                                        std::cerr << "SET COMPUTE CDA : " << yesOrNo << std::endl;
                                    }
                                }
                                else
                                {
                                    setComputeCrr( yesOrNo );
                                    if ( debugLevel > 0 )
                                    {
                                        debugStream << "SET COMPUTE CRR : " << yesOrNo << std::endl;
                                        std::cerr << "SET COMPUTE CRR : " << yesOrNo << std::endl;
                                    }
                                }
                            }
                            else if ( parameterType == C_SPEED_SENSOR_ID )
                            {
                                setSpeedSensorID( words[ wordNo ] );
                                speedQuantity.setDeviceName( speedSensorID );
                                distanceQuantity.setDeviceName( speedSensorID );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET SPEED SENSOR: " << speedSensorID << std::endl;
                                    std::cerr << "SET SPEED SENSOR: " << speedSensorID << std::endl;
                                }
                            }
                            else if ( parameterType == C_POWER_SENSOR_ID )
                            {
                                setPowerSensorID( words[ wordNo ] );
                                powerQuantity.setDeviceName( powerSensorID );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET POWER SENSOR: " << powerSensorID << std::endl;
                                    std::cerr << "SET POWER SENSOR: " << powerSensorID << std::endl;
                                }
                                if ( crrSensorID.empty() )
                                {
                                    crrSensorID  = "CRR_";
                                    crrSensorID += powerSensorID;
                                    crrQuantity.setDeviceName( crrSensorID );
                                }
                                if ( cdaSensorID.empty() )
                                {
                                    cdaSensorID  = "CDA_";
                                    cdaSensorID += powerSensorID;
                                    cdaQuantity.setDeviceName( cdaSensorID );
                                }
                                if ( etaSet )
                                {
                                    if ( isWheelTorquePowerSensor( powerSensorID ) )
                                    {
                                        setDriveTrainEfficiency( C_WHEEL_TORQUE_ETA );
                                    }
                                    else
                                    {
                                        setDriveTrainEfficiency( C_CRANK_TORQUE_ETA );
                                    }
                                }
                            }
                            else if ( parameterType == C_AERO_SENSOR_ID )
                            {
                                setAeroSensorID( words[ wordNo ] );
                                airSpeedQuantity.setDeviceName( aeroSensorID );
                                yawAngleQuantity.setDeviceName( aeroSensorID );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET AERO SENSOR : " << aeroSensorID << std::endl;
                                    std::cerr << "SET AERO SENSOR : " << aeroSensorID << std::endl;
                                }
                            }
                            else if ( parameterType == C_ALTITUDE_SENSOR_ID )
                            {
                                setAltitudeSensorID( words[ wordNo ] );
                                altitudeQuantity.setDeviceName( altitudeSensorID );
                                if ( debugLevel > 0 )
                                {
                                    debugStream << "SET ALT. SENSOR : " << altitudeSensorID << std::endl;
                                    std::cerr << "SET ALT. SENSOR : " << altitudeSensorID << std::endl;
                                }
                            }
                            else if ( ( parameterType == C_COMP_INTERVAL_ID ) && ( nbWords > 7 ) )
                            {
                                iValue = words[ wordNo++ ].toInt();
                                if ( iValue == 3 )
                                {
                                    dValue = words[ wordNo++ ].toDouble();
                                    if ( dValue > 0 )
                                    {
                                        setIntervalLength( dValue );
                                        if ( debugLevel > 0 )
                                        {
                                            debugStream << "SET INTERVAL    : " << intervalLength << std::endl;
                                            std::cerr << "SET INTERVAL    : " << intervalLength << std::endl;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------------------------------
//
// timeStamp2Double
//
// Auxiliary function. Normalize the time, i.e. convert the Unix epoch time to 'current time'.
// If the offset 'initialTime' has not yet been set set it to the current time stamp.
// Subtract 'initialTime' from the current time stamp and return the result.
//
// ----------------------------------------------------------------------------------------------------
double computationEngine::timeStamp2Double
(
    const amString &timeAsString
)
{
    double timeStamp = timeAsString.toDouble();
    if ( initialTime < 0 )
    {
        initialTime = timeStamp;
        timeStamp   = 0;
    }
    else
    {
        timeStamp -= initialTime;
    }
    return timeStamp;
}

#ifndef __USE_THREADS__
// --------------------------------------------------------------------------------------------------------------
//
// readData
// Single thread version: Read ANT+ data and compute CdA and/or crr.
//
// --------------------------------------------------------------------------------------------------------------
void computationEngine::readData
(
    void
)
{
    amString      inputBuffer;
    amString      deviceID;
    amString      parameterType;
    amString      errorMessage;
    amSplitString words;
    bool          outputParams    = true;
    int           computationRes  = 0;
    int           stringLength    = 0;
    int           wordNo          = 0;
    int           nbWords         = 0;
    double        timeStamp       = 0;
    double        timeStampPrev   = 0;
    double        airSpeed        = 0;
    double        yawAngle        = 0;
    double        altitude        = 0;
    double        speed           = 0;
    double        power           = 0;
    double        distance        = 0;
    double        curDistance     = 0;
    double        prevSpeed       = 0;
    double        prevSpeedTime   = -1;
    double        timeAll         = 0;
    double        timeAllPrevious = 0;
    double        timeStart       = 0;
    double        distStart       = 0;
    double        distEnd         = 0;
    double        waitTime        = C_ZMQ_WAIT;

    inputBuffer.reserve( bufferSize );

    readActive = true;

    zmqReader.setZMQExitString( exitString );
    zmqReader.setZMQBufferSize( bufferSize );
    zmqReader.zmqConnectSUB( inIPAddress,  inPortNo,  waitTime );
    zmqWriter.zmqConnectPUB( outIPAddress, outPortNo, waitTime );

    while ( running )
    {
        readActive &= zmqReader.isUp() && zmqWriter.isUp();

        outputParams = true;
        while ( readActive )
        {
            inputBuffer.clear();
            stringLength = zmqReader.zmqReadMessage( inputBuffer, errorMessage );
            if ( stringLength > 0 )
            {
                if ( debugLevel > 1 )
                {
                    debugStream << inputBuffer << std::endl;
                }
                if ( isModificationCommand( inputBuffer ) )
                {
                    processModificationCommand( inputBuffer );
                }
                else if ( inputBuffer == C_END_OF_MESSAGES )
                {
                    startCompute = false;
                }
                else
                {
                    nbWords = words.split( inputBuffer );
                    if ( nbWords > 2 )
                    {
                        wordNo    = 0;
                        deviceID  = words[ wordNo++ ];
                        timeStamp = C_DBL_UNDEFINED;
                        airSpeed  = C_DBL_UNDEFINED;
                        yawAngle  = C_DBL_UNDEFINED;
                        altitude  = C_DBL_UNDEFINED;
                        power     = C_DBL_UNDEFINED;
                        speed     = C_DBL_UNDEFINED;
                        if ( isSpeedMessageForThisRider( deviceID ) )
                        {
                            // Start computation if power meter, aero sensor and altitude meter are defined (speed sensor has already been confirmed).
                            if ( ( debugLevel > 0 ) && outputParams )
                            {
                                outputParams = false;
                                std::cerr   << "Name       : " << riderName << std::endl;
                                debugStream << "Name       : " << riderName << std::endl;
                                std::cerr   << "Speed      : " << speedSensorID << std::endl;
                                debugStream << "Speed      : " << speedSensorID << std::endl;
                                std::cerr   << "Power      : " << powerSensorID << std::endl;
                                debugStream << "Power      : " << powerSensorID << std::endl;
                                std::cerr   << "Aero       : " << aeroSensorID << std::endl;
                                debugStream << "Aero       : " << aeroSensorID << std::endl;
                                std::cerr   << "Altitude   : " << altitudeSensorID << std::endl;
                                debugStream << "Altitude   : " << altitudeSensorID << std::endl;
                                std::cerr   << "Eta        : " << eta << std::endl;
                                debugStream << "Eta        : " << eta << std::endl;
                                std::cerr   << "Mass       : " << mass << std::endl;
                                debugStream << "Mass       : " << mass << std::endl;
                                std::cerr   << "Rho        : " << rho << std::endl;
                                debugStream << "Rho        : " << rho << std::endl;
                                if ( computeCda )
                                {
                                    std::cerr   << "Default crr: " << defaultCrr << std::endl;
                                    debugStream << "Default crr: " << defaultCrr << std::endl;
                                }
                                if ( computeCrr )
                                {
                                    std::cerr   << "Default CdA: " << defaultCda << std::endl;
                                    debugStream << "Default CdA: " << defaultCda << std::endl;
                                }
                                std::cerr   << "Interval L.: " << intervalLength << std::endl;
                                debugStream << "Interval L.: " << intervalLength << std::endl;
                            }
                            startCompute = !powerSensorID.empty() && !aeroSensorID.empty() && !altitudeSensorID.empty();
                            timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                            timeSpeed    = timeStamp;
                            timeDistance = timeStamp;
                            if ( isPowerSensor( deviceID ) )
                            {
                                //      0           1        2          3          4       ...
                                // <device_id>   <time>   <power>   <cadence>   <speed>    ...
                                if ( isPowerMessageForThisRider( deviceID ) )
                                {
                                    power     = words[ wordNo ].toDouble();
                                    timePower = timeStamp;
                                }
                                wordNo += 2;
                                speed = words[ wordNo ].toDouble();
                            }
                            else
                            {
                                //      0           1        2       ...
                                // <device_id>   <time>   <speed>    ...
                                speed = words[ wordNo ].toDouble();
                            }
                            if ( prevSpeedTime >= 0 )
                            {
                                 // Use average speed in interval to compute the distance traveled
                                 curDistance  = ( speed + prevSpeed ) * ( timeStamp - prevSpeedTime ) / 2.0;
                                 distance    += curDistance;
                            }
                            prevSpeed     = speed;
                            prevSpeedTime = timeStamp;
                        }
                        else if ( startCompute )
                        {
                            // ---------------------------------------------------------------------------------
                            // Only start collecting data once the speed sensor (i.e. distance sensor) kicks in
                            // This way time t = 0 and distance d = 0 will be at index 0 in the arrays
                            // for the different quantities.
                            // ---------------------------------------------------------------------------------
                            if ( isAeroMessageForThisRider( deviceID ) )
                            {
                                //      0           1          2             3         ...
                                // <device_id>   <time>   <air_speed>   <yaw_angle>    ...
                                timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                                airSpeed     = words[ wordNo++ ].toDouble();
                                yawAngle     = words[ wordNo   ].toDouble();
                                timeAirSpeed = timeStamp;
                                timeYawAngle = timeStamp;
                            }
                            else if ( isAltitudeMessageForThisRider( deviceID ) )
                            {
                                //      0           1        2          ...
                                // <device_id>   <time>   <altitude>    ...
                                timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                                altitude     = words[ wordNo ].toDouble();
                                timeAltitude = timeStamp;
                            }
                            else if ( isPowerMessageForThisRider( deviceID ) )
                            {
                                //      0           1        2          3       ...
                                // <device_id>   <time>   <power>   <cadence>   ...
                                timeStamp = timeStamp2Double( words[ wordNo++ ] );
                                power     = words[ wordNo ].toDouble();
                            }
                            else if ( debugLevel > 0 )
                            {
                                std::cerr << "        Unknown Device " << deviceID << std::endl;
                                debugStream << "        Unknown Device " << deviceID << std::endl;
                            }
                        }

                        if ( isDefined( timeStamp ) )
                        {
                            if ( isDefined( power ) )
                            {
                                powerQuantity.newValue( timeStamp, power );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                powerQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }

                            if ( isDefined( speed ) )
                            {
                                speedQuantity.newValue( timeStamp, speed );
                                distanceQuantity.newValue( timeStamp, distance );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                speedQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                                distanceQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }

                            if ( isDefined( airSpeed ) )
                            {
                                airSpeedQuantity.newValue( timeStamp, airSpeed );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                airSpeedQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }

                            if ( isDefined( yawAngle ) )
                            {
                                yawAngleQuantity.newValue( timeStamp, yawAngle );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                yawAngleQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }

                            if ( isDefined( altitude ) )
                            {
                                altitudeQuantity.newValue( timeStamp, altitude );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                altitudeQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }

                            if ( isDefined( timeAirSpeed ) && isDefined( timeSpeed ) && isDefined( timeAltitude ) &&
                                 isDefined( timeYawAngle ) && isDefined( timePower ) && isDefined( timeDistance ) )
                            {
                                // Compute the maximum time for which all values exist.
                                timeStampPrev = timeStamp;
                                timeAll       = timeAirSpeed;
                                if ( timeAll > timeYawAngle )
                                {
                                    timeAll = timeYawAngle;
                                }
                                if ( timeAll > timeAltitude )
                                {
                                    timeAll = timeAltitude;
                                }
                                if ( timeAll > timeSpeed )
                                {
                                    timeAll = timeSpeed;
                                }
                                if ( timeAll > timePower )
                                {
                                    timeAll = timePower;
                                }
                                if ( timeAll > timeDistance )
                                {
                                    timeAll = timeDistance;
                                }

                                if ( timeAll > timeAllPrevious )
                                {
                                    // -------------------------------------------------------------
                                    // New maximum time: Compute CdA

                                    // -------------------------------------------------------------
                                    // First, determine the interval over which to integrate
                                    distEnd = distanceQuantity.interpolate( timeAll );
                                    if ( distEnd <= intervalLength )
                                    {
                                        distStart = 0;
                                    }
                                    else
                                    {
                                        distStart = distEnd - intervalLength;
                                    }

                                    // -------------------------------------------------------------
                                    // Convert the interval boundaries from distance to time.
                                    timeStart = distanceQuantity.interpolateTime( distStart );;

                                    // -------------------------------------------------------------
                                    // Compute the CdA (or crr) value for the current interval
                                    computationRes = computeCdaOrCrrInterval( timeStart, timeAll, distStart, distEnd );
                                    if ( computationRes == 0 )
                                    {
                                        timeAllPrevious = timeAll;
                                    }
                                    else
                                    {
                                        std::cerr << "COMPUTATION ERROR " << computationRes << " [ " << timeStart << ", " << timeAll << " ]"  << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // -------------------------------------------------------------
        // Wait before the next query
        msleep( sleepTime );
    }
}
#else // __USE_THREADS__
// --------------------------------------------------------------------------------------------------------------
//
// readAntMessages and computeCdaOrCrr
//
// These are the functions that are executed by the two threads created in the main program 'ras'.
//
// The two threads (read and compute) are managed by 4 boolean variables
//    1. running     : Drives the outer loop of both threads.
//                     If 'running == true'  the two threads are active and wait for further instructions.
//                     If 'running == false' the two threads (and the main program) are ended.
//    2. startReading: Drives the inner loop of the read thread.
//                     If 'startReading == true' the read thread reads ZMQ data.
//                     What it does with the data depends on the value of 'startCollect' (see 3).
//                     If 'startReading == false' the inner loop of the read thread is ended and the read thread
//                     continues in the outer loop waiting for 'startReading' to be set to 'true'.
//                     The value of 'startReading' is evaluated as
//                         startReading = readActive && zmqReader.isUp() && zmqWrite.isUp().
//                     The value of 'readActive' can be set from the main program.
//    3. startCollect: Drives the evaluation of ANT+ data.
//                     'startCollect' is set to 'true'  when the symbol C_START_OF_MESSAGES is read.
//                     'startCollect' is set to 'false' when the symbol C_END_OF_MESSAGES   is read.
//                     If 'startCollect == true'  the read thread evaluates the ANT+ data that were read.
//                     If 'startCollect == false' the read thread only reads data until 'C_START_OF_MESSAGES' is read.
//    4. startCompute: Drives the inner loop of the compute thread.
//                     'startCompute' is set to 'true' when 'startCollect' == true and the first distance message is read
//                     (in our case this is a SPEED sensor message since distance is computed from speed).
//                     'startCompute' is set to 'false' when 'startCollect' is set to 'false' (see 3. above).
//
// --------------------------------------------------------------------------------------------------------------
void computationEngine::readAntMessages
(
    void
)
{
    amString      inputBuffer;
    amString      deviceID;
    amString      parameterType;
    amString      errorMessage;
    amSplitString words;
    bool          startReading    = false;
    bool          startCollect    = false;
    bool          outputParams    = true;
    int           stringLength    = 0;
    int           wordNo          = 0;
    int           nbWords         = 0;
    int           nbFailConnectR  = 0;
    int           nbFailConnectW  = 0;
    double        timeStamp       = 0;
    double        timeStampPrev   = 0;
    double        airSpeed        = 0;
    double        yawAngle        = 0;
    double        altitude        = 0;
    double        speed           = 0;
    double        power           = 0;
    double        distance        = 0;
    double        curDistance     = 0;
    double        prevSpeed       = 0;
    double        prevSpeedTime   = -1;
    double        timeAll         = 0;
    double        timeAllPrevious = 0;
    double        timeStart       = 0;
    double        distStart       = 0;
    double        distEnd         = 0;
    double        waitTime        = C_ZMQ_WAIT;

    inputBuffer.reserve( bufferSize );
    zmqReader.setZMQExitString( exitString );
    zmqReader.setZMQBufferSize( bufferSize );

    while ( running )
    {
        startReading = readActive;
        if ( !zmqReader.isUp() )
        {
            zmqReader.zmqConnectSUB( inIPAddress,  inPortNo,  waitTime );
            if ( zmqReader.isUp() )
            {
                nbFailConnectR = 0;
            }
            else
            {
                ++nbFailConnectR;
                std::cerr << "zmqReader: not up." << std::endl;
                std::cerr << zmqReader.getErrorMessage() << std::endl;
                startReading = false;
            }
        }
        if ( !zmqWriter.isUp() )
        {
            zmqWriter.zmqConnectPUB( outIPAddress, outPortNo, waitTime );
            if ( zmqWriter.isUp() )
            {
                nbFailConnectW = 0;
            }
            else
            {
                ++nbFailConnectW;
                std::cerr << "zmqWriter: not up." << std::endl;
                std::cerr << zmqWriter.getErrorMessage() << std::endl;
                startReading = false;
            }
        }

        outputParams = true;
        while ( startReading )
        {
            inputBuffer.clear();
            stringLength = zmqReader.zmqReadMessage( inputBuffer, errorMessage );
            if ( stringLength > 0 )
            {
                if ( debugLevel > 2 )
                {
                    debugStream << inputBuffer << std::endl;
                }
                if ( isModificationCommand( inputBuffer ) )
                {
                    processModificationCommand( inputBuffer );
                }
                else if ( inputBuffer == C_START_OF_MESSAGES )
                {
                    reset();
                    startCompute = false;
                    startCollect = true;
                }
                else if ( inputBuffer == C_END_OF_MESSAGES )
                {
                    startCompute = false;
                    startCollect = false;
                }
                else if ( startCollect )
                {
                    nbWords = words.split( inputBuffer );
                    if ( nbWords > 2 )
                    {
                        wordNo    = 0;
                        deviceID  = words[ wordNo++ ];
                        timeStamp = C_DBL_UNDEFINED;
                        airSpeed  = C_DBL_UNDEFINED;
                        yawAngle  = C_DBL_UNDEFINED;
                        altitude  = C_DBL_UNDEFINED;
                        power     = C_DBL_UNDEFINED;
                        speed     = C_DBL_UNDEFINED;
                        if ( isSpeedMessageForThisRider( deviceID ) )
                        {
                            // Start computation if power meter, aero sensor and altitude meter are defined (speed sensor has already been confirmed).
                            if ( ( debugLevel > 0 ) && outputParams )
                            {
                                outputParams = false;
                                std::stringstream auxStream;
                                auxStream << "Name       : " << riderName << std::endl;
                                auxStream << "Speed      : " << speedSensorID << std::endl;
                                auxStream << "Power      : " << powerSensorID << std::endl;
                                auxStream << "Aero       : " << aeroSensorID << std::endl;
                                auxStream << "Altitude   : " << altitudeSensorID << std::endl;
                                auxStream << "Eta        : " << eta << std::endl;
                                auxStream << "Mass       : " << mass << std::endl;
                                auxStream << "Rho        : " << rho << std::endl;
                                if ( computeCda )
                                {
                                    auxStream << "Default crr: " << defaultCrr << std::endl;
                                }
                                if ( computeCrr )
                                {
                                    auxStream << "Default CdA: " << defaultCda << std::endl;
                                }
                                auxStream << "Interval L.: " << intervalLength << std::endl;
                                debugStream << std::endl << auxStream.str() << std::endl;
                                std::cerr   << std::endl << auxStream.str() << std::endl;
                            }
                            startCompute = !powerSensorID.empty() && !aeroSensorID.empty() && !altitudeSensorID.empty();
                            timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                            timeSpeed    = timeStamp;
                            timeDistance = timeStamp;
                            if ( isPowerSensor( deviceID ) )
                            {
                                //      0           1        2          3          4       ...
                                // <device_id>   <time>   <power>   <cadence>   <speed>    ...
                                if ( isPowerMessageForThisRider( deviceID ) )
                                {
                                    power     = words[ wordNo ].toDouble();
                                    timePower = timeStamp;
                                }
                                wordNo += 2;
                                speed = words[ wordNo ].toDouble();
                            }
                            else
                            {
                                //      0           1        2       ...
                                // <device_id>   <time>   <speed>    ...
                                speed = words[ wordNo ].toDouble();
                            }
                            if ( prevSpeedTime >= 0 )
                            {
                                 // Use average speed in interval to compute the distance traveled
                                 curDistance  = ( speed + prevSpeed ) * ( timeStamp - prevSpeedTime ) / 2.0;
                                 distance    += curDistance;
                            }
                            prevSpeed     = speed;
                            prevSpeedTime = timeStamp;
                        }
                        else if ( startCompute )
                        {
                            // ---------------------------------------------------------------------------------
                            // Only start collecting data once the speed sensor (i.e. distance sensor) kicks in
                            // This way time t = 0 and distance d = 0 will be at index 0 in the arrays
                            // for the different quantities.
                            // ---------------------------------------------------------------------------------
                            if ( isAeroMessageForThisRider( deviceID ) )
                            {
                                //      0           1          2             3         ...
                                // <device_id>   <time>   <air_speed>   <yaw_angle>    ...
                                timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                                airSpeed     = words[ wordNo++ ].toDouble();
                                yawAngle     = words[ wordNo   ].toDouble();
                                timeAirSpeed = timeStamp;
                                timeYawAngle = timeStamp;
                            }
                            else if ( isAltitudeMessageForThisRider( deviceID ) )
                            {
                                //      0           1        2          ...
                                // <device_id>   <time>   <altitude>    ...
                                timeStamp    = timeStamp2Double( words[ wordNo++ ] );
                                altitude     = words[ wordNo ].toDouble();
                                timeAltitude = timeStamp;
                            }
                            else if ( isPowerMessageForThisRider( deviceID ) )
                            {
                                //      0           1        2          3       ...
                                // <device_id>   <time>   <power>   <cadence>   ...
                                timeStamp = timeStamp2Double( words[ wordNo++ ] );
                                power     = words[ wordNo ].toDouble();
                            }
                            else if ( debugLevel > 0 )
                            {
                                debugStream << "        Unknown Device " << deviceID << std::endl;
                            }
                        }

                        if ( isDefined( timeStamp ) )
                        {
                            powerQuantity.lockMainMutex( true, "new value" );
                            if ( isDefined( power ) )
                            {
                                if ( debugLevel > 3 )
                                {
                                    std::cerr << "    power = " << power << "    words = " << words << std::endl;
                                }
                                powerQuantity.newValue( timeStamp, power );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                powerQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }
                            powerQuantity.unlockMainMutex( true );

                            distanceQuantity.lockMainMutex( true, "new value" );
                            speedQuantity.lockMainMutex( true, "new value" );
                            if ( isDefined( speed ) )
                            {
                                if ( debugLevel > 3 )
                                {
                                    std::cerr << "    speed = " << speed << "    words = " << words << std::endl;
                                }
                                speedQuantity.newValue( timeStamp, speed );
                                distanceQuantity.newValue( timeStamp, distance );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                speedQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                                distanceQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }
                            speedQuantity.unlockMainMutex( true );
                            distanceQuantity.unlockMainMutex( true );

                            airSpeedQuantity.lockMainMutex( true, "new value" );
                            if ( isDefined( airSpeed ) )
                            {
                                if ( debugLevel > 3 )
                                {
                                    std::cerr << "    airSpeed = " << airSpeed << "    words = " << words << std::endl;
                                }
                                airSpeedQuantity.newValue( timeStamp, airSpeed );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                airSpeedQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }
                            airSpeedQuantity.unlockMainMutex( true );

                            yawAngleQuantity.lockMainMutex( true, "new value" );
                            if ( isDefined( yawAngle ) )
                            {
                                yawAngleQuantity.newValue( timeStamp, yawAngle );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                yawAngleQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }
                            yawAngleQuantity.unlockMainMutex( true );

                            altitudeQuantity.lockMainMutex( true, "new value" );
                            if ( isDefined( altitude ) )
                            {
                                if ( debugLevel > 3 )
                                {
                                    std::cerr << "    altitude = " << altitude << "    words = " << words << std::endl;
                                }
                                altitudeQuantity.newValue( timeStamp, altitude );
                            }
                            else if ( timeStampPrev != timeStamp )
                            {
                                // Only insert for a new time value
                                altitudeQuantity.insert( timeStamp, C_DBL_UNDEFINED );
                            }
                            altitudeQuantity.unlockMainMutex( true );
                        }
                    }
                }
            }
        }
        // -------------------------------------------------------------
        // Wait before the next query
        msleep( sleepTime );
    }
}

void computationEngine::computeCdaOrCrr
(
    void
)
{
    bool   computeOK       = false;
    double timeStamp       = 0;
    double timeStampPrev   = 0;
    double airSpeed        = 0;
    double yawAngle        = 0;
    double altitude        = 0;
    double speed           = 0;
    double power           = 0;
    double distance        = 0;
    double curDistance     = 0;

    double timeAll         = 0;
    double timeAllPrevious = 0;
    double timeStart       = 0;
    double distStart       = 0;
    double distEnd         = 0;
    double waitTime        = 5;

    int    computationRes  = 0;

    while ( running )
    {
        timeStamp       = 0;
        timeStampPrev   = 0;
        airSpeed        = 0;
        yawAngle        = 0;
        altitude        = 0;
        speed           = 0;
        power           = 0;
        distance        = 0;
        curDistance     = 0;
        timeAll         = 0;
        timeAllPrevious = 0;
        timeStart       = 0;
        distStart       = 0;
        distEnd         = 0;
        waitTime        = 5;

        while ( startCompute )
        {
            computeOK = false;
            if ( isDefined( timeAirSpeed ) && isDefined( timeSpeed ) && isDefined( timeAltitude ) &&
                 isDefined( timeYawAngle ) && isDefined( timePower ) && isDefined( timeDistance ) )
            {
                // Compute the maximum time for which all values exist.
                timeStampPrev = timeStamp;
                timeAll       = timeAirSpeed;
                if ( timeAll > timeYawAngle )
                {
                    timeAll = timeYawAngle;
                }
                if ( timeAll > timeAltitude )
                {
                    timeAll = timeAltitude;
                }
                if ( timeAll > timeSpeed )
                {
                    timeAll = timeSpeed;
                }
                if ( timeAll > timePower )
                {
                    timeAll = timePower;
                }
                if ( timeAll > timeDistance )
                {
                    timeAll = timeDistance;
                }

                if ( timeAll > timeAllPrevious )
                {
                    // -------------------------------------------------------------
                    // New maximum time: Compute CdA

                    // -------------------------------------------------------------
                    // First, determine the interval over which to integrate
                    distanceQuantity.lockMainMutex( true, "compute" );
                    distEnd = distanceQuantity.interpolate( timeAll );
                    if ( isDefined( distEnd ) )
                    {
                        if ( distEnd <= intervalLength )
                        {
                            distStart = 0;
                        }
                        else
                        {
                            distStart = distEnd - intervalLength;
                        }

                        // -------------------------------------------------------------
                        // Convert the interval boundaries from distance to time.
                        timeStart = distanceQuantity.interpolateTime( distStart );;
                        distanceQuantity.unlockMainMutex( true );
                        if ( isDefined( timeStart ) )
                        {
                            // -------------------------------------------------------------
                            // Compute the CdA (or crr) value for the current interval
                            computeOK = true;
                            powerQuantity.lockMainMutex( true, "compute" );
                            speedQuantity.lockMainMutex( true, "compute" );
                            airSpeedQuantity.lockMainMutex( true, "compute" );
                            yawAngleQuantity.lockMainMutex( true, "compute" );
                            altitudeQuantity.lockMainMutex( true, "compute" );

                            computationRes = computeCdaOrCrrInterval( timeStart, timeAll, distStart, distEnd );

                            powerQuantity.unlockMainMutex( true );
                            speedQuantity.unlockMainMutex( true );
                            airSpeedQuantity.unlockMainMutex( true );
                            yawAngleQuantity.unlockMainMutex( true );
                            altitudeQuantity.unlockMainMutex( true );

                            if ( computationRes == 0 )
                            {
                                timeAllPrevious = timeAll;
                            }
                            else if ( debugLevel > 2 )
                            {
                                std::cerr << "COMPUTATION ERROR " << computationRes << " [ " << timeStart << ", " << timeAll << " ]"  << std::endl;
                            }
                        }
                    }
                    else
                    {
                        distanceQuantity.unlockMainMutex( true );
                    }
                }
            }
        }
        if ( !computeOK )
        {
            msleep( waitTime );
        }
    }
}
#endif // __USE_THREADS__

