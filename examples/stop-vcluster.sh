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

#print_misc "Bash version ${BASH_VERSION}."

if [[ "$(sudo docker ps -aq)" == ""  ]]; then
    print_warning "No containers to be stopped."
else
    print_info "Stopping containers..."
    sudo docker stop $(sudo docker ps -aq)
    print_info "Removing containers..."
    sudo docker rm $(sudo docker ps -aq)
    print_info "Done."
fi

./show-vcluster.sh
