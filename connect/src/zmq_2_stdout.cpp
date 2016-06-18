// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// zmq2stdout                                                                                                                     //
//                                                                                                                                //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// Main function that reads data from ZMQ on the given IP address and port and outputs them to stdout.                            //
//                                                                                                                                //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //

#include <iostream>
#include <unistd.h>

#include <vector>

#include <am_constants.h>
#include <am_string.h>
#include <am_util.h>

#include "connect_util.h"
#include "zmq_constants.h"
#include "zmq_version.h"
#include "zmq_read.h"

const char C_PROGRAM_NAME[] = "zmq2stdout";

static void help
(
   void
)
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " reads data from the ZMQ connection with the given IP address and port number and writes them to stdout.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " takes the following arguments:";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -B <buffer_size>";
    std::cout << std::endl;
    std::cout << "        Set the buffer size for ZMQ messages to <buffer_size>.";
    std::cout << std::endl;
    std::cout << "        Default: <buffer_size> = " << C_ZMQ_BUFFER_SIZE << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -b";
    std::cout << std::endl;
    std::cout << "       Output in Build Number. Current Build Number " << AM_ZMQ_BUILD << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -d";
    std::cout << std::endl;
    std::cout << "       Run in diagnostics mode.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -D";
    std::cout << std::endl;
    std::cout << "       Increase debug level. Accumuative.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -h";
    std::cout << std::endl;
    std::cout << "        Display this message.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -i <ip_address>";
    std::cout << std::endl;
    std::cout << "        Read from IP address <ip_address>.";
    std::cout << std::endl;
    std::cout << "        Default: <ip_address> = \"";
    std::cout << C_DEFAULT_ZMQ_IP_ADDRESS;
    std::cout << "\".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -l <label>";
    std::cout << std::endl;
    std::cout << "        Assign the label <label> to a network connection.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -P <pause>";
    std::cout << std::endl;
    std::cout << "        Pause <pause> milliseconds before starting to execute the code.";
    std::cout << std::endl;
    std::cout << "        Default: <pause> = " << C_PAUSE << " ms.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -p <port_number>";
    std::cout << std::endl;
    std::cout << "        Read from port number <port_number>";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -v";
    std::cout << std::endl;
    std::cout << "        Output version (short form). Current verison " << AM_ZMQ_VERSION << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -V";
    std::cout << std::endl;
    std::cout << "        Output version (long form). Current verison " << AM_ZMQ_VERSION << "." << AM_ZMQ_BUILD << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -w <wait_time>";
    std::cout << std::endl;
    std::cout << "        Wait <wait_time> milliseconds after creating the connections.";
    std::cout << std::endl;
    std::cout << "        Default: <wait_time> = " << C_ZMQ_WAIT << " ms.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -X";
    std::cout << std::endl;
    std::cout << "        Output exit string (if defined)";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -x <exit_string>";
    std::cout << std::endl;
    std::cout << "        Exit the program when the string <exit_string> has been read:";
    std::cout << std::endl;
    std::cout << std::endl;
}

int main
(
    int   argc,
    char *argv[]
)
{
    bool     running          = true;
    bool     outputExitString = false;
    bool     diagnostics      = false;
    int      debugLevel       = 0;
    int      errorCode        = 0;
    int      result           = 0;
    int      portNo           = -1;
    int      waitTime         = C_ZMQ_WAIT;
    int      option           = 0;
    int      pause            = 0;
    size_t   zmqBufferSize    = C_ZMQ_BUFFER_SIZE;
    amString argument         = "";
    amString errorMessage     = "";
    amString exitString       = "";
    amString ipAddress        = C_DEFAULT_ZMQ_IP_ADDRESS;
    amString label            = "";

    opterr = 0;

    const char optionString[] = "bB:Ddhi:l:p:P:vVw:Xx:";

    while ( running && ( option = getopt( argc, argv, optionString ) ) != -1 )
    {
        switch ( option )
        {
            case 'B':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    zmqBufferSize = atoi( optarg );
                    if ( zmqBufferSize <= 0 )
                    {
                        errorCode = E_BAD_BUFFER_SIZE;
                    }
                }
                break;
            case 'b':
                std::cout << AM_ZMQ_BUILD << std::endl;
                running = false;
                break;
            case 'D':
                debugLevel++;
                break;
            case 'd':
                diagnostics = true;
                break;
            case 'v':
                std::cout << AM_ZMQ_VERSION << std::endl;
                running = false;
                break;
            case 'V':
                std::cout << C_PROGRAM_NAME << " Version " << AM_ZMQ_VERSION << "." << AM_ZMQ_BUILD << std::endl;
                std::cout << C_COPYRIGHT << std::endl;
                running = false;
                break;
            case 'h':
                help();
                running = false;
                break;
            case 'i':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    ipAddress = optarg;
                }
                break;
            case 'l':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    label = optarg;
                }
                break;
            case 'p':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    portNo = atoi( optarg );
                }
                break;
            case 'P':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    pause = atoi( optarg );
                    if ( pause < 0 )
                    {
                        errorCode = E_BAD_PAUSE_VALUE;
                    }
                }
                break;
            case 'w':
                waitTime = atoi( optarg );
                if ( waitTime < 0 )
                {
                    waitTime = 0;
                }
                break;
            case 'X':
                outputExitString = true;
                break;
            case 'x':
                exitString = optarg;
                break;
            case '?':
                errorCode = E_UNKNOWN_OPTION;
                break;
            default:
                errorCode = E_UNKNOWN_OPTION;
                break;
        }
        if ( errorCode != 0 )
        {
            running = false;
        }
    }

    if ( running && ( errorCode == 0 ) )
    {
        if ( ipAddress.empty() )
        {
            errorCode = E_MC_NO_IP_ADDRESS;
            errorMessage += "\nError: No IP Address.\n";
        }
        else if ( !isIPAddress( ipAddress ) )
        {
            errorCode = E_MC_NO_IP_ADDRESS;
            errorMessage += "\nError: IP Address \"";
            errorMessage += ipAddress;
            errorMessage += "\" is not a valid IP Address.\n";
        }
    }

    if ( running && ( errorCode == 0 ) )
    {
        if ( !isPortNumber( portNo ) )
        {
            errorCode = E_MC_NO_PORT_NUMBER;
            errorMessage += "\nError: No valid port number.\n";
        }
    }

    if ( running && ( errorCode == 0 ) )
    {
        zmqRead zmqReader( ipAddress, portNo, waitTime );

        errorCode = zmqReader.getErrorCode();
        if ( errorCode == 0 )
        {
            if ( exitString.size() > 0 )
            {
                zmqReader.setExitString( exitString );
                zmqReader.setOutputExitString( outputExitString );
            }
            zmqReader.setDebugOutput( debugLevel );
            zmqReader.setDiagnostics( diagnostics );
            zmqReader.setPause( pause );
            zmqReader.setZMQBufferSize( zmqBufferSize );

            zmqReader.zmq2stream( std::cout );

            errorCode = zmqReader.getErrorCode();
        }
        errorMessage += zmqReader.getErrorMessage();
    }

    result = outputConnectError( C_PROGRAM_NAME, errorMessage, errorCode, argument );

    exit( result );
}

