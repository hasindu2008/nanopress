#!/bin/sh
# combine all read signals into one column with header 'raw_signal'
USAGE="usage: $0 (B|S)LOW5_FILE"

if [ $# -ne 1 ]
then
	echo "$USAGE"
	exit 1
fi

FILE="$1"
BASEDIR=$(dirname "$0")

"$BASEDIR"/util/rm_hdr.sh "$FILE" | cut -f 8 | tr , '\n'
