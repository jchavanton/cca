#!/bin/bash

INSTRUCTIONS="$0 <call-id>\n";

if [ "$1" == "" ]
then
	printf "${INSTRUCTIONS}"
	exit
fi

CALL_ID=$1
LEN=${#CALL_ID}
TO_TAG=$2
LEN2=${#TO_TAG}
CONTENT_LEN=$((273+${LEN}+${LEN2}))
printf "call_id[${CALL_ID}][${LEN}]to_tag[${TO_TAG}][$LEN2][$CONTENT_LEN]\n"

cat test/test_mi.txt | sed "s/CALL_ID/${CALL_ID}/ ; s/TO_TAG/${TO_TAG}/ ; s/CONTENT_LEN/${CONTENT_LEN}/" > /tmp/test_mi.txt
cat /tmp/test_mi.txt | nc 127.0.0.1 8080
