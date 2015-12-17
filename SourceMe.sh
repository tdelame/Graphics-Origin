#!/bin/bash
export project="Graphics Origin"
source scripts/start.sh

#starting the project mode, to export aliases and set variables
__start
export | grep "_started=\"true\"" &> /dev/null
if [ ! $? ]
then 
  echo "$project mode not started."
  echo "use: source SourceMe.sh to launch it correctly."
fi

#add some functions to ease management
source scripts/switch.sh #switch configuration
source scripts/stop.sh #stop project mode

if [ ! -e bin/build_conf.mk ]
then 
  switch release
fi
