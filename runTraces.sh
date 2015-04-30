#!/bin/bash 
zcat ../Large\ Traces/bzip2.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t ../Large\ Traces/bzip2.gz&

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t ../Large\ Traces/h264ref.gz&


zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t ../Large\ Traces/libquantum.gz&

zcat ../Large\ Traces/omnetpp.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t ../Large\ Traces/omnetpp.gz


zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t ../Large\ Traces/sjeng.gz&


zcat ../Large\ Traces/bzip2.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t ../Large\ Traces/bzip2.gz&

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t ../Large\ Traces/h264ref.gz&


zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t ../Large\ Traces/libquantum.gz

zcat ../Large\ Traces/omnetpp.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t ../Large\ Traces/omnetpp.gz&


zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t ../Large\ Traces/sjeng.gz

