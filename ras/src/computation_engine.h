#ifndef __COMPUTATION_ENGINE_H__
#define __COMPUTATION_ENGINE_H__

#include "quantity.h"

#include <am_string.h>
#include <zmq_read.h>
#include <zmq_write.h>

class computationEngine
{
    public:

        computationEngine( void );
        ~computationEngine( void ) { }

        inline void setReadActive( bool value ) { readActive = value; }
        inline void setRunning   ( bool value ) { running    = value; }
        inline void setComputeCda( bool value ) { computeCda = value; }
        inline void setComputeCrr( bool value ) { computeCrr = value; }

        inline void setBufferSize   ( int value ) { bufferSize    = value; }
        inline void setRhoPrecision ( int value ) { rhoPrecision  = value; }
        inline void setCdaPrecision ( int value ) { cdaPrecision  = value; }
        inline void setTimePrecision( int value ) { timePrecision = value; }
        inline void setCrrPrecision ( int value ) { crrPrecision  = value; }

        inline void setDefaultCrr          ( double value ) { defaultCrr     = value; }
        inline void setDefaultCda          ( double value ) { defaultCda     = value; }
        inline void setDriveTrainEfficiency( double value ) { eta            = value; }
        inline void setCurrentAirDensity   ( double value ) { rho            = value; }
        inline void setRiderAndBikeMass    ( double value ) { mass           = value; }
        inline void setIntervalLength      ( double value ) { intervalLength = value; }

        inline void setRiderName       ( const amString &value ) { riderName        = value; }
        inline void setSpeedSensorID   ( const amString &value ) { speedSensorID    = value; }
        inline void setPowerSensorID   ( const amString &value ) { powerSensorID    = value; }
        inline void setAeroSensorID    ( const amString &value ) { aeroSensorID     = value; }
        inline void setAltitudeSensorID( const amString &value ) { altitudeSensorID = value; }
        inline void setCdaSensorID     ( const amString &value ) { cdaSensorID      = value; }
        inline void setCrrSensorID     ( const amString &value ) { crrSensorID      = value; }
        inline void setExitString      ( const amString &value ) { exitString       = value; }

        inline void setInputIPAddress ( const amString &value ) { inIPAddress  = value; }
        inline void setOutputIPAddress( const amString &value ) { outIPAddress = value; }
        inline void setInputPortNo    ( int value )             { inPortNo     = value; }
        inline void setOutputPortNo   ( int value )             { outPortNo    = value; }

        inline void setSleepTime     ( int value ) { sleepTime      = value; }
        inline void setMinSimpsonRule( int value ) { minSimpsonRule = value; }
        inline void setMaxSimpsonRule( int value ) { maxSimpsonRule = value; }
        inline void setSimpsonRuleEpsilon( double value ) { simpsonRuleEpsilon = value; }

        inline void setDebugLevel( int value ) { debugLevel = value; }
        inline int  getDebugLevel( void ) const { return debugLevel; }

        void processModificationCommand( const amString &inputBuffer );
#       ifdef __USE_THREADS__
        void readAntMessages( void );
        void computeCdaOrCrr( void );
#       else // __USE_THREADS__
        void readData( void );
#       endif // __USE_THREADS__


    private:

        amString riderName;
        amString speedSensorID;
        amString powerSensorID;
        amString aeroSensorID;
        amString altitudeSensorID;
        amString crrSensorID;
        amString cdaSensorID;
        amString exitString;

        quantity crrQuantity;
        quantity cdaQuantity;
        quantity speedQuantity;
        quantity powerQuantity;
        quantity distanceQuantity;
        quantity airSpeedQuantity;
        quantity yawAngleQuantity;
        quantity altitudeQuantity;

        double rho;
        double eta;
        double mass;
        double defaultCda;
        double defaultCrr;
        double intervalLength;
        double JAero;
        double JPow;
        double timeAirSpeed;
        double timeYawAngle;
        double timeAltitude;
        double timeSpeed;
        double timePower;
        double timeDistance;
        double prevIntervalStart;
        double prevIntervalEnd;
        double simpsonRuleEpsilon;
        double initialTime;

        int definitionCode;
        int sleepTime;
        int bufferSize;
        int minSimpsonRule;
        int maxSimpsonRule;

        bool etaSet;
        bool computeCda;
        bool computeCrr;
        bool readActive;
        bool startCompute;
        bool running;

        amString inIPAddress;
        amString outIPAddress;
        int      inPortNo;
        int      outPortNo;
        zmqRead  zmqReader;
        zmqWrite zmqWriter;

        void reset( void );

        bool isAeroMessageForThisRider    ( const amString &deviceID );
        bool isAltitudeMessageForThisRider( const amString &deviceID );
        bool isSpeedMessageForThisRider   ( const amString &deviceID );
        bool isPowerMessageForThisRider   ( const amString &deviceID );

        double timeStamp2Double( const amString &timeAsString );
        int  computeCdaOrCrrInterval( double time0, double time1, double dist0, double dist1 );
        void outputValue( const amString &sensorID, double timeValue, double quantityValue );

        double trapzoidAeroFunction          ( double time0, double time1, double oldSum, int maxIndex );
        double computeAeroIntegralSimpson    ( double time0, double time1 );
        double computeAeroIntegral           ( double time0, double time1 );
        double computeKineticEnergyIntegral  ( double time0, double time1 );
        double computeAeroEnergyIntegral     ( double time0, double time1 );
        double computePotentialEnergyIntegral( double time0, double time1 );
        double computePowerEnergyIntegral    ( double time0, double time1 );
        double integratePower                ( double time0, double time1 );
        double computeFrictionEnergyIntegral ( double time0, double time1 );


        bool          writeLock;
        int           debugLevel;
        int           cdaMaxLineCount;
        int           crrMaxLineCount;
        int           cdaLineCounter;
        int           crrLineCounter;
        std::ofstream debugStream;

        void outputCda( double JJPow, double JJPot, double JJKin, double JJrr,  double JJAero, double curCda, double time0, double time1, double dist0, double dist1 );
        void outputCrr( double JJPow, double JJPot, double JJKin, double JJFri, double JJda,   double curCrr, double time0, double time1, double dist0, double dist1 );

        int powPrecision;
        int potPrecision;
        int kinPrecision;
        int jrrPrecision;
        int aeroPrecision;
        int cdaPrecision;
        int timePrecision;
        int time1Precision;
        int distPrecision;
        int friPrecision;
        int jdaPrecision;
        int crrPrecision;
        int rhoPrecision;

        int powWidth;
        int potWidth;
        int kinWidth;
        int jrrWidth;
        int aeroWidth;
        int cdaWidth;
        int timeWidth;
        int distWidth;
        int friWidth;
        int jdaWidth;
        int crrWidth;
};

#endif // __COMPUTATION_ENGINE_H__


