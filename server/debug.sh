#!/bin/bash

PID_USER="ulprod"
#SOURCE_ROOT="/home/brent/lyra/src"
COMMAND=`basename $0`
COMMAND_PATH=`dirname $0`
SOURCE_ROOT=$(readlink -f $COMMAND_PATH/..)
TARGET_PID=""

print_usage () {
  echo "Usage: $COMMAND [OPTIONS] <command>"
  echo "Usage: $COMMAND [OPTIONS] <PID>"
  echo ""
  echo "List of possible commands:"
  echo "  masterd                    Attach to first located masterd"
  echo "  gamed                      Attach to gamed with lowest port"
  echo "  leveld <level>             Attach to leveld running specified level"
  echo ""
  echo "Options:"
  echo "  -r | --root <source_root>  Specify root directory for source files."
  echo "                             Default value is parent of directory where"
  echo "                             this script resides."
  echo ""
  echo "  -p | --pid-user <username> Commands attach to processes running as"
  echo "                             this user. Not used when PID is specified."
  echo "                             Default value is ulprod."
  echo ""
  exit
}

exit_if_target_pid_set() {
  if [ ! -z "$TARGET_PID" ]; then
    print_usage
  fi
}

validate_source_dir() {
  if [ ! -d "$SOURCE_ROOT/server/gamed" ]; then
    echo "'$SOURCE_ROOT' is not a valid source root directory."
    echo ""
    print_usage
  fi
}

validate_target_pid() {
  if [[ $TARGET_PID -eq -1 ]]; then
    echo "Could not find running process for command."
    echo ""
  elif [[ ! $TARGET_PID =~ ^[1-9][0-9]*$ ]]; then
    print_usage
  fi
}

echo ""

while [ $# -gt 0 ]
do
KEY="$1"
shift

case "$KEY" in
  -r|--root)
    SOURCE_ROOT=$(readlink -f $1)
    shift
    ;;
  -p|--pid-user)
    PID_USER=$1
    shift
    ;;
  gamed)
    exit_if_target_pid_set

    # Find pid of gamed process with lowest port running as given user
    TARGET_PID=`ps -ef|awk -v min=99999999 -v user=$PID_USER -v pid=-1 '$1==user && $8~/\/gamed$/ && $10<min {pid=$2;min=$10} END{print pid}'`
    ;;
  leveld)
    exit_if_target_pid_set

    LEVEL=$1
    if [ -z "$LEVEL" ]; then
      print_usage
    fi

    # Find pid of leveld process running given level as given user
    TARGET_PID=`ps -ef|awk -v level=$LEVEL -v user=$PID_USER -v pid=-1 '$1==user && $8~/\/leveld$/ && $11==level {pid=$2} END{print pid}'`
    shift
    ;;
  masterd)
    exit_if_target_pid_set

    # Find pid of masterd running as given user
    TARGET_PID=`ps -ef|awk -v user=$PID_USER -v pid=-1 '$1==user && $8~/\/masterd$/ {pid=$2} END{print pid}'`
    ;;
  *)
    exit_if_target_pid_set
    TARGET_PID=$KEY
    ;;
esac
done

validate_source_dir
validate_target_pid


echo "Attaching to pid $TARGET_PID..."

# remove echo below after tested
echo gdb attach $TARGET_PID -x $COMMAND_PATH/gdbinit -d $SOURCE_ROOT/server/gamed -d $SOURCE_ROOT/server/leveld -d $SOURCE_ROOT/libsrc/server -d $SOURCE_ROOT/libsrc/net -d $SOURCE_ROOT/libsrc/util -d $SOURCE_ROOT/libsrc/smsg -d $SOURCE_ROOT/libsrc/gmsg -d $SOURCE_ROOT/libsrc/rmsg -d $SOURCE_ROOT/libsrc/db -d $SOURCE_ROOT/libsrc/dbi_mysql -d $SOURCE_ROOT/libsrc/gdbm -d $SOURCE_ROOT/libsrc/dbi_gdbm 
