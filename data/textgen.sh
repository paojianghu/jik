#!/usr/bin/env bash

WGET=wget
if ! type "$WGET" &> /dev/null; then
  echo Command $WGET not found
  exit 1
fi
TEXTGEN_DIR=$(dirname "$0")/textgen
rm -rf $TEXTGEN_DIR
mkdir -p $TEXTGEN_DIR
$WGET -P $TEXTGEN_DIR http://textfiles.com/stories/adv_alad.txt
