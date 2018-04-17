#!/bin/bash


INSTRUCTIONS="$0 <call-id>\n";

if [ "$1" == "" ]
then
	printf "${INSTRUCTIONS}"
	exit
fi


CALL_ID=$1
LEN=${#CALL_ID}
CONTENT_LEN=$((283+${LEN}))
printf "call_id[${CALL_ID}]len[${LEN}][$CONTENT_LEN]\n"

cat test/test_mi.txt | sed "s/CALL_ID/${CALL_ID}/ ; s/CONTENT_LEN/${CONTENT_LEN}/" > /tmp/test_mi.txt
cat /tmp/test_mi.txt | nc 127.0.0.1 8080
