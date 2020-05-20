# DAB-FM-sundtek-api
DAB FM using sundtek api  

#Prerequest  

Sundtek DAB/FM Dongle by Namatek  
qt5  

# Install  

download and install sundtek drivers:  

open a terminal  

sudo -s  
cd /tmp  
wget http://sundtek.de/media/sundtek_netinst.sh  
chmod 777 sundtek_netinst.sh  
./sundtek_netinst.sh  
exit  


download and install qt5 if missing:  

open a terminal if closed before  

sudo apt-get update  
sudo apt-get install qt5-default  

download and install DAB/FM app:  

open a terminal if closed before  

git clone https://github.com/Vitalic66/DAB-FM-sundtek-api.git  
cd DAB-FM-sundtek-api  
qmake dab_fm_suntek_api.pro  
make

start program:  
./DAB_FM_sundtek  
