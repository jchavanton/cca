#!/bin/sh
LOCAL_PORT=${LOCAL_PORT=4291}
LOG_FILE=${LOG_FILE=""}
XMLRPC_CCA_HOST=${XMLRPC_CCA_HOST=127.0.0.1}
XMLRPC_PORT=${XMLRPC_PORT=4292}

ARGS="-p $LOCAL_PORT -s $XMLRPC_CCA_HOST:$XMLRPC_PORT"
if [ "$LOG_FILE" != "" ] ; then
      ARGS="$ARGS -l ${LOG_FILE}"
fi

CMD="call_control_agent"
echo "\n Running: $CMD $ARGS"
exec $CMD $ARGS
