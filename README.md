# Introduction
CLUSIM: A Fast and Scalable Trace-Driven Computer Cluster Simulator  
Authors: Cheng-Yueh Liu, Po-Yao Huang 

# Prerequisite

## Ubuntu 16.04
`sudo apt-get install docker.io` 

## CentOS 7
`sudo yum install docker`


# How To Execute a Simple MPI Application on Virtual Container Cluster
## 1. Setup Docker
```
sudo systemctl enable docker
sudo systemctl start docker
```

## 2. Empower Linux Users with tcpdump Tracing on Docker Network Interface

```
./empower-tcpdump.sh username
tcpdump -i docker0 -w justForTest.pcap
```

## 3. Lauch a Docker-based Virtual Cluster and Run a MPI-application
```
cd clusim/vcluster
docker build -t vcluster .
./start-vcluster.sh 4 0
```
Then, we will log in to master node (node0) 
```
cd ~ && ./demo.sh
```
### Snapshot of Demo 
![Alt text](demo/mpi-nas-bt.png)



## 4. Download, Build and Test NS-3
```
Step I:  
cd clusim
wget http://www.nsnam.org/release/ns-allinone-3.26.tar.bz2   
tar xjf ns-allinone-3.26.tar.bz2   
ln -s ns-allinone-3.26 ns3   

Step II:     
cd ns3   
./build.py --enable-examples --enable-tests   

Step III:      
cd ns-3.26   
./waf --run hello-simulator   
cp examples/tutorial/first.cc scratch/myfirst.cc   
./waf --run scratch/myfirst   
```

## 5. Using Clusim to Simulate and Analyze Network Performance
Modify the path to the logged pcap file in simple.cfg  
```
cd clusim  
make 
./clusim default.cfg 
```
