#!/bin/bash

if [ "$(id -u)" = "0" ]; then 
	echo "Please don't run as sudo"
  exit 1
fi

# Enables egrep-style extended pattern matching
#shopt -s extglob 
# replaced above by doing bin bash instead of tcsh 

# Required directories
INSTALLDIR=$HOME/lyra
SBINDIR=$INSTALLDIR/sbin
BINDIR=$INSTALLDIR/bin
DBDIR=$INSTALLDIR/db
LIBDIR=$INSTALLDIR/lib
SRCDIR=$INSTALLDIR/src
VARDIR=$INSTALLDIR/var

# Required files
HOSTIDTXT=$SRCDIR/hostid.txt
PWTXT=$SRCDIR/pw.txt

install_binaries() {
	echo "Copying game server binaries"
	cp -v ../server/masterd/masterd $SBINDIR
	cp -v ../server/gamed/gamed     $SBINDIR
	cp -v ../server/leveld/leveld   $SBINDIR

	echo "Copying utility binaries"
	# cp -v ../util/!(*@exe) $BINDIR
	#above no longer works replacing with below
	#swipped and modified below from https://stackoverflow.com/questions/44402916/copy-only-executable-files-cross-platform
	find ../util/ -type f -exec test -x {} \; -exec cp -v {} "$BINDIR/" \;
}

install_scripts() {
	echo "Copying scripts"
	cp -v bin/* $BINDIR
	cp -v lib/* $LIBDIR
	cp -v db/* $DBDIR
	cp -v level/* $SRCDIR
}

if [ -e $HOME/lyra ]; then
  echo "Previous install detected. Only binaries and scripts will be replaced."
  install_binaries
  install_scripts
  exit
fi

echo -n "Please enter the database root password:"
read -s ROOTPASS

echo
echo -e "Available IP addresses: " `hostname -I`
echo "If your using this computer for database use 127.0.0.1 \n"
echo -n "Please enter the database IP address:"
read IPADDR

# Initialize variables
#IPADDR=`hostname -I`
DBPORT=3306
DBADMIN="support@example.com"
DBRETURNEMAIL="support@example.com"
DBKEY="H16 90293311ALKWEVB"
DBSALT="3A"
DBPASS=`pwgen 15 1`
DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

echo "Creating directories"
mkdir -v -p $INSTALLDIR $SBINDIR $BINDIR $DBDIR $SRCDIR $VARDIR $LIBDIR
mkdir -v -p $VARDIR/pid $VARDIR/log $VARDIR/text 


install_binaries
install_scripts

echo "1" > $HOSTIDTXT

echo "DBHOST $IPADDR" > $PWTXT
echo "DBPORT $DBPORT" >> $PWTXT
echo "DBADMIN $DBADMIN" >> $PWTXT
echo "DBRETURNEMAIL $DBRETURNEMAIL" >> $PWTXT
echo "DBKEY $DBKEY" >> $PWTXT
echo "DBSALT $DBSALT" >> $PWTXT
echo "ul_item ul_item $DBPASS" >> $PWTXT
echo "ul_player ul_player $DBPASS" >> $PWTXT
echo "ul_guild ul_guild $DBPASS" >> $PWTXT
echo "ul_level ul_level $DBPASS" >> $PWTXT
echo "ul_server ul_server $DBPASS" >> $PWTXT
echo "ul_billing ul_billing $DBPASS" >> $PWTXT

echo "Installing databases"
for DATABASE in ${DATABASES[@]}
do
  mysql -u root -p"$ROOTPASS" -e "create database $DATABASE";
  mysql -u root -p"$ROOTPASS" $DATABASE < sql/$DATABASE.sql
  mysql -u root -p"$ROOTPASS" -e "GRANT ALL ON $DATABASE.* TO '$DATABASE'@'$IPADDR' IDENTIFIED BY '$DBPASS'"
done

mysql -u root -p"$ROOTPASS" -e "UPDATE ul_server.server SET host_name = '$IPADDR'"


echo
echo "Install complete"

