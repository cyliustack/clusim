#!/bin/bash

function help() {
	echo "$0 [-o osname (e.g., opensuse-tumbleweed)] [-p SSH_PORT (e.g., 10022)]"
	exit 0
}

function main() {
	while getopts o:p:h flag
	do
	    case "${flag}" in
	        o) os=${OPTARG};;
	        p) port=${OPTARG};;
	        h) help;;
	    esac
	done
	
	if [[ "${os}" == "" ]];then
		os="opensuse-tumbleweed"
	fi
	
	if [[ "${port}" == "" ]];then
		port="10022"
	fi
	
	drive_path="${os}.img"
	
	echo "OS: ${os}";
	echo "Drive Path: ${drive_path}";
	echo "SSH port: ${port}";
	qemu-system-x86_64 -smp 4 -m 2G -drive file=${drive_path} -net user,hostfwd=tcp::${port}-:22 -net nic -serial stdio
}

main "$@"
