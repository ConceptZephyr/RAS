#include <unistd.h>
#include <iostream>
#include <fstream>

#include <pthread.h>

#include <am_constants.h>
#include <zmq_constants.h>

#include <am_util.h>
#include <am_string.h>
#include <am_split_string.h>

#include "ras_constants.h" 
#include "computation_engine.h" 

const char C_PROGRAM_NAME[] = "ras";
const char C_PREFERENCE_FILE[] = ".ras.rc";

static void help
(
    void
)
{
    std::cout << std::endl;
    std::cout << "The program \"" << C_PROGRAM_NAME << "\"";
    std::cout << std::endl;
    std::cout << "    reads (fully-cooked) ANT+ data for a rider from a ZMQ port,";
    std::cout << std::endl;
    std::cout << "    computes the CdA (or crr) in real time";
    std::cout << std::endl;
    std::cout << "    and outputs the result on another ZMQ port.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    In order to compute the Cda (crr) the \"" << C_PROGRAM_NAME << "\" needs the data from";
    std::cout << std::endl;
    std::cout << "        a Power Meter";
    std::cout << std::endl;
    std::cout << "        a Speed Sensor";
    std::cout << std::endl;
    std::cout << "        a Wind Speed Sensor";
    std::cout << std::endl;
    std::cout << "        an Altitude Meter.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " takes the following arguments:";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -b <buffer_size>";
    std::cout << std::endl;
    std::cout << "       Set the size of the ZMQ buffer to <buffer_size>.";
    std::cout << std::endl;
    std::cout << "       Default: <buffer_size> = ";
    std::cout << C_ZMQ_BUFFER_SIZE;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -B";
    std::cout << std::endl;
    std::cout << "       Output Version and Build numbers.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -C <compute_cda>";
    std::cout << std::endl;
    std::cout << "       If <compute_cda> == 'yes' or 'true' compute CdA.";
    std::cout << std::endl;
    std::cout << "       Default: <compute_cda> = ";
    std::cout << ( C_COMPUTE_CDA_DEFAULT ? "'yes'." : "'no'." );
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -c <compute_crr>";
    std::cout << std::endl;
    std::cout << "       If <compute_crr> == 'yes' or 'true' compute crr.";
    std::cout << std::endl;
    std::cout << "       Default: <compute_crr> = ";
    std::cout << ( C_COMPUTE_CRR_DEFAULT ? "'yes'." : "'no'." );
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -d <device_file>";
    std::cout << std::endl;
    std::cout << "       Read definitions of values used in the CdA (crr) computation from the file <device_file>.";
    std::cout << std::endl;
    std::cout << "       If no definitions are given the default values apply.";
    std::cout << std::endl;
    std::cout << "            Current Air Density          : ";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -D";
    std::cout << std::endl;
    std::cout << "       Increase debug level by 1 (accumulative).";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_DEFAULT_AIR_DENISTY;
    std::cout << " kg/m^3.";
    std::cout << std::endl;
    std::cout << "            Drive Train Efficiency       : ";
    std::cout << C_DEFAULT_ETA;
    std::cout << ".";
    std::cout << std::endl;
    std::cout << "            Mass of Rider and Bike       : ";
    std::cout << C_DEFAULT_MASS;
    std::cout << " kg.";
    std::cout << std::endl;
    std::cout << "            Computation Interval Length  : ";
    std::cout << C_DEFAULT_INTERVAL_LENGTH;
    std::cout << " m.";
    std::cout << std::endl;
    std::cout << "            Default CdA (crr Computation): ";
    std::cout << C_DEFAULT_CDA;
    std::cout << " m^2.";
    std::cout << std::endl;
    std::cout << "            Default crr (CdA Computation): ";
    std::cout << C_DEFAULT_CRR;
    std::cout << ".";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -h";
    std::cout << std::endl;
    std::cout << "       Display this message.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -l <label>";
    std::cout << std::endl;
    std::cout << "       Gie a name (or label) to a ZMQ connection.";
    std::cout << std::endl;
    std::cout << "       The '-l' option is without effect, its purpose is merely for better readibilty.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -p <port_no>";
    std::cout << std::endl;
    std::cout << "       Set the input ZMQ port number to <port_no>.";
    std::cout << std::endl;
    std::cout << "       Default: <port_no> = ";
    std::cout << C_RAS_INPUT_PORT_NUMBER;
    std::cout << ".";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -P <port_no>";
    std::cout << std::endl;
    std::cout << "       Set the output ZMQ port number to <port_no>.";
    std::cout << std::endl;
    std::cout << "       Default: <port_no> = ";
    std::cout << C_RAS_OUTPUT_PORT_NUMBER;
    std::cout << ".";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -v";
    std::cout << std::endl;
    std::cout << "       Output Version (short form).";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -V";
    std::cout << std::endl;
    std::cout << "       Output Version (long form).";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -x <zmq_exit_string>";
    std::cout << std::endl;
    std::cout << "       Set the ZMQ exit string to <zmq_exit_string>.";
    std::cout << std::endl;
    std::cout << "       Default: <zmq_exit_string> = \"";
    std::cout << C_ZMQ_EXIT_STRING;
    std::cout << "\".";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -z <ip_address>";
    std::cout << std::endl;
    std::cout << "       Set the input ZMQ IP address to <ip_addess>.";
    std::cout << std::endl;
    std::cout << "       Default: <ip_address> = \"";
    std::cout << C_RAS_INPUT_IP_ADDRESS;
    std::cout << "\".";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "    -Z <ip_address>";
    std::cout << std::endl;
    std::cout << "       Set the output ZMQ IP address to <ip_addess>.";
    std::cout << std::endl;
    std::cout << "       Default: <ip_address> = \"";
    std::cout << C_RAS_OUTPUT_IP_ADDRESS;
    std::cout << "\".";
    std::cout << std::endl;
    std::cout << std::endl;
}

