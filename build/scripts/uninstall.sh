#!/bin/bash

if [ "$(id -u)" = "0" ]; then 
	echo "Please don't run with sudo"
  exit 1
fi
DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

echo -n "Please enter the database root password:"
read -s ROOTPASS

# Required directories
INSTALLDIR=$HOME/lyra

rm -rf $INSTALLDIR

for DATABASE in ${DATABASES[@]}
do
   mysql -u root -p"$ROOTPASS" -e "DROP DATABASE $DATABASE"
done

echo
echo "Done"
