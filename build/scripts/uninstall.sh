#!/bin/bash

if [ "$(id -u)" = "0" ]; then 
	echo "Please don't run with sudo"
  exit 1
fi
DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

echo -n "Please enter the database root password:"
read -s ROOTPASS

echo "Available IP ADDRESSES: "`hostname -I`
echo -n "it's possible it's localhost, or 127.0.0.1"
echo -n "What is the ip to your database server: "
read IPADDR

# Required directories
INSTALLDIR=$HOME/lyra

rm -rf $INSTALLDIR

for DATABASE in ${DATABASES[@]}
do
   mysql -u root -p"$ROOTPASS" -e "DROP DATABASE $DATABASE"
   mysql -u root -p"$ROOTPASS" -e "DROP USER $DATABASE@$IPADDR";
done

echo
echo "Done"
