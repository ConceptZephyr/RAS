// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// mc2stdout                                                                                                                      //
//                                                                                                                                //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// Main function that reads data from multicast on the given IP address and port and outputs them to stdout.                      //
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
#include "mc_version.h"
#include "am_multicast_read.h"

const char C_PROGRAM_NAME[] = "mc2stdout";

static void help
(
   void
)
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " reads data from Multicast on the given IP address and port number and outputs them to stdout.";
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " takes the following arguments:";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -B <buffer_size>";
    std::cout << std::endl;
    std::cout << "        Set the buffer size for multcast messages to <buffer_size>.";
    std::cout << std::endl;
    std::cout << "        Default: <buffer_size> = " << C_BUFFER_SIZE << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -b";
    std::cout << std::endl;
    std::cout << "       Output in Build Number. Current Build Number " << AM_MC_BUILD << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -d";
    std::cout << std::endl;
    std::cout << "       Turn Dignostics Mode ON.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -D";
    std::cout << std::endl;
    std::cout << "       Turn Debug Mode ON.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -h";
    std::cout << std::endl;
    std::cout << "        Display this message.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -I <interface>";
    std::cout << std::endl;
    std::cout << "        Read on interface <interface>.";
    std::cout << std::endl;
    std::cout << "        Default: <interface> = \"" << C_DEFAULT_INTERFACE << "\".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -i <ip_address>";
    std::cout << std::endl;
    std::cout << "        Read from IP address <ip_address>.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -l <label>";
    std::cout << std::endl;
    std::cout << "        Assign the label <label> to a network connection.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -p <port_number>";
    std::cout << std::endl;
    std::cout << "        Read from port number <port_number>";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -t <time_out>";
    std::cout << std::endl;
    std::cout << "        Timeout connection after <time_out> seconds";
    std::cout << std::endl;
    std::cout << "        Default: <time_out> = " << C_TIME_OUT_SEC_DEFAULT << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -v";
    std::cout << std::endl;
    std::cout << "        Output version (short form). Current verison " << AM_MC_VERSION << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -V";
    std::cout << std::endl;
    std::cout << "        Output version (long form). Current verison " << AM_MC_VERSION << "." << AM_MC_BUILD << ".";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -X";
    std::cout << std::endl;
    std::cout << "        Output exit string (if defined)";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -x <exit_string>";
    std::cout << std::endl;
    std::cout << "        Set the exit string to <exit_string>";
    std::cout << std::endl;
}

int main
(
    int   argc,
    char *argv[]
)
{
    bool     running          = true;
    bool     diagnostics      = false;
    bool     outputExitString = false;
    int      debugOut         = 0;
    int      errorCode        = 0;
    int      result           = 0;
    int      portNo           = -1;
    int      option           = 0;
    int      bufferSize       = C_BUFFER_SIZE;
    double   timeOut          = C_TIME_OUT_SEC_DEFAULT;
    amString argument         = "";
    amString errorMessage     = "";
    amString exitString       = "";
    amString ipAddress        = "";
    amString interface        = C_DEFAULT_INTERFACE;
    amString label            = "";

    opterr = 0;

    const char optionString[] = "bB:dDhI:i:l:p:t:vVXx:";

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
                    bufferSize = atoi( optarg );
                    if ( bufferSize <= 0 )
                    {
                        errorCode = E_BAD_BUFFER_SIZE;
                    }
                }
                break;
            case 'b':
                std::cout << AM_MC_BUILD << std::endl;
                running = false;
                break;
            case 'd':
                diagnostics = true;
                break;
            case 'D':
                debugOut = true;
                break;
            case 'v':
                std::cout << AM_MC_VERSION << std::endl;
                running = false;
                break;
            case 'V':
                std::cout << C_PROGRAM_NAME << " Version " << AM_MC_VERSION << "." << AM_MC_BUILD << std::endl;
                std::cout << C_COPYRIGHT << std::endl;
                running = false;
                break;
            case 'h':
                help();
                running = false;
                break;
            case 'I':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    interface = optarg;
                }
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
            case 't':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    timeOut = atof( optarg );
                }
                break;
            case 'X':
                outputExitString = true;
                break;
            case 'x':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    exitString = optarg;
                }
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
        else if ( !isMulticastIPAddress( ipAddress ) )
        {
            errorCode = E_MC_NO_IP_ADDRESS;
            errorMessage += "\nError: IP Address \"";
            errorMessage += ipAddress;
            errorMessage += "\" is not a valid ";
            if ( isIPAddress( ipAddress ) )
            {
                errorMessage += "multicast ";
            }
            errorMessage += "IP Address.\n";
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
        amMulticastRead mcReader( interface, ipAddress, portNo, timeOut );
        errorCode = mcReader.getErrorCode();
        if ( errorCode == 0 )
        {
            if ( exitString.size() > 0 )
            {
                mcReader.setExitString( exitString );
                mcReader.setOutputExitString( outputExitString );
            }
            mcReader.setBufferSize( bufferSize );
            mcReader.setDebugOut( debugOut );
            mcReader.setDiagnostics( diagnostics );

            mcReader.mc2Stream( std::cout );

            errorCode = mcReader.getErrorCode();
        }
        errorMessage += mcReader.getErrorMessage();
    }

    result = outputConnectError( C_PROGRAM_NAME, errorMessage, errorCode, argument );

    exit( result );
}

