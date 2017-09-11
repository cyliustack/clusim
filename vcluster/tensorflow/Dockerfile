FROM ubuntu:16.04
MAINTAINER Cheng-Yueh Liu <cyliustack@gmail.com>
RUN apt-get update && apt-get install -y openssh-server sshpass gcc g++ make  iputils-ping git vim wget gfortran iperf python-pip python-dev curl pssh 
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
EXPOSE 22
RUN echo -e  'y\n'| ssh-keygen -N ''  -f /root/.ssh/id_rsa
RUN echo "root" > /root/password.txt 
RUN cat /root/password.txt 
RUN cd /root && git clone https://github.com/cyliustack/benchmark 
RUN cp /root/benchmark/tools/ssh_copy_id_to_all.sh /root/
RUN wget http://www.mpich.org/static/downloads/3.2/mpich-3.2.tar.gz
RUN tar -xvf mpich-3.2.tar.gz
RUN cd mpich-3.2 && ./configure --prefix=/opt/mpich && make -j4 && make install
RUN cd /root/benchmark/mpi/gemm && make
RUN echo export PATH=/opt/mpich/bin:$PATH >> ~/.bashrc
RUN echo export TERM=xterm >> ~/.bashrc
RUN echo export LD_LIBRARY_PATH=/opt/mpich/lib:$LD_LIBRARY_PATH >> ~/.bashrc
RUN echo "    StrictHostKeyChecking no" >> /etc/ssh/ssh_config
#RUN cp /root/benchmark/mpi/tools/demo.sh /root

#install tensorflow
RUN pip install tensorflow
RUN cd /root && git clone https://github.com/tensorflow/benchmarks.git TensorflowBenchmarks
RUN wget https://raw.githubusercontent.com/softDi/clusim/master/vcluster/tensorflow/TFdemo.sh
RUN chmod +x TFdemo.sh
RUN cd /root && wget https://raw.githubusercontent.com/softDi/clusim/master/vcluster/tensorflow/demo.sh && chmod +x demo.sh
#install java and bazel (optional)
#RUN apt-get update && apt-get upgrade -y && apt-get install -y  software-properties-common && add-apt-repository ppa:webupd8team/java -y && apt-get update && echo oracle-java7-installer shared/accepted-oracle-license-v1-1 select true | /usr/bin/debconf-set-selections && apt-get install -y oracle-java8-installer && apt-get clean
#RUN echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" |  tee /etc/apt/sources.list.d/bazel.list
#RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
#RUN apt-get update && apt-get install -y bazel

#RUN 
CMD ["/usr/sbin/sshd", "-D"]





