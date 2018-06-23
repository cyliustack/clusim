#!/bin/bash
if [[ $(which yum) ]]  ; then
    yum install qemu-kvm qemu-img virt-manager libvirt tigervnc
elif [[ $(which apt) ]] ; then
    apt-get install qemu-kvm qemu-img virt-manager libvirt
fi
systemctl start libvirtd
systemctl enable libvirtd

