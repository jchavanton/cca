#!/bin/sh

# docker build . -t ccc
IMAGE="ccc"
docker stop ccc
docker rm ccc
docker run -d \
        --name=ccc \
        --net=host \
        --cap-add=IPC_LOCK \
        --cap-add=SYS_NICE \
        --restart=always \
        --log-opt max-size=10m \
        --log-opt max-file=5 \
        $IMAGE \
        tail -f /dev/null