#ifdef __USE_THREADS__
static void *computeCdaOrCrr
(
    void *inputPtr
)
{
    if ( inputPtr != NULL )
    {
        computationEngine *rasEnginePtr = static_cast<computationEngine *>( inputPtr );
        if ( rasEnginePtr->getDebugLevel() > 0 )
        {
            std::cerr << std::endl << "computeCdaOrCrr START THREAD" << std::endl;
        }
        rasEnginePtr->computeCdaOrCrr();
        if ( rasEnginePtr->getDebugLevel() > 0 )
        {
            std::cerr << std::endl << "computeCdaOrCrr END THREAD" << std::endl;
        }
    }
    return NULL;
}

static void *readAntMessages
(
    void *inputPtr
)
{
    if ( inputPtr != NULL )
    {
        computationEngine *rasEnginePtr = static_cast<computationEngine *>( inputPtr );
        if ( rasEnginePtr->getDebugLevel() > 0 )
        {
            std::cerr << std::endl << "readAntMessages START THREAD" << std::endl;
        }
        rasEnginePtr->readAntMessages();
        if ( rasEnginePtr->getDebugLevel() > 0 )
        {
            std::cerr << std::endl << "readAntMessages END THREAD" << std::endl;
        }
    }
    return NULL;
}
#endif // __USE_THREADS__


