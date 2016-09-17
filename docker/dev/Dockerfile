#
# Docker for Code compiling and production
# Author: Endre Boock <dev@tibock.de>
#

FROM ubuntu:trusty

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y libc6-dev zlib1g-dev libxml2-dev valgrind g++ make
RUN apt-get install -y git gnupg vim

VOLUME ["/src"]

ENTRYPOINT ["bash", "-c"]
CMD ["bash"]
