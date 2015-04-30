#!/bin/bash 
bin/Debug/./Memory\ Simulator -f config_file -t 5M-bzip2 < ../traces-5M/bzip2 & 

bin/Debug/./Memory\ Simulator -f config_file -t 5M-h264ref < ../traces-5M/h264ref &


bin/Debug/./Memory\ Simulator -f config_file -t 5M-libquantum < ../traces-5M/libquantum

bin/Debug/./Memory\ Simulator -f config_file -t 5M-omnetpp.gz < ../traces-5M/omnetpp &


bin/Debug/./Memory\ Simulator -f config_file -t 5M-sjeng < ../traces-5M/sjeng &