int main
(
    int   argc,
    char *argv[]
)
{
    const char    optionString[] = "b:Bc:C:Dd:hl:p:P:vVz:Z:";
    bool          running             = true;
    bool          computeCda          = C_COMPUTE_CDA_DEFAULT;
    bool          computeCrr          = C_COMPUTE_CRR_DEFAULT;
    int           debugLevel          = 0;
    int           errorCode           = 0;
    int           option              = 0;
    int           nbWords             = 0;
    int           resultThread        = 0;
    int           bufferSize          = C_ZMQ_BUFFER_SIZE;
    int           portNumberIn        = C_RAS_INPUT_PORT_NUMBER;
    int           portNumberOut       = C_RAS_OUTPUT_PORT_NUMBER;
    amString      ipAddressIn         = C_RAS_INPUT_IP_ADDRESS;
    amString      ipAddressOut        = C_RAS_OUTPUT_IP_ADDRESS;
    amString      exitString          = C_ZMQ_EXIT_STRING;
    amString      preferencesFileName = C_PREFERENCE_FILE;
    amString      buffer              = "";
    amString      deviceFileName      = "";
    amString      auxString;
    amString      errorMessage;
    amSplitString words;
    std::ifstream inStream;
    pthread_t     readThread;  // Thread which reads sensor data and stores them into data structures
    pthread_t     compThread;  // Thread which takes data from the data structures and performs computations


    while ( running && ( option = getopt( argc, argv, optionString ) ) != -1 )
    {
        switch ( option )
        {
            case 'b':
                 bufferSize = atoi( optarg );
                 break;
            case 'c':
                 computeCrr = evalBoolArg( optarg );
                 break;
            case 'C':
                 computeCda = evalBoolArg( optarg );
                 break;
            case 'B':
                 std::cout << C_VERSION << ":" << C_BUILD_NUMBER << std::endl;
                 running = false;
                 break;
            case 'D':
                 ++debugLevel;
                 break;
            case 'd':
                 deviceFileName = optarg;
                 break;
            case 'h':
                 help();
                 running = false;
                 break;
            case 'l':
                 break;
            case 'p':
                 portNumberIn = atoi( optarg );
                 break;
            case 'P':
                 portNumberOut = atoi( optarg );
                 break;
            case 'v':
                 std::cout << C_VERSION << std::endl;
                 running = false;
                 break;
            case 'V':
                 std::cout << C_PROGRAM_NAME << " " << C_VERSION << std::endl;
                 std::cout << C_COPYRIGHT << std::endl;
                 running = false;
                 break;
            case 'X':
                 exitString = optarg;
                 break;
            case 'z':
                 ipAddressIn = optarg;
                 break;
            case 'Z':
                 ipAddressOut = optarg;
                 break;
            case '?':
                 errorCode = E_UNKNOWN_OPTION;
                 running   = false;
                 break;
            default:
                 errorCode = E_UNKNOWN_OPTION;
                 running   = false;
                 break;
        }   
    }


    if ( running )
    {
        computationEngine rasEngine;
        if ( !deviceFileName.empty() )
        {
            inStream.open( deviceFileName );
            if ( inStream.good() )
            {
                while ( inStream.good() && !inStream.eof() )
                {
                    std::getline( inStream, buffer );
                    if ( !buffer.empty() )
                    {
                        rasEngine.processModificationCommand( buffer );
                    }
                }
                inStream.close();
            }
            else
            {
                errorMessage += "\nCould not open file \"";
                errorMessage += deviceFileName;
                errorMessage += "\" for reading.";
                errorCode     = E_READ_FILE_NOT_OPEN;
                running       = false;
            }
        }

        if ( running )
        {
            inStream.open( preferencesFileName );
            if ( inStream.good() )
            {
                while ( inStream.good() && !inStream.eof() )
                {
                    std::getline( inStream, buffer );
                    if ( !buffer.empty() )
                    {
                        nbWords = words.split( buffer );
                        if ( nbWords > 1 )
                        {
                            if ( words[ 0 ] == "RHO_PRECISION" )
                            {
                                rasEngine.setRhoPrecision( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "CDA_PRECISION" )
                            {
                                rasEngine.setCdaPrecision( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "TIME_PRECISION" )
                            {
                                rasEngine.setTimePrecision( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "CRR_PRECISION" )
                            {
                                rasEngine.setCrrPrecision( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "SLEEP_TIME" )
                            {
                                rasEngine.setSleepTime( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "MIN_SIMPSON_RULE" )
                            {
                                rasEngine.setMinSimpsonRule( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "MAX_SIMPSON_RULE" )
                            {
                                rasEngine.setMaxSimpsonRule( words[ 1 ].toInt() );
                            }
                            else if ( words[ 0 ] == "SIMPSON_RULE_EPSILON" )
                            {
                                rasEngine.setSimpsonRuleEpsilon( words[ 1 ].toDouble() );
                            }
                        }
                    }
                }
                inStream.close();
            }
        }

        if ( running )
        {
            rasEngine.setInputIPAddress ( ipAddressIn   );
            rasEngine.setOutputIPAddress( ipAddressOut  );
            rasEngine.setInputPortNo    ( portNumberIn  );
            rasEngine.setOutputPortNo   ( portNumberOut );
            rasEngine.setBufferSize     ( bufferSize    );
            rasEngine.setComputeCrr     ( computeCrr    );
            rasEngine.setComputeCda     ( computeCda    );
            rasEngine.setExitString     ( exitString    );
            rasEngine.setReadActive     ( true          );
            rasEngine.setDebugLevel     ( debugLevel    );

#ifdef __USE_THREADS__
            resultThread = pthread_create( &readThread, NULL, readAntMessages, ( void * ) &rasEngine );
            if ( resultThread )
            {
                errorCode = E_CREATE_THREAD;
                running = false;
                errorMessage += "\nCould not create \"readAntMessages Thread\"";
            }
        }
        if ( running )
        {
            msleep( C_ZMQ_WAIT );
            resultThread = pthread_create( &compThread, NULL, computeCdaOrCrr, ( void * ) &rasEngine );
            if ( resultThread )
            {
                errorCode = E_CREATE_THREAD;
                running = false;
                errorMessage += "\nCould not create \"computeCdaOrCrr Thread\"";
            }
        }
        if ( running )
        {
            pthread_join( compThread, NULL );
            pthread_join( readThread, NULL );
            pthread_exit( NULL );
            pthread_kill( readThread, SIGKILL );
            pthread_kill( compThread, SIGKILL );
#else // __USE_THREADS__
            rasEngine.readData();
#endif // __USE_THREADS__
        }
    }

    std::cerr << C_PROGRAM_NAME;
    if ( errorCode )
    {
        std::cerr << " exited with error " << errorCode << std::endl;
        if ( !errorMessage.empty() )
        {
            std::cerr << errorMessage << std::endl;
        }
    }
    else
    {
        std::cerr << " exited without error." << std::endl;
    }
}

