#!/bin/bash

# Enables egrep-style extended pattern matching
shopt -s extglob

echo -n "Please enter the database root password:"
read -s ROOTPASS

echo
echo "Available IP addresses: " `hostname -I`
echo -n "Please enter the database IP address:"
read IPADDR

# Initialize variables
#IPADDR=`hostname -I`
DBPORT=3306
DBADMIN="support@underlight.com"
DBRETURNEMAIL="support@underlight.com"
DBKEY="H16 90293311ALKWEVB"
DBSALT="3A"
DBPASS=`pwgen 15 1`
DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

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

echo "Creating directories"
mkdir -v -p $INSTALLDIR $SBINDIR $BINDIR $DBDIR $SRCDIR $VARDIR $LIBDIR
mkdir -v -p $VARDIR/pid $VARDIR/log $VARDIR/text 

echo "Copying game server binaries"
cp -v ../server/masterd/masterd $SBINDIR
cp -v ../server/gamed/gamed     $SBINDIR
cp -v ../server/leveld/leveld   $SBINDIR

echo "Copying utility binaries"
cp -v ../util/!(*@exe) $BINDIR

echo "Copying scripts"
cp ulctl ulctl_vars.csh $BINDIR
cp common.pl $LIBDIR
cp fix_level_items.pl $DBDIR

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
  mysql -u root -p"$ROOTPASS" < $DATABASE.sql
  mysql -u root -p"$ROOTPASS" -e "GRANT ALL ON $DATABASE.* TO '$DATABASE'@'$IPADDR' IDENTIFIED BY '$DBPASS'"
done

mysql -u root -p"$ROOTPASS" -e "UPDATE ul_server.server SET host_name = '$IPADDR'"

echo
echo "Install complete"
