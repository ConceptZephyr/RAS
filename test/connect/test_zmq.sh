###############################################################
###                                                         ###
### ZMQ                                                     ###
###                                                         ###
###############################################################
#/bin/bash

VERBOSE=0
if [ "$1" = "-v" ]; then
	VERBOSE=1
fi

BIN_DIR=../../bin
DELETE="/bin/rm -f"

EXIT_STRING=__EXIT__
TEST_FILE=testfile.txt
SLEEP_TIME=2
PAUSE_TIME=10
WAIT_TIME=200

DIFF=diff
X_DIFF="/Applications/xxdiff.app/Contents/MacOS/xxdiff"
USE_X_DIFF=0


ZMQ_READ_1=zmq2stdout
ZMQ_READ_2=zmq2file

ZMQ_WRITE_1=file2zmq
ZMQ_WRITE_2=stdin2zmq

ZMQ_RESULT_FILE_1_1=zmq_read_1_1.txt
ZMQ_RESULT_FILE_1_2=zmq_read_1_2.txt
ZMQ_RESULT_FILE_2_1=zmq_read_2_1.txt
ZMQ_RESULT_FILE_2_2=zmq_read_2_2.txt

ZMQ_IP_ADDRESS_1=127.0.0.1
ZMQ_PORT_NO_1=9981
ZMQ_IP_ADDRESS_2=127.0.0.1
ZMQ_PORT_NO_2=9982


${DELETE} ${ZMQ_RESULT_FILE_1_1} ${ZMQ_RESULT_FILE_1_2} ${ZMQ_RESULT_FILE_2_1} ${ZMQ_RESULT_FILE_2_2} \
          ${ZMQ_RESULT_FILE_1_1}.diff ${ZMQ_RESULT_FILE_1_2}.diff ${ZMQ_RESULT_FILE_2_1} ${ZMQ_RESULT_FILE_2_2}.diff


if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${ZMQ_READ_1} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -x ${EXIT_STRING} > ${ZMQ_RESULT_FILE_1_1} &"
fi
${BIN_DIR}/${ZMQ_READ_1} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -x ${EXIT_STRING} > ${ZMQ_RESULT_FILE_1_1} &

if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${ZMQ_READ_2} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -x ${EXIT_STRING} -f ${ZMQ_RESULT_FILE_1_2} &"
fi
${BIN_DIR}/${ZMQ_READ_2} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -x ${EXIT_STRING} -f ${ZMQ_RESULT_FILE_1_2} &

sleep ${SLEEP_TIME}
if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${ZMQ_WRITE_1} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -P ${PAUSE_TIME} -w ${WAIT_TIME} -x ${EXIT_STRING} -X -f ${TEST_FILE}"
fi
${BIN_DIR}/${ZMQ_WRITE_1} -i ${ZMQ_IP_ADDRESS_1} -p ${ZMQ_PORT_NO_1} -P ${PAUSE_TIME} -w ${WAIT_TIME} -x ${EXIT_STRING} -X -f ${TEST_FILE}



${DIFF} "${ZMQ_RESULT_FILE_1_1}" "${TEST_FILE}" > "${ZMQ_RESULT_FILE_1_1}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${ZMQ_RESULT_FILE_1_1} ${ZMQ_RESULT_FILE_1_1}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${ZMQ_RESULT_FILE_1_1}\" \"${ZMQ_RESULT_FILE_1_1}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${ZMQ_RESULT_FILE_1_1}"
	fi
fi
echo "Test ZMQ Read & Write 1.1 Result: ${RESULT}"

${DIFF} "${ZMQ_RESULT_FILE_1_2}" "${TEST_FILE}" > "${ZMQ_RESULT_FILE_1_2}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${ZMQ_RESULT_FILE_1_2} ${ZMQ_RESULT_FILE_1_2}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${ZMQ_RESULT_FILE_1_2}\" \"${ZMQ_RESULT_FILE_1_2}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${ZMQ_RESULT_FILE_1_2}"
	fi
fi
echo "Test ZMQ Read & Write 1.2 Result: ${RESULT}"





if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${ZMQ_READ_1} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -x ${EXIT_STRING} > ${ZMQ_RESULT_FILE_2_1} &"
fi
${BIN_DIR}/${ZMQ_READ_1} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -x ${EXIT_STRING} > ${ZMQ_RESULT_FILE_2_1} &

if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${ZMQ_READ_2} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -x ${EXIT_STRING} -f ${ZMQ_RESULT_FILE_2_2} &"
fi
${BIN_DIR}/${ZMQ_READ_2} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -x ${EXIT_STRING} -f ${ZMQ_RESULT_FILE_2_2} &

sleep ${SLEEP_TIME}
if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "cat ${TEST_FILE} | ${BIN_DIR}/${ZMQ_WRITE_2} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -P ${PAUSE_TIME} -w ${WAIT_TIME} -x ${EXIT_STRING} -X"
fi
cat ${TEST_FILE} | ${BIN_DIR}/${ZMQ_WRITE_2} -i ${ZMQ_IP_ADDRESS_2} -p ${ZMQ_PORT_NO_2} -P ${PAUSE_TIME} -w ${WAIT_TIME} -x ${EXIT_STRING} -X



${DIFF} "${ZMQ_RESULT_FILE_2_1}" "${TEST_FILE}" > "${ZMQ_RESULT_FILE_2_1}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${ZMQ_RESULT_FILE_2_1} ${ZMQ_RESULT_FILE_2_1}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${ZMQ_RESULT_FILE_2_1}\" \"${ZMQ_RESULT_FILE_2_1}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${ZMQ_RESULT_FILE_2_1}"
	fi
fi
echo "Test ZMQ Read & Write 2.1 Result: ${RESULT}"

${DIFF} "${ZMQ_RESULT_FILE_2_2}" "${TEST_FILE}" > "${ZMQ_RESULT_FILE_2_2}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${ZMQ_RESULT_FILE_2_2} ${ZMQ_RESULT_FILE_2_2}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${ZMQ_RESULT_FILE_2_2}\" \"${ZMQ_RESULT_FILE_2_2}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${ZMQ_RESULT_FILE_2_2}"
	fi
fi
echo "Test ZMQ Read & Write 2.2 Result: ${RESULT}"

