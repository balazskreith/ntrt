#!/bin/sh
if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit;
fi

cd ..
autoreconf -iv
./configure
make install
mkdir /usr/local/etc/mptsrv
mkdir /usr/local/etc/mptsrv/conf
cp install/$1/*.* /usr/local/etc/mptsrv/conf -R
cp install/$1/connections/*.* /usr/local/etc/mptsrv/conf/connections -R

mkdir /usr/local/etc/mptsrv/bash
cp install/bash/*.* /usr/local/etc/mptsrv/bash -R
chmod 777 /usr/local/etc/mptsrv/bash/*.*

#other peer
make dist

sshpass -p 'Ent3r27' scp mptsrv-0.25.tar.gz balazs@172.16.1.12:/home/balazs/mptsrv-0.25.tar.gz
sshpass -p 'Ent3r27' scp install/lab2-conf/*.* balazs@172.16.1.12:/home/balazs/configs/
sshpass -p 'Ent3r27' scp install/lab2-conf/connections/*.* balazs@172.16.1.12:/home/balazs/configs/connections/
sshpass -p 'Ent3r27' scp install/bash/*.* balazs@172.16.1.12:/home/balazs/configs/bash/
sshpass -p 'Ent3r27' scp install/install-lab2.sh balazs@172.16.1.12:/home/balazs/install.sh
sshsudo-lab2 -u balazs 172.16.1.12 chmod 700 install.sh
sshsudo-lab2 -u balazs 172.16.1.12 ./install.sh