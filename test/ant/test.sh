#!/bin/bash

BIN_DIR=../../bin
DELETE=/bin/rm

DIFF=diff
X_DIFF="/Applications/xxdiff.app/Contents/MacOS/xxdiff"
USE_X_DIFF=0

ANT_FILE=testfile.ant
DATA_DIRECTORY=test
SEMICOOKED_OUT=0
SEMICOOKED_IN=0
VERBOSE=0
JSON_ARG=
JSON=
DEVICE=ALL
RUN=1
TEST_ALL=0
MAX_COUNTER=1
DEVICE_FILE_ARG=
DEVICE_FILE="${DATA_DIRECTORY}/deviceIDs"
PARAMETERS=""
HAS_PARAMETERS=

${DELETE} "${DATA_DIRECTORY}/*.txt.tmp" "${DATA_DIRECTORY}/*.txt.diff" 2>/dev/null

export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:../../lib
echo "DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}"

until [ -z "$1" ]
do   
	case $1 in  
		-A | A)
			TEST_ALL=1
			;;
		-d | d)
			DEVICE_FILE_ARG=" -d ${DEVICE_FILE}"
			;;
		-D | D)
			shift
			DEVICE=$1
			;;
		-h | h)
			echo "Usage"
			echo "    $0 [-D <device_type>] [-h] [-S]"
			echo "where"
			echo "    -A              : Test all cominations."
			echo "    -D <device_type>: Run test for device <device_type>."
			echo "                      Supported device types:"
			echo "                           ALL       : All Sensors."
			echo "                           AERO      : Aero Sensor."
			echo "                           AUDIO     : Audio Sensor (1)."
			echo "                           BLOOD     : Blood Pressure Sensor (1)."
			echo "                           CAD       : Cadence Sensor."
			echo "                           ENV       : Environment Sensor (1)."
			echo "                           HRM       : Heart Rate Sensor."
			echo "                           MULTI     : Multi-Sport Speed and Distance Sensor (1)."
			echo "                           POWER     : Power Sensor."
			echo "                           SPB7      : Speed Only Sensor."
			echo "                           SPCAD     : Speed and Cadence Sensor."
			echo "                           STRIDE    : Stride Based Speed and Distance Sensor (1)."
			echo "                           WEIGHT    : Weight Scale Sensor (1)."
			echo "                      Default: <device_type> = ${DEVICE}"
			echo "    -h              : Display this message."
			echo "    -S              : Run test for semi-cooked output."
			echo "                      Default: fully-cooked output."
			echo "(1) Tests are not yet available due to lack of ANT+ input data."
        		RUN=0
			;;
		-J | J)
			JSON="JSON"
			JSON_ARG=" -J"
			;;
		-s | s)
			SEMI_ARG=" -s"
			;;
		-S | S)
			SEMI_ARG=" -S"
			;;
		-x | X)
			USE_X_DIFF=1
			;;
		-v | v)
			VERBOSE=1
			;;
	esac
	shift
done

if [ "${RUN}" = "1" ]; then
	if [ "${DEVICE}" = "ALL" ]; then
		EXECUTABLE=bridge2txt
		OUTPUT_FILE_ROOT=allTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "AERO" ]; then
		EXECUTABLE=aero_ant2txt
		OUTPUT_FILE_ROOT=aeroTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "AUDIO" ]; then
		EXECUTABLE=audio_ant2txt
		OUTPUT_FILE_ROOT=audioTest
	elif [ "${DEVICE}" = "BLOOD" ]; then
		EXECUTABLE=blood_pressure_ant2txt
		OUTPUT_FILE_ROOT=bloodTest
	elif [ "${DEVICE}" = "CAD" ]; then
		EXECUTABLE=cadence_only_ant2txt
		OUTPUT_FILE_ROOT=cadTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "ENV" ]; then
		EXECUTABLE=environment_ant2txt
		OUTPUT_FILE_ROOT=envTest
	elif [ "${DEVICE}" = "HRM" ]; then
		EXECUTABLE=hrm_ant2txt
		OUTPUT_FILE_ROOT=hrmTest
	elif [ "${DEVICE}" = "MULTI" ]; then
		EXECUTABLE=multi_sport_speed_dist_ant2txt
		OUTPUT_FILE_ROOT=multiTest
	elif [ "${DEVICE}" = "POWER" ]; then
		EXECUTABLE=power_ant2txt
		OUTPUT_FILE_ROOT=powerTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "SPB7" ]; then
		EXECUTABLE=speed_only_ant2txt
		OUTPUT_FILE_ROOT=speedOnlyTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "SPCAD" ]; then
		EXECUTABLE=spcad_ant2txt
		OUTPUT_FILE_ROOT=spcadTest
		HAS_PARAMETERS=1
	elif [ "${DEVICE}" = "STRIDE" ]; then
		EXECUTABLE=stride_speed_dist_ant2txt
		OUTPUT_FILE_ROOT=strideTest
	elif [ "${DEVICE}" = "WEIGHT" ]; then
		EXECUTABLE=weight_ant2txt
		OUTPUT_FILE_ROOT=weightTest
	else
		echo "ERROR: Unknown device type \"${DEVICE}\""
        	RUN=0
	fi
fi

