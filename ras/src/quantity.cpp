#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>

#include <am_util.h>
#include <am_constants.h>
#include <algorithm>
#include <thread>

#include "quantity.h"

#define C_QUANTITY_ALLOCATION_CHUNK ( 1 << 10 )
#define C_MAX_LOCK_MUTEX            1000;
#define C_MUTEX_SLEEP_INTERVAL      10

const char C_ARRAYS_INDENT[] = "    ";





#if 0
std::string fileName( quantityName );
fileName += ".dbg";
std::ofstream ooo;
ooo.open( fileName.c_str(), std::ios_base::app );
ooo.close();
#endif




quantity::quantity
(
    amQuantityType thisQuantity
)
{
    quantityType       = thisQuantity;
    quantityName       = quantityTypeName( quantityType );
    deviceName         = "<unkown>";
    allocationChunk    = C_QUANTITY_ALLOCATION_CHUNK;
    indent             = C_ARRAYS_INDENT;
    maxLockMutex       = C_MAX_LOCK_MUTEX;
    mutexSleepInterval = ( std::chrono::duration<double, std::milli> ) C_MUTEX_SLEEP_INTERVAL;
    outputMutex        = false;
    reset();
}

void quantity::reset
(
   void
)
{
    minValue              = C_MAX_DBL_VALUE;
    maxValue              = C_MIN_DBL_VALUE;
    monotonous            = 0;
    indexLastDefinedValue = -1;
    unlockMainMutex( true );
    valueArray.clear();
    timeArray.clear();
    increaseCapacity( 0 );
}

void quantity::setQuantityType
(
    amQuantityType newType
)
{
    quantityType = newType;
    quantityName = quantityTypeName( quantityType );
}

int quantity::findInterval
(
    double  timeValue,
    int    &indexLow,
    int    &indexHigh
) const
{
    int result = 0; //  0: Empty Array (low = high = 0)
                    //  1: equal to point in array (low = high and time = timeArray[ low ])
                    //  2: not equal to point in array (low = high - 1 and timeArray[ low ] < time < timeArray[ high ])
                    // -1: Outside of array: below
                    // -2: Outside of array: above

    indexLow = indexHigh = 0;

    if ( timeArray.size() > 0 )
    {
        std::vector<double>::const_iterator higher = std::lower_bound( timeArray.begin(), timeArray.end(), timeValue );
        if ( higher == timeArray.end() )
        {
            result = -2;
            indexLow = indexHigh = timeArray.size();
        }
        else if ( higher == timeArray.begin() )
        {
            if ( timeValue < timeArray[ 0 ] )
            {
                result = -1;
            }
            else
            {
                result = 1;
            }
        }
        else
        {
            indexLow = indexHigh = ( int ) ( higher - timeArray.begin() );
            if ( timeValue == *higher )
            {
                result = 1;
            }
            else
            {
                result = 2;
                --indexLow;
            }
        }
    }

    return result;
}

double quantity::interpolate
(
    double timeValue
) const
{
    double result     = C_DBL_UNDEFINED;
    double deltaQ     = 0;
    double deltaT     = 0;
    int    indexLow   = 0;
    int    indexHigh  = 0;
    int    findResult = findInterval( timeValue, indexLow, indexHigh );

    switch ( findResult )
    {
        case  0: //  0: Empty Array (low = high = 0)
        case -1: // -1: Outside of array: below
        case -2: // -2: Outside of array: above
                 break;
        case  1: //  1: equal to point in array (low = high and time = timeArray[ low ])
                 result = valueArray[ indexLow ];
                 break;
        default: //  2: not equal to point in array (low = high - 1 and timeArray[ low ] < time < timeArray[ high ])
                 deltaQ = valueArray[ indexHigh ] - valueArray[ indexLow ];
                 deltaT = timeArray    [ indexHigh ] - timeArray    [ indexLow ];
                 result = valueArray[ indexLow ] + ( timeValue - timeArray[ indexLow ] ) * deltaQ / deltaT;
                 break;
    }
    return result;
}

void quantity::increaseCapacity
(
    int minSize
)
{
    if ( timeArray.capacity() <= minSize )
    {
        int newCapacity = minSize + allocationChunk;

        timeArray.reserve( newCapacity );
        valueArray.reserve( newCapacity );
    }
}

