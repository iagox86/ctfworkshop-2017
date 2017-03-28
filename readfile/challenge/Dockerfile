FROM ubuntu:xenial

RUN adduser --disabled-password --gecos '' ctf

RUN apt-get update && apt-get install -y xinetd gcc make libc6-dev-i386 libssl-dev coreutils

WORKDIR /home/ctf
ADD src/* /home/ctf/
RUN make
RUN chown -R root:root .

# Set up xinetd
ADD xinetd.conf /etc/xinetd.conf
RUN mkdir /var/log/xinetd/

ADD readfile.xinetd /etc/xinetd.d/readfile

USER root
EXPOSE 6124

CMD service xinetd restart && tail -F /var/log/xinetd/xinetd.log
