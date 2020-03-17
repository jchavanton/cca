#!/bin/sh
LOCAL_PORT=${LOCAL_PORT=4291}
LOG_FILE=${LOG_FILE=""}
XMLRPC_CCA_HOST=${XMLRPC_CCA_HOST=127.0.0.1}
XMLRPC_PORT=${XMLRPC_PORT=4292}
MAX_CONN=100

ARGS="-p $LOCAL_PORT -s $XMLRPC_CCA_HOST:$XMLRPC_PORT -m ${MAX_CONN}"
if [ "$LOG_FILE" != "" ] ; then
      ARGS="$ARGS -l ${LOG_FILE}"
fi

if [ "$1" = "" ]; then
	CMD="call_control_agent ${ARGS}"
else
	CMD="$*"
fi

echo "\n Running: ${CMD}"
exec ${CMD}
