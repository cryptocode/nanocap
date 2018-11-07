FROM ubuntu:18.04
MAINTAINER cryptocode <cryptocode@zolo.io>
LABEL Description="Protocol analyzer for the Nano network"
EXPOSE 7077

RUN apt-get update && apt-get install -y \
	apt-transport-https
RUN apt-get update && apt-get install -y \
    software-properties-common
RUN add-apt-repository universe
RUN apt-get update && apt-get install -y \
	curl \
	wget \
    git \
    cmake \
    python3.4 \
    python3-pip

RUN mkdir /nanocap
RUN cd /nanocap
RUN cmake --version

RUN wget https://bootstrap.pypa.io/get-pip.py
RUN python3 get-pip.py 'pip==18.1'
RUN pip install conan --ignore-installed urllib3

WORKDIR /nanocap
RUN git clone https://github.com/cryptocode/nanocap.git .

RUN conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
RUN conan remote add andreybronin https://api.bintray.com/conan/andreybronin/conan
RUN conan install . --build missing
RUN cmake -G "Unix Makefiles" .
RUN make

CMD bin/nanocap --help
