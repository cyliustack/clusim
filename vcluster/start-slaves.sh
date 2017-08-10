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

if [[ "$1" == "" ]]
then
    print_error "Usage: ./start-slaves.sh NumOfSlaves"
    exit -1
fi 


#print_misc "Bash version ${BASH_VERSION}."

#echo -e "${C_CYAN} Bash version ${BASH_VERSION}. ${C_NONE}"
for ((i = 0; i <= $1-1; i++)); do
    print_info "Lauch $i-th container..."
    sudo docker run -d -p 9000-9900:22 --privileged --name="node$i" vcluster
done
print_info "Done."
