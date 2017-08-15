FROM ubuntu:16.04
MAINTAINER Cheng-Yueh Liu <cyliustack@gmail.com>
RUN apt-get update && apt-get install -y openssh-server sshpass gcc g++ make  iputils-ping git vim wget gfortran iperf
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
RUN cp /root/benchmark/mpi/tools/demo.sh /root

#RUN 
CMD ["/usr/sbin/sshd", "-D"]





