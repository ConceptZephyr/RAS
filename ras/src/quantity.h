#ifndef __QUANTITY_H__
#define __QUANTITY_H__

#include <am_constants.h>

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>

enum amQuantityType
{
    SPEED,
    CG_SPEED,
    POWER,
    TORQUE,
    CADENCE,
    ALTITUDE,
    YAW_ANGLE,
    LEAN_ANGLE,
    HEART_RATE,
    HEAD_WIND,
    CDA,
    CRR,
    AIR_SPEED,
    DISTANCE,
    UNDEFINED_QUANTITY
};

class quantity
{
    public:

        // --------------------------------------------------------------------------------------------
        // Public variables
        // --------------------------------------------------------------------------------------------

        // --------------------------------------------------------------------------------------------
        // Public methods
        // --------------------------------------------------------------------------------------------

        quantity( amQuantityType thisQuantity = UNDEFINED_QUANTITY );
                                          // Constructor
        ~quantity() {}                    // Destructor

        void reset( void );               // Reset all arrays and some variables
        void stopCollecting( void );      // Fill all undefined values through interpolation and/or extrapolation

        void lockMainMutex  ( bool lockMutex, const std::string &label );
                                          // Lock the internal mutex
        void unlockMainMutex( bool lockMutex );
                                          // Unlock the internal mutex

        void newValue( double timeVal, double quantityVal );
                                          // Add a new value pair
        int  insert  ( double timeVal, double quantityVal );
                                          // Insert a new value pair
                                          // Note: The difference between 'insert' and 'newValue' is that 'insert' mereley inserts the time and quantity
                                          //       value in the approprate spot (with repsect to time) in the arrays wheera 'newValue' calls 'insert'
                                          //       and then tries to fill in undefined quantity values through interpolation.
                                          //       As a rule, call 'newValue' when a new value from the sensor is to be added
                                          //                  call 'insert' when a new time with an undefined value is to be added
                                          //                  (for keeping the quantity data arrays for different quatities in synch).

        double interpolate( double timeVal ) const;
                                          // Interpolate the quantity value for the given time value
        double interpolateTime( double quantityValue ) const;
                                          // Interpolate the time value for the given quantity value
                                          // Note: The finction defined by (time, quantity) must be invertible, i.e. monotonous

        inline double getTimeValue    ( int index ) const { return ( ( index >= 0 ) && ( timeArray.size() > index ) ) ? timeArray[ index ] : C_DBL_UNDEFINED; }
                                          // Return timeArray[ index ] (if defined)
        inline double getQuantityValue( int index ) const { return ( ( index >= 0 ) && ( valueArray.size() > index ) ) ? valueArray[ index ] : C_DBL_UNDEFINED; }
                                          // Return valueArray[ index ] (if defined)

        int    getLastCompleteIndex    ( void ) const;
                                          // Return the maximum index for which all quantities up to this index are defined
        double getLastCompleteTimeValue( void ) const;
                                          // Return the maximum time value for which all quantities up to this time value are defined

        inline int getEndIndex( void ) const { return valueArray.size(); }
                                          // Return the size of the two arrays

        inline double getFirstTime( void ) const { return ( timeArray.size() > 0 ? timeArray.front() : C_DBL_UNDEFINED ); }
                                          // Return the first value in timeArray.
        inline double getLastTime( void ) const { return ( timeArray.size() > 0 ? timeArray.back() : C_DBL_UNDEFINED ); }
                                          // Return the last value in timeArray.

        inline double getFirstValue( void ) const { return ( valueArray.size() > 0 ? valueArray.front() : C_DBL_UNDEFINED ); }
                                          // Return the first value in valueArray.
        inline double getLastValue( void ) const { return ( valueArray.size() > 0 ? valueArray.back() : C_DBL_UNDEFINED ); }
                                          // Return the last value in valueArray.

        inline const std::string &getDeviceName( void ) const { return deviceName; }
                                          // Return the name of the device that produces the data
        inline void setDeviceName( const std::string &value ) { deviceName = value; }
                                          // Set the name of the device that produces the data

        inline const std::string &getQuantityName( void ) const { return quantityName; }
                                          // Return the name of quantity of the data that are stored
        inline amQuantityType getQuantityType( void ) const { return quantityType; }
                                          // Return the quantity of the data that are stored
        void setQuantityType( amQuantityType newType );
                                          // Set the quantity of the data that are stored

        inline double getMinValue( void ) const { return minValue; }
                                          // Return the minimal value in the valueArray
        inline double getMaxValue( void ) const { return maxValue; }
                                          // Return the maximal value in the valueArray

        double getMinValue( double startTime, double endTime ) const;
                                          // Return the minimal value in the valueArray between the indexes that correspond to startTime and endTime
        double getMaxValue( double startTime, double endTime ) const;
                                          // Return the maximal value in the valueArray between the indexes that correspond to startTime and endTime

        void computeMinAndMaxValues( double startTime, double endTime, double &minimum, double &maximum ) const;
                                          // Compute the minimal and maximal value in the valueArray between the indexes that correspond to startTime and endTime

        inline void setAllocationChunk( int value )  { allocationChunk = value; }
                                          // Return the allocation chunk size
        inline int  getAllocationChunk( void ) const { return allocationChunk; }
                                          // Set the allocation chunk size

