#!/bin/sh

cat test/test_mi.txt | nc 127.0.0.1 8080
cat test/test_message.txt | nc 127.0.0.1 8080
