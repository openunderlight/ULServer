#!/bin/bash

DATABASES=(ul_billing ul_guild ul_item ul_level ul_player ul_server)

echo -n "Please enter database username: "
read DBUSERNAME
echo -n "Please enter database password:"
read -s DBPASSWORD

for DATABASE in ${DATABASES[@]}
do
  mysql -u"$DBUSERNAME" -p"$DBPASSWORD" < "$DATABASE".sql 
done 
