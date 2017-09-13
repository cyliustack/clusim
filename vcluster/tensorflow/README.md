# Introduction
Executing TensorFlow on the CLUSIM.

# Prerequisite
## Ubuntu 16.04
`sudo apt-get install docker.io`
## CentOS 7
 `sudo yum install docker`


# How To Execute a Simple TensorFlow training on Virtual Container Cluster
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

## 3-1. Lauch a Docker-based Virtual Cluster and Run a TensorFlow training on 4 nodes
 ```
 cd clusim/vcluster/tensorflow
 docker build -t vcluster-tensorflow .
  ./start-vcluster.sh 4 0
 ```
 Then, we will log in to master node (node0)
 ```
 cd ~ && ./demo.sh 4
 ```
 ## 3-2 Lauch a Docker-based Virtual Cluster and Run a TensorFlow training on 8 nodes
 ```
 cd clusim/vcluster/tensorflow
 docker build -t vcluster-tensorflow .
  ./start-vcluster.sh 8 0
 ```
 Then, we will log in to master node (node0)
 ```
 cd ~ && ./demo.sh 8
 ```
