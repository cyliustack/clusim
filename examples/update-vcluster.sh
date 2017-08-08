#!/bin/sh
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

print_misc "Bash version ${BASH_VERSION}."

if [[ "$1" == ""  ]]; then
        print_warning "Usage: ./update-vcluster.sh path-to-newer-container-snapshot(a tar file)"
    exit -1
else
    print_info "Removing old image..."
    sudo docker rmi hpc/node
    print_info "Importing new image..."
    sudo cat $1 | sudo docker import - hpc/node
    print_info "Done."
fi

