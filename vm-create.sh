#!/bin/bash

function help() {
	echo "$0 [-o osname (e.g., opensuse-tumbleweed)] [-s SIZE_GB (e.g., 64G)] [-f image_filename.img] [-d dvd_path]"
	exit 0
}

function main() {
	while getopts o:s:f:d:h flag
	do
	    case "${flag}" in
	        o) os=${OPTARG};;
	        s) size=${OPTARG};;
	        f) fullname=${OPTARG};;
	        d) dvd=${OPTARG};;
	        h) help;;
	    esac
	done
	
	
	if [[ "${os}" == "" ]];then
		os="opensuse-tumbleweed"
	fi
	
	if [[ "${size}" == "" ]];then
		size="32G"
	fi
	
	if [[ "${fullname}" == "" ]];then
		fullname="${os}.img"
	fi
	
	if [[ "${dvd}" == "" ]];then
		dvd="${os}.iso"
	fi
	echo "OS: ${os}";
	echo "Image Size: ${size}";
	echo "Full Name: ${fullname}";
	if [[ -f "${fullname}" ]];then
		echo "The image file ${fullname} exists. Please remove it manually."
		exit -1
	fi
	qemu-img create -f qcow2 ${fullname} ${size}
	qemu-system-x86_64 -smp 4 -m 2G -drive file=${fullname} -net user,hostfwd=tcp::10022-:22 -net nic -serial stdio -boot d -cdrom ${dvd} 
}

main "$@"
