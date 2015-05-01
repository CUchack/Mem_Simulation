#!/bin/bash 

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -t libquantum &

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-All-2way -t libquantum&

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-All-4way -t libquantum &

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA -t libquantum

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA-L2big -t libquantum &

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-2way -t libquantum&

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t libquantum &

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t libquantum

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small-4way -t libquantum& 

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-4way -t libquantum&

zcat ../Large\ Traces/libquantum.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-big -t libquantum 

