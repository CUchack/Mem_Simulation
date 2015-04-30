#!/bin/bash 
zcat ../Large\ Traces/bzip2.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t bzip2 &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t h264ref &


zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t libquantum &
 
zcat ../Large\ Traces/omnetpp.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t omnetpp


zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t sjeng &


zcat ../Large\ Traces/bzip2.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t bzip2 &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t h264ref &


zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t libquantum

zcat ../Large\ Traces/omnetpp.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t omnetpp &


zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t sjeng

