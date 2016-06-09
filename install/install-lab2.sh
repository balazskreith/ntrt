rm -R mptsrv-0.25
tar -xzvf mptsrv-0.25.tar.gz
sudo cp configs/*.* /usr/local/etc/mptsrv/conf/
sudo cp configs/bash/*.* /usr/local/etc/mptsrv/bash/
sudo cp configs/connections/*.* /usr/local/etc/mptsrv/conf/connections/

sudo chmod 666 /usr/local/etc/mptsrv/conf/*.*
sudo chmod 666 /usr/local/etc/mptsrv/conf/connections/*.*
sudo chmod 777 /usr/local/etc/mptsrv/bash/*.*

cd mptsrv-0.25
./configure
sudo make install

