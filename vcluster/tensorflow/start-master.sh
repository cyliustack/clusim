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

if [[ "$1" == "" ]]
then
    print_error "Usage: ./start-master.sh master_nid"
    exit -1
fi 
master_nid=$1
#print_misc "Bash version ${BASH_VERSION}."
#echo -e "${C_CYAN} Bash version ${BASH_VERSION}. ${C_NONE}"
print_info "Execute bash on master container at node${master_nid}:"
    sudo docker exec -it TFnode${master_nid} bash 
print_info "Done."
