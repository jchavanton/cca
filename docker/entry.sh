#!/bin/sh
LOCAL_PORT=${LOCAL_PORT=4291}
LOG_FILE=${LOG_FILE=""}
KAM_SOCK=${KAM_SOCK=127.0.0.1:4292}

ARGS="-p $LOCAL_PORT -s $KAM_SOCK"
if [ "$LOG_FILE" != "" ] ; then
	ARGS="$ARGS -l ${LOG_FILE}"
fi
CMD="call_control_agent"
echo "\nRunning: $CMD $ARGS"
exec $CMD $ARGS
