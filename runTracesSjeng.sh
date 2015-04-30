#!/bin/bash 

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -t sjeng &

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-All-2way -t sjeng&

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-All-4way -t sjeng &

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA -t sjeng

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA-L2big -t sjeng &

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-2way -t sjeng&

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t sjeng &

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t sjeng

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small-4way -t sjeng& 

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-4way -t sjeng&

zcat ../Large\ Traces/sjeng.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-big -t sjeng 

