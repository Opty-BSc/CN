#!/usr/bin/env bash

sudo apt-get update
sudo apt-get -y install core-network
sudo apt-get -y install quagga
sudo add-apt-repository ppa:wireshark-dev/stable -y
sudo apt-get update
sudo DEBIAN_FRONTEND=noninteractive apt-get -y install wireshark-gtk
sudo addgroup -system wireshark
sudo chown root:wireshark /usr/bin/dumpcap
sudo setcap cap_net_raw,cap_net_admin=eip /usr/bin/dumpcap
sudo usermod -a -G wireshark $USER

# install additional network tools
sudo apt-get -y install nmap
sudo apt-get -y install traceroute
sudo apt-get -y install whois

# update Locale
sudo locale-gen UTF-8

# copy examples and configs (from inside the CORE node)
sudo cp -a /vagrant/data/icons/normal/* /usr/share/core/icons/normal/
sudo cp -a /vagrant/data/icons/tiny/* /usr/share/core/icons/tiny/
sudo cp -a /vagrant/data/core/nodes.conf /home/vagrant/.core/
sudo cp -a /vagrant/data/core/prefs.conf /home/vagrant/.core/
sudo chmod 664 /home/vagrant/.core/*.conf
sudo chown -R vagrant /home/vagrant/.core

# configure core.conf file for myservices
sudo sed -i 's/\#custom_services_dir = \/home\/username/custom_services_dir = \/home\/vagrant/' /etc/core/core.conf
# restart core
sudo service core-daemon restart
