Install
Questions?
Find us in Discord: https://discord.gg/yuDngyj

You may use AWS  new ECT instance in region of choice or as I did for this virtualbox, instructions on my setup below:

VIRTUALBOX ubuntu setup / config:

Ubuntu 18.04 LTS  https://www.ubuntu.com/download/server
Virtualbox 5.2.18r124319 https://www.virtualbox.org/wiki/Downloads


Following are my settings, yours may vary.

Open Virtualbox Manager
Create New vm
Name: Ubuntu
Type: Linux
Version Ubuntu (64-bit)

Memory Size: 8192 (or whatever you want to dedicate to it)


Create a virtual hard disk now
VDI
Dynamically Allocated


type in or select by clicking file folder icon, where you want your virtual hard disk to be created.
I choose D:\vboxdrive\ubuntu.vdi
yours may be C:\Users\whateveryourusernameis\VirtualBox VMs\ubuntu
or wherever you choose and have space.

Select disk space you want to give it with slider bar or type it.
all in all should take up around 5.1gb installed but give space to grow
10gb should be a good starting point.


Now you should see your Ubuntu vm, Left click it once, then click on settings.
Click on network tab, click on attached to: drop down, select Bridged Adapter click ok.


You'll see a pop up, "Select start-up disk"
click on the little folder with green arrow, find and select 
ubuntu-18.04.1-live-server-amd64.iso (or appropriate name)


Select English or whatever language prefered.


Install Ubuntu


select network


enter proxy server if you have one


mirror for alt install if you use it.


Use entire disk


select disk
done
The development environment user is 'uldev'. The test and production servers use 
'ulprod'. The instructions following assume cloning and building will be done 
from the current user home directory.

name: ulprod
server name: ulprod
username: ulprod
pass: thisisabadpasswordchangeme
pass: thisisabadpasswordchangeme

install any packages or popular "snaps"
selected none


Installation finished
reboot now



Setting up UBUNTU for ul server:

Login: ulprod
Pass: thisisabadpasswordchangeme


sudo apt update && sudo apt dist-upgrade && sudo apt autoremove
Pass:thisisabadpasswordchangeme

sudo do-release-upgrade -d

sudo apt-get update
reboot to make sure changes are applied if updated.

sudo add-apt-repository main
sudo add-apt-repository universe
sudo add-apt-repository restricted
sudo add-apt-repository multiverse
sudo apt update
reboot to make sure changes are applied if updated.

sudo apt-get install build-essential mtools python-pip python3-pip ninja-build meson libpth-dev libgdbm-dev tcsh pwgen libapache-dbi-perl -y
sudo apt update
reboot to make sure changes are applied if updated.

Install MariaDB

sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0xF1656F24C74CD1D8

sudo add-apt-repository 'deb [arch=amd64] http://mirror.zol.co.zw/mariadb/repo/10.3/ubuntu bionic main'

sudo apt update
sudo apt install software-properties-common mariadb-server libmysqlclient-dev -y
Enter root password (KEEP THIS SAFE, and remember it please)

sudo vi /etc/mysql/mariadb.conf.d/50-server.cnf
add in [mysqld] section
plugin-load-add = auth_socket.so

Uncomment the line bind-address=0.0.0.0 and enable skip-name-resolve
Press insert on keyboard to edit
[mariadb]
   ...
   bind-address=0.0.0.0
   skip-name-resolve
   ...

save and exit :wq!

sudo systemctl stop mariadb.service
sudo systemctl start mariadb.service
sudo systemctl enable mariadb.service

sudo mysql
use mysql;
update user set plugin='' where User='root';
flush privileges;
exit

mysql -u root -p
To test (should login with password, type to leave it)

mysql_secure_installation
ALLOW connections remotely? Y


lets check firewall and open ports.

sudo ufw status
Status:inactive

sudo ufw enable
Firewall is active and enabled on system startup

sudo ufw allow 7500:7509/tcp
sudo ufw allow 1:65535/udp
sudo ufw allow 22/tcp
sudo ufw allow 3306/tcp

confirm open ports ..
sudo ufw status

Clone Repository & Build
For security it is best to build and run server as a regular user. The development environment user is 'uldev'. The test and production servers use 'ulprod'. The instructions following assume cloning and building will be done from the current user home directory.

cd ~

git clone -b ubuntu_build https://github.com/openunderlight/ULServer

cd $HOME/ULServer/build

meson ..

ninja

cd ~/ULServer/build/scripts



./install.sh
enter the database root password you made before.

Available IP addresses 
type in whatever is available

cd ~/lyra/bin

To start the server:
./ulctl start

To stop the server:
./ulctl start

To verify it's running.
ps -ef | grep game
