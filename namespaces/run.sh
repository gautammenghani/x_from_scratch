#!/bin/bash
set -e
make
sudo chown root ./namespace
sudo chmod u+s namespace
sudo ./namespace ping 192.168.1.13
