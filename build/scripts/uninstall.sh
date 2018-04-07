#!/bin/bash

# Enables egrep-style extended pattern matching
shopt -s extglob

echo -n "Please enter the database root password:"
read -s ROOTPASS

DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

# Required directories
INSTALLDIR=$HOME/lyra

rm -rf $INSTALLDIR

for DATABASE in ${DATABASES[@]}
do
  mysql -u root -p"$ROOTPASS" -e "DROP DATABASE $DATABASE"
done

echo
echo "Done"