void quantity::lockMainMutex
(
    bool               lockMutex,
    const std::string &label
)
{
    while ( lockMutex )
    {
        // try to lock mutex
        if ( mainMutex.try_lock() )
        {
            mainMutexTag = label;
            break;
        }

        // can't get lock to modify 'job_shared'
        // but there is some other work to do
        if ( outputMutex )
        {
            std::cerr << "\"Trying to lock quantity mutex [" << quantityName << "] for \"";
            std::cerr << label << "\": Already locked by \"" << mainMutexTag << "\"." << std::endl;
        }
        std::this_thread::sleep_for( mutexSleepInterval );
    }
}

void quantity::unlockMainMutex
(
    bool lockMutex
)
{
    if ( lockMutex )
    {
        mainMutex.unlock();
    }
}

void quantity::newValue
(
    double timeValue,
    double quantityValue
)
{
    int position = insert( timeValue, quantityValue );

    if ( isDefined( quantityValue ) )
    {
        if ( maxValue < quantityValue )
        {
            maxValue = quantityValue;
        }
        if ( minValue > quantityValue )
        {
            minValue = quantityValue;
        }

        if ( ( valueArray.size() > 0 ) && !isDefined( valueArray.front() ) )
        {
            fillInMissingValuesAtStart();
        }

        if ( indexLastDefinedValue >= 0 )
        {
            double startTime  = timeArray[ indexLastDefinedValue ];
            double startValue = valueArray[ indexLastDefinedValue ];
            double deltaT     = timeValue     - startTime;
            double deltaV     = quantityValue - startValue;

            double slope      = deltaV / deltaT;
            double curValue   = 0;
            double curTime    = 0;

            if ( ( monotonous & 3 ) != 3 )
            {
                if ( quantityValue > startValue )
                {
                    monotonous |= 1;
                }
                else if ( quantityValue < startValue )
                {
                    monotonous |= 2;
                }
                else
                {
                    monotonous |= 4;
                }
            }

            ++indexLastDefinedValue;
            while ( indexLastDefinedValue < position )
            {
                curTime  = timeArray[ indexLastDefinedValue ];
                curValue = startValue + ( curTime - startTime ) * slope;
                valueArray[ indexLastDefinedValue ] = curValue;
                ++indexLastDefinedValue;
            }
        }
        else
        {
            indexLastDefinedValue = position;
        }
    }
}

int quantity::insert
(
    double timeValue,
    double quantityValue
)
{
    int position = 0;
    increaseCapacity( timeArray.size() );

    if ( timeArray.empty() || ( timeValue > timeArray.back() ) )
    {
        position = timeArray.size();
        timeArray.push_back( timeValue );
        valueArray.push_back( quantityValue );
    }
    else
    {
        std::vector<double>::const_iterator higher = std::lower_bound( timeArray.begin(), timeArray.end(), timeValue );
        position = ( int ) ( higher - timeArray.begin() );
        if ( timeValue == timeArray[ position ] )
        {
            valueArray[ position ] = quantityValue;
        }
        else
        {
            timeArray.insert( higher, timeValue );
            higher = valueArray.begin() + position;
            valueArray.insert( higher, quantityValue );
        }
    }

    if ( position <= indexLastDefinedValue )
    {
        while ( !isDefined( valueArray[indexLastDefinedValue ] ) )
        {
            --indexLastDefinedValue;
        }
    }

    return position;
}

bool quantity::fillInMissingValuesAtStart
(
    void
)
{
    bool result   = false;
    int  counter1 = 0;
    for ( counter1 = 0; counter1 < timeArray.size(); ++counter1 )
    {
        if ( isDefined( valueArray[ counter1 ] ) )
        {
            result = true;
            break;
        }
    }

    if ( result )
    {
        int    counter2 = 0;
        int    counter  = 0;
        double deltaT   = 0;
        double deltaV   = 0;
        double slope    = 0;

        result = false;
        for ( counter2 = counter1 + 1; counter2 < timeArray.size(); ++counter2 )
        {
            if ( isDefined( valueArray[ counter2 ] ) )
            {
                result = true;
                deltaT = timeArray[ counter2 ]  - timeArray[ counter1 ];
                deltaV = valueArray[ counter2 ] - valueArray[ counter1 ];
                slope  = deltaV / deltaT;
                break;
            }
        }

        if ( result )
        {
            for ( counter = 0; counter < counter1; ++counter )
            {
                valueArray[ counter ] = valueArray[ counter1 ] + ( timeArray[ counter ] - timeArray[ counter1 ] ) * slope;
            }
        }
    }
    return result;
}

