FROM ubuntu:16.04

RUN apt-get update && apt-get install -y openssh-server sshpass gcc g++ make  iputils-ping git vim wget
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
RUN cd /root && git clone https://github.com/cyliustack/sofa 
RUN cd /root && git clone https://github.com/cyliustack/benchmark 
RUN cd /root && git clone https://github.com/cyliustack/easybuild
RUN cp /root/benchmark/tools/ssh_copy_id_to_all.sh /root/

#RUN 
CMD ["/usr/sbin/sshd", "-D"]




