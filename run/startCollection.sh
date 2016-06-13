#!/bin/bash
PROGRAM_NAME=$0
EXECUTE=1
REPLAY_TOOL_IS_USED=0
PARAM_FILE=/Users/sgebel/Documents/version3/run/aero_params
WAIT=200
HELP=0

PORT_NO_ADDITIONAL_1_OUT=4011
PORT_NO_ADDITIONAL_2_OUT=4012
PORT_NO_SC_OUT=4001
PORT_NO_PARAM_OUT=4002
PORT_NO_HUB_OUT=4003
PORT_NO_FC_OUT=4004
PORT_NO_RP_OUT=8890
PORT_NO_HUB_2_OUT=4005
PORT_NO_RAS_OUT=4006

IP_ADDRESS=127.0.0.1
DEBUG_ARG=""

until [ -z "$1" ]
do
        case $1 in
                -h | h)
                        echo "Usage"
                        echo "   ${PROGRAM_NAME} [-h] [-D] [-F <port_no>] [-H <port_no>] [-I <ip_address>] [-P <port_no>] [-R <port_no>] [-S <port_no>] [-T <port_no>] [-V] [-w <wait_time>]"
                        echo "   where"
                        echo "       -h            : Output this message."
                        echo "       -D            : Increase the debug level by 1 (accumulative)."
                        echo "       -F <port_no>  : Set the port number for fully-cooked output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_SC_OUT."
                        echo "       -H <port_no>  : Set the port number for zmq hub output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_HUB_OUT."
                        echo "       -I <ip_address: Set the IP Address to <ip_address>."
                        echo "                       Default: <ip_address> = $IP_ADDRESS."
                        echo "       -P <port_no>  : Set the port number for parameter output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_PARAM_OUT."
                        echo "       -R <port_no>  : Set the port number for RAS output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_HUB_OUT."
                        echo "       -S <port_no>  : Set the port number for semi-cooked output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_SC_OUT."
                        echo "       -T <port_no>  : Set the port number for Replay Tool output to <port_no>."
                        echo "                       Default: <port_no> = $PORT_NO_RP_OUT."
                        echo "       -V            : Do not execute the command, merely print them out."
                        echo "       -w <wait_time>: Set the wait time after establishing the zmq connection to <wait_time> milliseconds."
                        echo "                       Default: <wait_time> = $WAIT."
                        HELP=1
                        ;;
                -D | D)
                        DEBUG_ARG="$DEBUG_ARG -D"
                        ;;
                -F | F)
        		shift
                        PORT_NO_FC_OUT=$1
                        ;;
                -H | H)
        		shift
                        PORT_NO_HUB_OUT=$1
                        ;;
                -I | I)
        		shift
                        IP_ADDRESS=$1
                        ;;
                -P | P)
        		shift
                        PORT_NO_PARAM_OUT=$1
                        ;;
                -r | r)
        		shift
                        PORT_NO_RP_OUT=$1
                        ;;
                -R | R)
        		shift
                        PORT_NO_RAS_OUT=$1
                        ;;
                -S | S)
        		shift
                        PORT_NO_SC_OUT=$1
                        ;;
                -V | V)
                        EXECUTE=0
                        ;;
                -w | w)
        		shift
                        WAIT=$1
                        ;;
        esac
        shift
done