bool quantity::fillInMissingValuesAtEnd
(
    void
)
{
    bool result   = false;
    int  counter2 = 0;

    for ( counter2 = timeArray.size() - 1; counter2 > 0; --counter2 )
    {
        if ( isDefined( valueArray[ counter2 ] ) )
        {
            result = true;
            break;
        }
    }

    if ( result )
    {
        int    counter1 = 0;
        int    counter  = 0;
        double deltaT   = 0;
        double deltaV   = 0;
        double slope    = 0;

        result = false;
        for ( counter1 = counter2 - 1; counter1 > 0; --counter1 )
        {
            if ( isDefined( valueArray[ counter1 ] ) )
            {
                result = true;
                deltaT = timeArray[ counter2 ]  - timeArray[ counter1 ];
                deltaV = valueArray[ counter2 ] - valueArray[ counter1 ];
                slope  = deltaV / deltaT;
                break;
            }
        }

        if ( result )
        {
            for ( counter = counter2 + 1; counter < timeArray.size(); ++counter )
            {
                valueArray[ counter ] = valueArray[ counter1 ] + ( timeArray[ counter ] - timeArray[ counter1 ] ) * slope;
            }
        }
    }
    return result;
}

bool quantity::fillInMissingValuesAtCenter
(
    void
)
{
    bool   result       = false;
    int    counter      = 0;
    int    counter1     = 0;
    int    counter2     = 0;
    double deltaT       = 0;
    double deltaV       = 0;
    double slope        = 0;
    for ( counter1 = 0; counter1 < timeArray.size(); ++counter1 )
    {
        if ( isDefined( valueArray[ counter2 ] ) )
        {
            result = true;
            break;
        }
    }

    if ( result )
    {
        result = false;
        for ( counter2 = counter1 + 1; counter2 < timeArray.size(); ++counter2 )
        {
            if ( isDefined( valueArray[ counter2 ] ) )
            {
                result = true;
                deltaT       = timeArray[ counter2 ]  - timeArray[ counter1 ];
                deltaV       = valueArray[ counter2 ] - valueArray[ counter1 ];
                slope        = deltaV / deltaT;
                for ( counter = counter1 + 1; counter < counter2; ++counter )
                {
                    valueArray[ counter ] = valueArray[ counter1 ] + ( timeArray[ counter ] - timeArray[ counter1 ] ) * slope;
                }
            }
            counter1 = counter2;
        }
    }
    return result;
}

void quantity::stopCollecting
(
    void
)
{
    fillInMissingValuesAtStart();
    fillInMissingValuesAtCenter();
    fillInMissingValuesAtEnd();
}

int quantity::findQuantityInterval
(
    double  quantityValue,
    int    &indexLow,
    int    &indexHigh
) const
{
    int result = 0; //  0: Empty Array (low = high = 0)
                    //  1: equal to point in array (low = high and time = valueArray[ low ])
                    //  2: not equal to point in array (low = high - 1 and valueArray[ low ] < quantityValue < valueArray[ high ])
                    // -1: Outside of array: below
                    // -2: Outside of array: above

    if ( ( monotonous & 3 ) != 3 )
    {
        indexLow = indexHigh = 0;

        if ( valueArray.size() > 0 )
        {
            std::vector<double>::const_iterator higher = std::lower_bound( valueArray.begin(), valueArray.end(), quantityValue );
            if ( higher == valueArray.end() )
            {
                result = -2;
            }
            else if ( higher == valueArray.begin() )
            {
                if ( quantityValue < valueArray[ 0 ] )
                {
                    result = -1;
                }
                else
                {
                    result = 1;
                }
            }
            else
            {
                indexLow = indexHigh = ( int ) ( higher - valueArray.begin() );
                if ( quantityValue == *higher )
                {
                    result = 1;
                }
                else
                {
                    result = 2;
                    --indexLow;
                }
            }
        }
    }
    while ( ( result > 0 ) && ( indexLow > 0 ) && ( quantityValue == valueArray[ indexLow - 1 ] ) )
    {
        --indexLow;
    }

    return result;
}

