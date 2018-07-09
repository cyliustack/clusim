# Introduction
CLUSIM: A Fast and Scalable Trace-Driven Computer Cluster Simulator  
Authors: Cheng-Yueh Liu, Po-Yao Huang, Yuan-Di Li 

# Prerequisite

## Ubuntu 16.04
`sudo apt-get install docker.io` 

## CentOS 7
`wget https://download.docker.com/linux/centos/7/x86_64/stable/Packages/docker-ce-18.03.1.ce-1.el7.centos.x86_64.rpm`
`sudo yum install docker-ce-18.03.1.ce-1.el7.centos.x86_64.rpm`


# How To Execute a Simple MPI Application on Virtual Container Cluster
## 1. Setup Docker
```
sudo systemctl enable docker
sudo systemctl start docker
```

## 2. Empower Linux Users with tcpdump Tracing on Docker Network Interface

```
cd clusim
./tools/empower-tcpdump.sh username
tcpdump -i docker0 -w justForTest.pcap
```

## 3. Lauch a Docker-based Virtual Cluster and Run a MPI-application
### Option1: Container Cluster
```
cd clusim/vcluster
docker build -t vcluster .
./start-vcluster.sh 4 0
```
Then, we will log in to master node (node0) 
```
cd ~ && ./demo-npb.sh bt A 4
exit 
```
### Option2: VM Cluster
Download the prebuilt ubuntu 16.04 image (url:"https://drive.google.com/open?id=1-xM6ZJ-q4Y9OxPwe6NDh9Yv6aKlJ7b-n"), and place it at /image/ as /image/ubuntu1604.img 
```
cd clusim/vmcluster
./vmcluster update-image
./start-vcluster.sh 4 0
``` 
Then, we will log in to clusim0 whose ip is 172.17.0.2 
```
./demo-npb.sh bt A 4
exit
```
### Snapshot of Demo 
![Alt text](demo/mpi-nas-bt.png)

## 4. Build NS-3
```
cd clusim/ns3
./build.py 
```

## 5. Using Clusim to Simulate and Analyze Network Performance
Modify the path to the logged pcap file in simple.cfg  
```
cd clusim  
make 
cp config/default.cfg .
./clusim default.cfg 
```
