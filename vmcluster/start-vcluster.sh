#!/bin/bash
C_NONE="\033[0m"
C_CYAN="\033[36m"
C_RED="\033[31m"
C_GREEN="\033[32m"
C_ORANGE="\033[33m"
C_BLUE="\033[34m"
C_PURPLE="\033[35m"
C_CYAN="\033[36m"
C_LIGHT_GRAY="\033[37m"

print_misc() {
    echo -e "${C_PURPLE} $1 ${C_NONE}"
}

print_info() {
    echo -e "${C_BLUE} $1 ${C_NONE}"
}

print_error() {
    echo -e "${C_RED} $1 ${C_NONE}"
}

print_warning() {
    echo -e "${C_ORANGE} $1 ${C_NONE}"
}
if [[ "$1" == "" ]] || [[ "$2" == "" ]]
then
    print_error "Usage: ./start-vcluster.sh NumOfNodes MasterNodeID"
    exit -1
fi 

num_nodes=$1
master_node_id=$2
print_misc "Bash version ${BASH_VERSION}."

print_info "./vmcluster create ${num_nodes}"
./vmcluster ${num_nodes} 
print_info "Done"

tcpdump -w vcluster.pcap -i virbr0 2> /dev/null  & 

print_info "./vmcluster login ${master_node_id}"
sshpass -p clusim ssh clusim@172.17.0.2 
print_info "Virtual cluster is normally closed."
pkill tcpdump