double quantity::interpolateTime
(
    double quantityValue
) const
{
    double result    = C_DBL_UNDEFINED;
    if ( ( monotonous & 3 ) != 3 )
    {
        double deltaQ     = 0;
        double deltaT     = 0;
        int    indexLow   = 0;
        int    indexHigh  = 0;
        int    findResult = findQuantityInterval( quantityValue, indexLow, indexHigh );

        switch ( findResult )
        {
            case  0: //  0: Empty Array (low = high = 0)
            case -1: // -1: Outside of array: below
            case -2: // -2: Outside of array: above
                     break;
            case  1: //  1: equal to point in array (low = high and quantityValue = valueArray[ low ])
                     result = timeArray[ indexLow ];
                     break;
            default: //  2: not equal to point in array (low = high - 1 and valueArray[ low ] < quantityValue < valueArray[ high ])
                     deltaQ = valueArray[ indexHigh ] - valueArray[ indexLow ];
                     deltaT = timeArray [ indexHigh ] - timeArray [ indexLow ];
                     result = timeArray[ indexLow ] + ( quantityValue - valueArray[ indexLow ] ) * deltaT / deltaQ;
                     break;
        }
    }
else std::cerr << "NOT MONOTONOUS" << std::endl;
    return result;
}

void quantity::outputArrays
(
    std::ostream &outStream,
    int           startIndex,
    int           endIndex
) const
{
    if ( endIndex < 0 )
    {
        endIndex = timeArray.size();
    }
    if ( startIndex < 0 )
    {
        startIndex += endIndex;
        if ( startIndex < 0 )
        {
            startIndex = 0;
        }
    }
    int curWidth = 1;
    if ( endIndex > 0 )
    {
        curWidth += ( int ) floor( log( endIndex ) / log( 10.0 ) );
    }
    for ( int index = startIndex; index < endIndex; ++index )
    {
        outStream << indent << std::setw( curWidth ) << index << ": [ " << std::fixed << timeArray[ index ] << ", " << valueArray[ index ] << " ]" << std::endl;
    }
}

int quantity::findTimeIndexAbove
(
    double timeValue
) const
{
    int indexLow  = 0;
    int indexHigh = 0;
    int result    = findInterval( timeValue, indexLow, indexHigh );
    if ( result <= 0 )
    {
        result = 0;
    }
    else if ( result == 1 )
    {
        result = indexHigh + 1;
    }
    else
    {
        result = indexHigh;
    }
    return result;
}

bool quantity::allValuesDefined
(
    double startTime,
    double endTime
) const
{
    int startIndex = findTimeIndexAbove( startTime );
    int endIndex   = findTimeIndexAbove( endTime );
    if ( startIndex > 0 )
    {
        --startIndex;
    }

    bool result = ( startIndex < endIndex );
    for ( int index = startIndex; result && ( index < endIndex ); ++index )
    {
        result = isDefined( valueArray[ index ] );
    }
    return result;
}

bool testAlignment
(
    int          startIndex,
    int          endIndex,
    std::string &errorMessage,
    quantity    *quantity1,
    quantity    *quantity2,
    quantity    *quantity3,
    quantity    *quantity4,
    quantity    *quantity5,
    quantity    *quantity6,
    quantity    *quantity7,
    quantity    *quantity8
)
{
    bool result = false;

    return result;
}

std::string quantityTypeName
(
    amQuantityType quantityType
)
{
    if ( quantityType == SPEED )
    {
        return "SPEED";
    }
    else if ( quantityType == CG_SPEED )
    {
        return "CG_SPEED";
    }
    else if ( quantityType == POWER )
    {
        return "POWER";
    }
    else if ( quantityType == TORQUE )
    {
        return "TORQUE";
    }
    else if ( quantityType == CADENCE )
    {
        return "CADENCE";
    }
    else if ( quantityType == ALTITUDE )
    {
        return "ALTITUDE";
    }
    else if ( quantityType == YAW_ANGLE )
    {
        return "YAW_ANGLE";
    }
    else if ( quantityType == LEAN_ANGLE )
    {
        return "LEAN_ANGLE";
    }
    else if ( quantityType == HEART_RATE )
    {
        return "HEART_RATE";
    }
    else if ( quantityType == HEAD_WIND )
    {
        return "HEAD_WIND";
    }
    else if ( quantityType == CDA )
    {
        return "CDA";
    }
    else if ( quantityType == CRR )
    {
        return "CRR";
    }
    else if ( quantityType == AIR_SPEED )
    {
        return "AIR_SPEED";
    }
    else if ( quantityType == DISTANCE )
    {
        return "DISTANCE";
    }
    return "UNDEFINED";
}

