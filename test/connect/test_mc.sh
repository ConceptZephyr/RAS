###############################################################
###                                                         ###
### Multicast                                               ###
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
PAUSE_TIME=200

DIFF=diff
X_DIFF="/Applications/xxdiff.app/Contents/MacOS/xxdiff"
USE_X_DIFF=0


MC_READ_1=mc2stdout
MC_READ_2=mc2file

MC_WRITE_1=file2mc
MC_WRITE_2=stdin2mc

MC_RESULT_FILE_1_1=mc_read_1_1.txt
MC_RESULT_FILE_1_2=mc_read_1_2.txt
MC_RESULT_FILE_2_1=mc_read_2_1.txt
MC_RESULT_FILE_2_2=mc_read_2_2.txt

${DELETE} ${MC_RESULT_FILE_1_1} ${MC_RESULT_FILE_1_2} ${MC_RESULT_FILE_2_1} ${MC_RESULT_FILE_2_2} \
          ${MC_RESULT_FILE_1_1}.diff ${MC_RESULT_FILE_1_2}.diff ${MC_RESULT_FILE_2_1}.diff ${MC_RESULT_FILE_2_2}.diff

MC_IP_ADDRESS_1=239.1.1.1
MC_PORT_NO_1=9991
MC_IP_ADDRESS_2=239.1.1.2
MC_PORT_NO_2=9992


if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${MC_READ_1} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} > ${MC_RESULT_FILE_1_1} &"
fi
${BIN_DIR}/${MC_READ_1} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} > ${MC_RESULT_FILE_1_1} &

if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${MC_READ_2} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} -f ${MC_RESULT_FILE_1_2} &"
fi
${BIN_DIR}/${MC_READ_2} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} -f ${MC_RESULT_FILE_1_2} &

sleep ${SLEEP_TIME}
if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${MC_WRITE_1} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} -f ${TEST_FILE} -X"
fi
${BIN_DIR}/${MC_WRITE_1} -i ${MC_IP_ADDRESS_1} -p ${MC_PORT_NO_1} -x ${EXIT_STRING} -f ${TEST_FILE} -X



${DIFF} "${MC_RESULT_FILE_1_1}" "${TEST_FILE}" > "${MC_RESULT_FILE_1_1}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${MC_RESULT_FILE_1_1} ${MC_RESULT_FILE_1_1}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${MC_RESULT_FILE_1_1}\" \"${MC_RESULT_FILE_1_1}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${MC_RESULT_FILE_1_1}"
	fi
fi
echo "Test MC Read & Write 1.1 Result: ${RESULT}"

${DIFF} "${MC_RESULT_FILE_1_2}" "${TEST_FILE}" > "${MC_RESULT_FILE_1_2}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${MC_RESULT_FILE_1_2} ${MC_RESULT_FILE_1_2}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${MC_RESULT_FILE_1_2}\" \"${MC_RESULT_FILE_1_2}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${MC_RESULT_FILE_1_2}"
	fi
fi
echo "Test MC Read & Write 1.2 Result: ${RESULT}"





if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${MC_READ_1} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} > ${MC_RESULT_FILE_2_1} &"
fi
${BIN_DIR}/${MC_READ_1} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} > ${MC_RESULT_FILE_2_1} &

if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "${BIN_DIR}/${MC_READ_2} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} -f ${MC_RESULT_FILE_2_2} &"
fi
${BIN_DIR}/${MC_READ_2} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} -f ${MC_RESULT_FILE_2_2} &

sleep ${SLEEP_TIME}
if [ "${VERBOSE}" = "1" ]; then
	echo ""
	echo "cat ${TEST_FILE} | ${BIN_DIR}/${MC_WRITE_2} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} -X"
fi
cat ${TEST_FILE} | ${BIN_DIR}/${MC_WRITE_2} -i ${MC_IP_ADDRESS_2} -p ${MC_PORT_NO_2} -x ${EXIT_STRING} -X



${DIFF} "${MC_RESULT_FILE_2_1}" "${TEST_FILE}" > "${MC_RESULT_FILE_2_1}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${MC_RESULT_FILE_2_1} ${MC_RESULT_FILE_2_1}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${MC_RESULT_FILE_2_1}\" \"${MC_RESULT_FILE_2_1}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${MC_RESULT_FILE_2_1}"
	fi
fi
echo "Test MC Read & Write 2.1 Result: ${RESULT}"

${DIFF} "${MC_RESULT_FILE_2_2}" "${TEST_FILE}" > "${MC_RESULT_FILE_2_2}.diff"
if [ "$?" = "0" ]; then
	RESULT="OK"
	${DELETE} ${MC_RESULT_FILE_2_2} ${MC_RESULT_FILE_2_2}.diff
else
	RESULT="Fail. See files \"${TEST_FILE}\" \"${MC_RESULT_FILE_2_2}\" \"${MC_RESULT_FILE_2_2}.diff\""
	if [ "${USE_X_DIFF}" = "1" ]; then
		"${X_DIFF}" "${TEST_FILE}" "${MC_RESULT_FILE_2_2}"
	fi
fi
echo "Test MC Read & Write 2.2 Result: ${RESULT}"



