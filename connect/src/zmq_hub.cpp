#include <iostream>
#include <unistd.h>

#include <vector>

#include <am_constants.h>
#include <am_string.h>

#include "connect_util.h"
#include "zmq_version.h"
#include "zmq_hub_class.h"

const char C_PROGRAM_NAME[] = "zmq_hub";

static void help
(
   void
)
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << C_PROGRAM_NAME << " takes N > 0 pairs of input IP addresses and port numbers and";
    std::cout << std::endl;
    std::cout << "    M > 0 pairs of output IP addresses and port numbers as arguments.";
    std::cout << std::endl;
    std::cout << "    It reads data form the N input sockets and sends these data out on the M output sockets.";
    std::cout << std::endl;
    std::cout << "    The IP addresses and port numbers are paired in the order in which they are specified,";
    std::cout << std::endl;
    std::cout << "    i.e. the first IP address specified is paired with the first port number specified,";
    std::cout << std::endl;
    std::cout << "    the second IP address specified is paired with the second port number specified, etc.";
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
    std::cout << "    -h";
    std::cout << std::endl;
    std::cout << "        Display this message.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -i <in_ip_address>";
    std::cout << std::endl;
    std::cout << "        Read input from the IP address <in_ip_address>.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -l <label>";
    std::cout << std::endl;
    std::cout << "        Assign the label <label> to a network connection.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -o <out_ip_address>";
    std::cout << std::endl;
    std::cout << "        Write output to the IP address <out_ip_address>.";
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
    std::cout << "        Read from or write to port number <port_number>";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -t <time_stamp_mode>";
    std::cout << std::endl;
    std::cout << "        <time_stamp_mode> = 'A': Append new timestamp to the ANT+ and timing messages.";
    std::cout << std::endl;
    std::cout << "        <time_stamp_mode> = 'K': Keep timestamps of the ANT+ and timing messages unchanged.";
    std::cout << std::endl;
    std::cout << "        <time_stamp_mode> = 'O': Overwite the time stamps of the ANT+ and timing messages.";
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "    -T <time_precision>";
    std::cout << std::endl;
    std::cout << "        Output the timestamps with <time_precision> places after the decimal dot.";
    std::cout << std::endl;
    std::cout << "        Note: This option only applies combined with options '-t A' or '-t O'.";
    std::cout << std::endl;
    std::cout << "        Default: <time_precision> = " << C_TIME_PRECISION << ".";
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
    amTimeStampMode timeStampMode = KEEP_AS_IS;
    bool            running       = true;
    bool            diagnostics   = false;
    int             errorCode     = 0;
    int             result        = 0;
    int             portNo        = -1;
    int             waitTime      = C_ZMQ_WAIT;
    int             option        = 0;
    int             pause         = 0;
    size_t          timePrecision = C_TIME_PRECISION;
    size_t          zmqBufferSize = C_ZMQ_BUFFER_SIZE;
    amString        argument      = "";
    amString        errorMessage  = "";
    amString        exitString    = "";

    std::vector<amString> inIPAddress;
    std::vector<amString> label;
    std::vector<amString> inLabel;
    std::vector<amString> outIPAddress;
    std::vector<amString> outLabel;
    std::vector<int>      allPortNumbers;
    std::vector<int>      inPortNumbers;
    std::vector<int>      outPortNumbers;
    std::vector<bool>     isInIPAddress;

    opterr = 0;

    const char optionString[] = "bB:dhi:l:o:p:P:t:T:vVw:x:";

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
                    isInIPAddress.push_back( true );
                    inIPAddress.push_back( optarg );
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
                    label.push_back( optarg );
                }
                break;
            case 'o':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    outIPAddress.push_back( optarg );
                    isInIPAddress.push_back( false );
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
                    allPortNumbers.push_back( portNo );
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
            case 't':
                switch ( *optarg )
                {
                    case 'A': timeStampMode = APPEND;
                              break;
                    case 'K': timeStampMode = KEEP_AS_IS;
                              break;
                    case 'O': timeStampMode = OVERWRITE;
                              break;
                    default : errorCode = E_NO_TIMESTAMP_VALUE;
                              break;
                }
                break;
            case 'T':
                if ( ( optarg == NULL ) || ( *optarg == 0 ) || ( *optarg == '-' ) )
                {
                    errorCode = E_MISSING_ARG;;
                    argument  = argv[ optind - 1 ];
                }
                else
                {
                    timePrecision = atoi( optarg );
                    if ( timePrecision > C_MAX_TIME_PRECISION )
                    {
                        errorCode = E_BAD_PRECISION_VALUE;
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


    if ( running )
    {
        if ( isInIPAddress.size() == 0 )
        {
            errorCode = E_NO_CONNECTION_IN_OUT;
            running   = false;
        }
        else if ( inIPAddress.size() == 0 )
        {
            errorCode = E_NO_CONNECTION_IN;
            running   = false;
        }
        else if ( outIPAddress.size() == 0 )
        {
            errorCode = E_NO_CONNECTION_OUT;
            running   = false;
        }
        else if ( allPortNumbers.size() > isInIPAddress.size() )
        {
            errorCode = E_TOO_MANY_PORT_NOS;
            running   = false;
        }
        else if ( allPortNumbers.size() < isInIPAddress.size() )
        {
            errorCode = E_TOO_FEW_PORT_NOS;
            running   = false;
        }
    }

    if ( running )
    {
        int counter = 0;
        for ( ; counter < ( int ) allPortNumbers.size(); ++counter )
        {
            if ( isInIPAddress[ counter ] )
            {
                inPortNumbers.push_back( allPortNumbers[ counter ] );
                if ( counter < ( int ) label.size() )
                {
                    inLabel.push_back( label[ counter ] );
                }
                else
                {
                    inLabel.push_back( "" );
                }
            }
            else
            {
                outPortNumbers.push_back( allPortNumbers[ counter ] );
                if ( counter < ( int ) label.size() )
                {
                    outLabel.push_back( label[ counter ] );
                }
                else
                {
                    outLabel.push_back( "" );
                }
            }
        }

        allPortNumbers.clear();
        label.clear();
        isInIPAddress.clear();

        inLabel.clear();
        outLabel.clear();

        zmqHub hub( inIPAddress, inPortNumbers, outIPAddress, outPortNumbers, waitTime );

        if ( hub.getErrorCode() == 0 )
        {
            hub.setTimeStampMode( timeStampMode );
            hub.setExitString( exitString );
            hub.setDiagnostics( diagnostics );
            hub.setPause( pause );
            hub.setTimePrecision( timePrecision );
            hub.setZMQBufferSize( zmqBufferSize );
            hub.run();

            errorMessage += hub.getErrorMessage();
        }
    }

    if ( errorCode )
    {
        result = errorCode;
        std::cout << C_PROGRAM_NAME << ":" << std::endl;
        if ( errorMessage.size() )
        {
            std::cout << "    " << errorMessage << std::endl;
        }
        std::cout << "    errorCode = " << errorCode << std::endl;
    }

    exit( result );
}