        inline void setOutputMutex( bool value ) { outputMutex = value; }
                                          // Turn mutex debug information output ON/OFF
        inline void setMaxLockMutex( int value ) { maxLockMutex = value; }
                                          // Set the maximum number of trials to unlock an already locked mutex
        inline void setMutexSleepInterval( double value ) { mutexSleepInterval = ( std::chrono::duration<double, std::milli> ) value; }
                                          // Set the wait time between unsuccessful trials to unlock an already locked mutex

        void outputArrays( std::ostream &outStream = std::cerr, int startIndex = 0, int endIndex = -1 ) const;
        inline void setIndent( const std::string &value ) { indent = value; }

        int findTimeIndexAbove( double timeValue ) const;
        bool allValuesDefined( double startTime, double endTime ) const;

    private:

        // --------------------------------------------------------------------------------------------
        // Private variables
        // --------------------------------------------------------------------------------------------

        amQuantityType      quantityType;   // Type of quantity stored in this data structure
        std::string         quantityName;   // Name of the quantity type  stored in this data structure
        std::string         deviceName;     // Name (ID) of the ANT+ device that created the data stored in this data structure
        std::string         indent;         // Indent for output Arrays function

        std::vector<double> valueArray;     // Array to store quantity data
        std::vector<double> timeArray;      // Array to time  quantity data. Both arrays are aligned (have same length).

        double              maxValue;       // Overall minumum quantity value
        double              minValue;       // Overall maxumum quantity value

        int                 allocationChunk;
                                          // Chunk of new data to be allocated once current capacity is exhausted
        int                 monotonous;   // Indicator whether or not the function defined by (time, quantity) is monotonous.
                                          //    1: strictly monotonously rising : if t[i] < t[j] then q[i] < q[j] 
                                          //    2: strictly monotonously falling: if t[i] < t[j] then q[i] > q[j] 
                                          //    4: not strictly monotonous (i.e. there are pairs (t1, q1), (t2, q2) with t1 != t1 but q1 == q2
                                          //  Notes: (1) If monotonous & 3 == 3 The function cannot be inverted
                                          //         (2) If monotonous & 7 == 5 or 6: The function is monotonous, but not strictly monotonous.
                                          //             Technically it cannot be inverted, but it can be 'quasi-inverted' by choosing the
                                          //             minimum time value for the given quantity

        int                 indexLastDefinedValue;
                                          // Index of the last defined quantitity value
                                          // In order to keep data structures for different quantities in synch
                                          // values ( t, <undefined> ) may be added.

        std::mutex          mainMutex;    // Mutex to prevent simultaneous access by read and write functions
        std::string         mainMutexTag; // Tag of the last mutex lock
        bool                outputMutex;  // If true output a message to stderr if an already locked mutex is to be locked again
        int                 maxLockMutex; // Maximum number of trials to lock an already locked mutex:
                                          //     Wait, then hope the lock has been unlocked, try to lock again
        std::chrono::duration<double, std::milli> mutexSleepInterval;
                                          // Wait time (in milliseconds) between attempts to lock an already locked mutex.

        // --------------------------------------------------------------------------------------------
        // Private methods
        // --------------------------------------------------------------------------------------------

        int findInterval( double timeValue, int &indexLow, int &indexHigh ) const;
                                          // Find the indices of the enlcosing interval for a time value
                                          // Depending on return value, indexLow and indexHigh are defined by
                                          // timeArray[ indexLow ] < timeValue < timeArray[ indexHigh ]
                                          //   0: Time array is empty: indexLow = indexHigh = 0
                                          //   1: Time value is equal to a point in the time array
                                          //      indexLow = indexHigh and and timeValue = timeArray[ indexLow ] = timeArray[ indexHigh ]
                                          //   2: Time value is not equal to any point in the time array
                                          //      indexLow = indexHigh - 1 and and timeArray[ indexLow ] < timeValue < timeArray[ indexHigh ]
                                          //  -1: Time value is less than the first value in the time array: indexLow = indexHigh = 0
                                          //  -2: Time value is greater than the last value in the time array: indexLow = indexHigh = timeValue.size()

        int findQuantityInterval( double quantityValue, int &indexLow, int &indexHigh ) const;
                                          // Auxilairy function that finds the indices of the enlcosing interval for a time value

        void increaseCapacity( int minSize );
                                          // Increase the capacity by allocationChunk if minSize exceeds the current capacity


        bool fillInMissingValuesAtStart( void );
                                          // Fill in missing quantity values at the start of the valueArray through extrapolation (if possible)
                                          // Return true if successful

        bool fillInMissingValuesAtEnd( void );
                                          // Fill in missing quantity values at the end of the valueArray through extrapolation (if possible)
                                          // Return true if successful

        bool fillInMissingValuesAtCenter( void );
                                          // Fill in missing quantity values inside the valuesArray through interpolation (if possible)
                                          // Return true if successful
};

extern bool testAlignment
(
    int          startIndex,
    int          endIndex,
    std::string &errorMessage,
    quantity    *quantity1,
    quantity    *quantity2 = NULL,
    quantity    *quantity3 = NULL,
    quantity    *quantity4 = NULL,
    quantity    *quantity5 = NULL,
    quantity    *quantity6 = NULL,
    quantity    *quantity7 = NULL,
    quantity    *quantity8 = NULL
);

extern std::string quantityTypeName
(
    amQuantityType quantityType
);

#endif // __QUANTITY_H__
