#!/bin/bash
BRIDGE2TXT=bridge2txt
ZMQ_HUB=zmqhub
RAS=ras

killall ${BRIDGE2TXT}
killall ${ZMQ_HUB}
killall ${RAS}
killall -KILL ${RAS}
