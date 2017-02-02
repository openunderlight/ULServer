#!/bin/bash

BRANCH=$1
DATE=`date '+%Y-%m-%d %H:%M:%S'`
HOSTNAME=`hostname`

if [ "$BRANCH" == "" ] 
then 
  BRANCH="UNKNOWN" 
fi

echo "Server: $HOSTNAME"
echo "Branch: $BRANCH"
echo "Date: $DATE"
