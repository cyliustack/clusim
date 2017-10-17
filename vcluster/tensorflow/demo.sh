#!/bin/bash
~/benchmark/tools/ssh_copy_id_to_all.sh $1
~/benchmark/tools/genhosts.sh $1
if [[ "$1" == 4 ]]; then
	parallel-ssh -h hosts.txt -t 18000 "cd / && ./TFdemo.sh"
fi
if [[ "$1" == 8 ]]; then
	parallel-ssh -h hosts.txt -t 36000 "cd / && ./TF8nodes.sh"
fi