if [ "${TEST_ALL}" = "1" ]; then
	MAX_COUNTER=10
	SEMI_ARG=
	DEVICE_FILE_ARG=
	JSON_ARG=
fi

if [ "${RUN}" = "1" ]; then

	while [ "$COUNTER" != "$MAX_COUNTER" ]; do

		if [ "${DEVICE_FILE_ARG}" = "" ]; then
			PARAMETERS=""
		else
			PARAMETERS="Parameter"
		fi

		if [ "${JSON_ARG}" = "" ]; then
			JSON=""
		else
			JSON="JSON"
		fi

		if [ "${SEMI_ARG}" = " -S" ]; then
			INPUT_FILE="${DATA_DIRECTORY}/${ANT_FILE}"
			OUTPUT_FILE="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}Semi${JSON}.txt"
			OUTPUT_FILE_TMP="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}Semi${JSON}.tmp"
			OUTPUT_FILE_DIFF="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}Semi${JSON}.diff"
			TEST_NAME="Semi-Out"
		elif [ "${SEMI_ARG}" = " -s" ]; then
			INPUT_FILE="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}Semi.txt"
			OUTPUT_FILE="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.txt"
			OUTPUT_FILE_TMP="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.tmp"
			OUTPUT_FILE_DIFF="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.diff"
			TEST_NAME="Semi-In"
		else
			INPUT_FILE="${DATA_DIRECTORY}/${ANT_FILE}"
			OUTPUT_FILE="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.txt"
			OUTPUT_FILE_TMP="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.tmp"
			OUTPUT_FILE_DIFF="${DATA_DIRECTORY}/${OUTPUT_FILE_ROOT}${PARAMETERS}Fully${JSON}.diff"
			TEST_NAME="Fully"
		fi

		if [ "${PARAMETERS}" != "" ]; then
			TEST_NAME="${TEST_NAME}-${PARAMETERS}"
		fi
		if [ "${JSON}" != "" ]; then
			TEST_NAME="${TEST_NAME}-${JSON}"
		fi

		if [ "${VERBOSE}" = "1" ] ; then
			echo "${BIN_DIR}/${EXECUTABLE}${SEMI_ARG}${JSON_ARG}${DEVICE_FILE_ARG} -x -f \"${INPUT_FILE}\" > \"${OUTPUT_FILE_TMP}\""
			echo "${DIFF} \"${OUTPUT_FILE}\" \"${OUTPUT_FILE_TMP}\""
		else
			${BIN_DIR}/${EXECUTABLE}${SEMI_ARG}${JSON_ARG}${DEVICE_FILE_ARG} -x -f "${INPUT_FILE}" > "${OUTPUT_FILE_TMP}"
			${DIFF} "${OUTPUT_FILE}" "${OUTPUT_FILE_TMP}" > "${OUTPUT_FILE_DIFF}"

			if [ "$?" = "0" ]; then
				RESULT="OK"
				${DELETE} "${OUTPUT_FILE_TMP}"
				${DELETE} "${OUTPUT_FILE_DIFF}"
			else
				RESULT="Fail. See files \"${OUTPUT_FILE}\" \"${OUTPUT_FILE_TMP}\" \"${OUTPUT_FILE_DIFF}\""
				if [ "${USE_X_DIFF}" = "1" ]; then
					"${X_DIFF}" "${OUTPUT_FILE}" "${OUTPUT_FILE_TMP}"
				fi
			fi

			echo "Test ${TEST_NAME} ${DEVICE} Result: ${RESULT}"
		fi

		let COUNTER=COUNTER+1
		if [ "$COUNTER" = 1 ]; then
			SEMI_ARG=" -s"
			DEVICE_FILE_ARG=
			JSON_ARG=
		elif [ "$COUNTER" = 2 ]; then
			SEMI_ARG=" -S"
			DEVICE_FILE_ARG=
			JSON_ARG=
		elif [ "$COUNTER" = 3 ]; then
			SEMI_ARG=
			DEVICE_FILE_ARG=
			JSON_ARG=" -J"
		elif [ "$COUNTER" = 4 ]; then
			SEMI_ARG=" -s"
			DEVICE_FILE_ARG=
			JSON_ARG=" -J"
		elif [ "$COUNTER" = 5 ]; then
			SEMI_ARG=" -S"
			DEVICE_FILE_ARG=
			JSON_ARG=" -J"
		elif [ "$COUNTER" = 6 ]; then
			if [ "${HAS_PARAMETERS}" = "1" ]; then
				SEMI_ARG=
				DEVICE_FILE_ARG=" -d ${DEVICE_FILE}"
				JSON_ARG=
			else
				let COUNTER=MAX_COUNTER+1
			fi
		elif [ "$COUNTER" = 7 ]; then
			SEMI_ARG=" -s"
			DEVICE_FILE_ARG=" -d ${DEVICE_FILE}"
			JSON_ARG=
		elif [ "$COUNTER" = 8 ]; then
			SEMI_ARG=
			DEVICE_FILE_ARG=" -d ${DEVICE_FILE}"
			JSON_ARG=" -J"
		elif [ "$COUNTER" = 9 ]; then
			SEMI_ARG=" -s"
			DEVICE_FILE_ARG=" -d ${DEVICE_FILE}"
			JSON_ARG=" -J"
		fi
	done
fi

