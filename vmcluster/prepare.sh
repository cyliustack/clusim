#!/bin/bash
WITH_SUDO=""
if [[ $(which sudo) ]]; then 
    WITH_SUDO="sudo" 
fi


if [[ $(which yum) ]]  ; then
    $WITH_SUDO yum install -y qemu-kvm qemu-img virt-manager libvirt tigervnc sshpass 
elif [[ $(which apt) ]] ; then
    $WITH_SUDO apt-get install -y qemu-kvm qemu-img virt-manager libvirt sshpass
fi
$WITH_SUDO systemctl start libvirtd
$WITH_SUDO systemctl enable libvirtd

