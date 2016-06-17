// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// mc2file                                                                                                                        //
//                                                                                                                                //
// ------------------------------------------------------------------------------------------------------------------------------ //
//                                                                                                                                //
// Main function that reads data from multicast on the given IP address and port and outputs them to file.                        //
//                                                                                                                                //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------------------ //

#include <iostream>
#include <fstream>
#include <unistd.h>

#include <vector>

#include <am_constants.h>
#include <am_string.h>

#include "connect_util.h"
#include "mc_version.h"
#include "am_multicast_read.h"

const char C_PROGRAM_NAME[] = "mc2file";

static void help
(
   void
)
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " reads data from Multicast on the given IP address and port number and outputs them to file.";
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
    std::cout << "    -f <file_name>";
    std::cout << std::endl;
    std::cout << "        Write output to file <file_name>.";
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
    bool            running       = true;
    bool            diagnostics   = false;
    int             debugOut      = 0;
    int             errorCode     = 0;
    int             result        = 0;
    int             portNo        = -1;
    int             option        = 0;
    int             bufferSize    = C_BUFFER_SIZE;
    double          timeOut       = C_TIME_OUT_SEC_DEFAULT;
    amString        argument      = "";
    amString        errorMessage  = "";
    amString        exitString    = "";
    amString        ipAddress     = "";
    amString        interface     = C_DEFAULT_INTERFACE;
    amString        fileName      = "";
    amString        label         = "";

    opterr = 0;

    const char optionString[] = "bB:dDhI:i:l:p:t:vVx:";

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
            case 'f':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    fileName = optarg;
                }
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

    if ( running && ( errorCode != 0 ) )
    {
        if ( fileName.empty() )
        {
            running       = false;
            errorCode     = E_NO_FILE_NAME;
            errorMessage += "\nError: No file name.";
        }
    }

    if ( running && ( errorCode != 0 ) )
    {
        std::ofstream outStream( fileName.c_str() );
        if ( !outStream.good() )
        {
            errorCode     = E_WRITE_FILE_NOT_OPEN;
            errorMessage += "\nError: Could not open file \"";
            errorMessage += fileName;
            errorMessage += "\" for writing.";
        }

        if ( errorCode == 0 )
        {
            amMulticastRead mcReader( interface, ipAddress, portNo, timeOut );
            errorCode = mcReader.getErrorCode();
            if ( errorCode == 0 )
            {
                mcReader.setBufferSize( bufferSize );
                mcReader.setDebugOut( debugOut );
                mcReader.setDiagnostics( diagnostics );
                mcReader.mc2Stream( outStream );
                mcReader.setExitString( exitString );
                errorCode = mcReader.getErrorCode();
            }
            errorMessage += mcReader.getErrorMessage();
        }
    }

    result = outputConnectError( C_PROGRAM_NAME, errorMessage, errorCode, argument );

    exit( result );
}

