#!/bin/bash

MOTDFILE="$HOME/lyra/var/text/motd.txt"

BRANCH=$1
DATE=`date '+%Y-%m-%d %H:%M:%S'`
HOSTNAME=`hostname`

if [ "$BRANCH" == "" ] 
then 
  BRANCH="UNKNOWN" 
fi

echo "Server: $HOSTNAME" > "$MOTDFILE"
echo "Branch: $BRANCH" >> "$MOTDFILE"
echo "Date: $DATE" >> "$MOTDFILE"
