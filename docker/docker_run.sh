#!/bin/sh

# docker build . -t call_control_agent
IMAGE="call_control_agent"
docker stop call_control_agent
docker rm call_control_agent
docker run -d \
        --name=call_control_agent \
        --net=host \
        --cap-add=IPC_LOCK \
        --cap-add=SYS_NICE \
        --restart=always \
        --log-opt max-size=10m \
        --log-opt max-file=5 \
        $IMAGE \
        tail -f /dev/null

