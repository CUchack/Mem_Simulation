#!/bin/bash 

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -t h264ref &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-All-2way -t h264ref&

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-All-4way -t h264ref &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA -t h264ref

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-All-FA-L2big -t h264ref &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-2way -t h264ref&

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-8way -t h264ref &

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small -t h264ref

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L1-small-4way -t h264ref& 

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-4way -t h264ref&

zcat ../Large\ Traces/h264ref.gz | bin/Debug/./Memory\ Simulator -f config_file-L2-big -t h264ref 

