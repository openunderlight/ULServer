# Install

## Questions?
Find us in Discord: https://discord.gg/yuDngyj

## CentOS 7

### AWS
* Create a new EC2 instance in your region of choice. 
* For now use CentOS. Technically any distro should work but Underlight uses CentOS for its SELinux capabilities. Note: Amazon doesn't have an AMI of its own for CentOS, but you can get it for free in the AMI Marketplace.
* Open ports 7500-7509 TCP in/out.
* Open port 80 if you want an HTTP server.
* Open all inbound/outbound UDP traffic. This will be necessary for agents.
* Log on as centos@ec2-blah.12.34.56.78 with the pem/ppk provided during setup of your instance.
* sudo adduser ulprod
* sudo su
* cd ~ulprod
* cat authorized_keys
* Now paste your public SSH key into here. It can be the same as the one for your root access but it SHOULDN'T BE, I mean are you INSANE?
* chmod 700 .ssh/
* chmod 600 authorized_keys
* restorecon -R -v .ssh

### Build Dependencies -- Run these as root!

`yum install -y epel-release`

`yum update -y`

`yum groupinstall -y "Development Tools"`

```yum install -y bind-utils network-tools pwgen \ 
                p7zip tcsh vim-enhanced screen telnet  \
                wget pth pth-devel gdbm-devel gdbm dbi \
                zlib-devel asciidoc pkgconfig  \
                python34 python34-setuptools perl-DBD-mysql```

`easy_install-3.4 pip`

`yum install -y python3-pip`

_Note: You may not need to easy_install AND yum install pip3, but I couldn't get it working without installing via yum. YMMV._

### Ninja

`cd /usr/src`

`git clone https://github.com/ninja-build/ninja.git`

`cd ninja`

`git checkout release`

`./configure.py --bootstrap`

`./ninja rpm`

`rpm -i <insert ninja RPM filename here>`

Verify: `ninja`

### Meson

`pip3 install meson==0.44`

_Note: ==_

Verify: `meson`

_Note: If this does not work it is possible that meson installed in a directory not in your PATH. For me it installed into /usr/local/bin; check to find the meson binary and ensure that that directory is in your PATH. If not export PATH to the correct place._

### Install MariaDB

`cd /etc/yum.repos.d`
`vim MariaDB.repo`
* Add this to that file in vim
```[mariadb]
name = MariaDB
baseurl = http://yum.mariadb.org/10.3/centos7-amd64
gpgkey = https://yum.mariadb.org/RPM-GPG-KEY-MariaDB
gpgcheck = 1
```

`yum update -y`

`yum install MariaDB-server MariaDB-client MariaDB-devel -y`

`systemctl enable mariadb`

`systemctl start mariadb`

`mysql_secure_installation`

Note: Be sure to remember the root password for MariaDB. This will be needed
for configuring the server after building.

`cd /usr/lib64`
`ln -s libmysqlclient.so.18 libmysqlclient.so`

`vi /etc/my.cnf.d/server.cnf`

Uncomment the line bind-address=0.0.0.0 and enable skip-name-resolve

```
   [mariadb]
   ...
   bind-address=0.0.0.0
   skip-name-resolve
   ...
```

Restart MariaDB

`systemctl restart mariadb`

### Clone Repository & Build

For security it is best to build and run server as a regular user. The 
development environment user is 'uldev'. The test and production servers use 
'ulprod'. The instructions following assume cloning and building will be done 
from the current user home directory.

`git clone https://github.com/openunderlight/ULServer`

`cd $HOME/ulserver/build`

`meson ..`

`ninja`

### Install Server

`cd $HOME/ulserver/build/scripts`

`./install.sh`

Follow the prompts for entering the root password and IP address assignment.

### Load Level Files

Copy the server level files generated by the level editor to $HOME/lyra/src

`cd $HOME/lyra/src`

`make level`

### Start Server

`cd $HOME/lyra/bin`

`./ulctl start`

### Shutdown Server

`cd $HOME/lyra/bin`
`./ulctl stop`
