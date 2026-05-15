#!/bin/bash
sudo a2enmod proxy_fcgi setenvif
sudo a2enmod headers ssl 
sudo systemctl restart php8.5-fpm
sudo systemctl restart apache2
#sudo systemctl restart caddy