if [ "${HELP}" = "0" ]; then

	PORT_NO_FC_IN=${PORT_NO_HUB_OUT}
	PORT_NO_HUB_IN_1=${PORT_NO_SC_OUT}
	PORT_NO_HUB_IN_2=${PORT_NO_PARAM_OUT}
	PORT_NO_HUB_IN_3=${PORT_NO_RP_OUT}
	PORT_NO_HUB_2_IN_1=${PORT_NO_HUB_OUT}
	PORT_NO_HUB_2_IN_2=${PORT_NO_ADDITIONAL_1_OUT}
	PORT_NO_HUB_2_IN_3=${PORT_NO_ADDITIONAL_2_OUT}
	PORT_NO_RAS_IN=${PORT_NO_HUB_2_OUT}


	# ------------------------------------------------------------------------------------------------------------------------
	ZMQ_HUB=/Users/sgebel/Documents/version3/bin/zmqhub
	HUB_ARGS="-l \"BRIDGE\" -i ${IP_ADDRESS} -p ${PORT_NO_HUB_IN_1}    -l \"PARAMS\" -i ${IP_ADDRESS} -p ${PORT_NO_HUB_IN_2}     -l \"REPLAY\" -i ${IP_ADDRESS} -p ${PORT_NO_HUB_IN_3}    -l \"SEMI\" -o ${IP_ADDRESS} -p ${PORT_NO_HUB_OUT}"
	echo ""
	echo "${ZMQ_HUB} ${HUB_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${ZMQ_HUB} ${HUB_ARGS} &
	fi


	# ------------------------------------------------------------------------------------------------------------------------
	BRIDGE2TXT=/Users/sgebel/Documents/version3/bin/bridge2txt
	ANT_2_SEMICOOKED="${BRIDGE2TXT}"
	ANT_2_SEMICOOKED_ARGS="-l \"BRIDGE\" -Z ${IP_ADDRESS} -p ${PORT_NO_SC_OUT} -S"
	echo ""
	echo "${ANT_2_SEMICOOKED} ${ANT_2_SEMICOOKED_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${ANT_2_SEMICOOKED} ${ANT_2_SEMICOOKED_ARGS} &
	fi


	# ------------------------------------------------------------------------------------------------------------------------
	ZMQ_HUB_2=/Users/sgebel/Documents/version3/bin/zmqhub
	HUB_2_ARGS="-l \"HUB_1\" -i ${IP_ADDRESS} -p ${PORT_NO_HUB_OUT}    -l \"ADDITIONAL_1\" -i ${IP_ADDRESS} -p ${PORT_NO_ADDITIONAL_1_OUT}    -l \"ADDITIONAL_2\" -i ${IP_ADDRESS} -p ${PORT_NO_ADDITIONAL_2_OUT}     -l \"HUB_2\" -o ${IP_ADDRESS} -p ${PORT_NO_HUB_2_OUT}"
	echo ""
	echo "${ZMQ_HUB_2} ${HUB_2_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${ZMQ_HUB_2} ${HUB_2_ARGS} &
	fi


	# ------------------------------------------------------------------------------------------------------------------------
	SEMICOOKED_2_FULLYCOOKED="${BRIDGE2TXT} -s"
	SEMICOOKED_2_FULLYCOOKED_ARGS="-l \"SEMI\" -z ${IP_ADDRESS} -p ${PORT_NO_FC_IN}    -l \"FULLY\" -Z ${IP_ADDRESS} -p ${PORT_NO_FC_OUT}"
	echo ""
	echo "${SEMICOOKED_2_FULLYCOOKED} ${SEMICOOKED_2_FULLYCOOKED_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${SEMICOOKED_2_FULLYCOOKED} ${SEMICOOKED_2_FULLYCOOKED_ARGS} &
	fi


	# ------------------------------------------------------------------------------------------------------------------------
	RAS=/Users/sgebel/Documents/version3/bin/ras
	RAS_ARGS="-z ${IP_ADDRESS} -p ${PORT_NO_RAS_IN} -l \"FULLY\" -Z ${IP_ADDRESS} -P ${PORT_NO_RAS_OUT} -l \"RAS\"${DEBUG_ARG}"
	echo ""
	echo "${RAS} ${RAS_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${RAS} ${RAS_ARGS} &
	fi


	# ------------------------------------------------------------------------------------------------------------------------
	FILE2ZMQ=/Users/sgebel/Documents/version3/bin/file2zmq
	FILE2ZMQ_ARGS="-i ${IP_ADDRESS} -p ${PORT_NO_PARAM_OUT} -l \"PARAMS\" -f ${PARAM_FILE} -P 100 -w ${WAIT}"
	echo ""
	echo "${FILE2ZMQ} ${FILE2ZMQ_ARGS}"
	if [ "${EXECUTE}" = "1" ]; then
		${FILE2ZMQ} ${FILE2ZMQ_ARGS} &
	fi
else
	echo ""
	echo ""
	echo "    +------------+                +-------------+             +-------+            +-------+            +-----+            +-------+"
	echo "    |            |    Input       |             |    ZMQ      |       |            |       |            |     |            |       |"
	echo "    |  LOG_FILE  | -------------> | REPLAY_TOOL | ----------> |       |            |       |            |     |            |       |"
	echo "    |            |    Stream      |             |    8890     |       |            |       |            |     |            |       |"
	echo "    +------------+                +-------------+             |       |            | SEMI  |            |     |            |       |"
	echo "                                                              |       |            |       |            |     |            |       |"
	echo "                                                              |  ZMQ  |            |       |            | ZMQ |            |       |"
	echo "    +------------+                +-------------+             |       |            |       |            |     |            |       |"
	echo "    |            |    Multicast   |             |    ZMQ      |       |    ZMQ     |       |    ZMQ     |     |    ZMQ     |       |    ZMQ"
	echo "    |    ANT+    | -------------> |   ANT_2_SC  | ----------> |       | ---------> |   TO  | -------->  |     | ---------> |  RAS  | --------->"
	echo "    |            |                |             |    4001     |       |    4003    |       |    4004    | HUB |    4005    |       |    4006"
	echo "    +------------+                +-------------+             |       |            |       |            |     |            |       |"
	echo "                                                              |       |            |       |            |     |            |       |"
	echo "                                                              |  HUB  |            | FULLY |            |  2  |            |       |"
	echo "    +------------+                +-------------+             |       |            |       |            |     |            |       |"
	echo "    |            |    Input       |             |    ZMQ      |       |            |       |            |     |            |       |"
	echo "    | PARAM_FILE | -------------> | FILE_2_ZMQ  | ----------> |       |            |       |            |     |            |       |"
	echo "    |            |    Stream      |             |    4002     |       |            |       |            |     |            |       |"
	echo "    +------------+                +-------------+             +-------+            +-------+            |     |            |       |"
	echo "                                                                                                        |     |            |       |"
	echo "    +------------+                                                                                      |     |            |       |"
	echo "    |            |                                   ZMQ                                                |     |            |       |"
	echo "    | ADDITIONAL | -----------------------------------------------------------------------------------> |     |            |       |"
	echo "    |     1      |                                   4011                                               |     |            |       |"
	echo "    |            |                                                                                      |     |            |       |"
	echo "    +------------+                                                                                      |     |            |       |"
	echo "                                                                                                        |     |            |       |"
	echo "    +------------+                                                                                      |     |            |       |"
	echo "    |            |                                   ZMQ                                                |     |            |       |"
	echo "    | ADDITIONAL | -----------------------------------------------------------------------------------> |     |            |       |"
	echo "    |     2      |                                   4012                                               |     |            |       |"
	echo "    |            |                                                                                      |     |            |       |"
	echo "    +------------+                                                                                      +-----+            +-------+
	echo "                                                                                                        
	echo ""
fi
echo ""
