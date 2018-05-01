#!/bin/bash
apt install -y libmunge-dev libmunge2 munge

rm /etc/munge/munge.key

dd if=/dev/urandom bs=1 count=1024 > /etc/munge/munge.key

chown munge:munge /etc/munge/munge.key

chmod 400 /etc/munge/munge.key

sed -i '/munge/c\munge:x:501:501::/var/run/munge;/sbin/nologin'  /etc/passwd

/etc/init.d/munge start

munge -n | unmunge

apt install -y slurm-wlm slurm-wlm-doc

# Install munge in each node of cluster
apt install -y libmunge-dev libmunge2 munge

# Copy munge.key file from server to each node from cluster
scp /etc/munge/munge.key root@node:/etc/munge/munge.key

# on each node
chown munge:munge /etc/munge/munge.key
chmod 400 /etc/munge/munge.key

apt-get install -y slurmd

/etc/init.d/slurmd start

/etc/init.d/slurmctld start


