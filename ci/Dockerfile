FROM ubuntu:20.04
LABEL Description="Protocol analyzer for the Nano network"
EXPOSE 8077

ENV DEBIAN_FRONTEND=noninteractive
RUN ln -fs /usr/share/zoneinfo/UTC /etc/localtime

RUN apt-get update && apt-get install -y \
    apt-transport-https \
    software-properties-common \
    curl \
    wget \
    cmake \
    python3.4 \
    python3-pip \
    git \  
    g++    

# Add the required libraries. 
# Please note that the package names may vary between Ubuntu versions.
RUN apt-get update && apt-get install -y \
    libboost-all-dev \
    libjsoncpp-dev \
    libpcap-dev

# Create a directory for the project
RUN mkdir /nanocap
WORKDIR /nanocap

# Clone and install PcapPlusPlus
RUN git clone https://github.com/seladb/PcapPlusPlus.git
WORKDIR PcapPlusPlus
RUN git checkout 8f87300e4f308bfaa62b76f6d43df3e662c12b9e
RUN ./configure-linux.sh --default
RUN make all
RUN make install
WORKDIR /nanocap

# Copy the local project directory into the Docker image
COPY . /nanocap

# Download json.hpp from JSONForModernCPP GitHub repository
RUN mkdir -p /nanocap/external/JSONForModernCPP/include/nlohmann
RUN curl -Lo /nanocap/external/JSONForModernCPP/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp

# Modify CMakeLists.txt to use the downloaded json.hpp file
RUN sed -i 's|include_directories (${Boost_INCLUDE_DIRS})|include_directories (${Boost_INCLUDE_DIRS} /nanocap/external/JSONForModernCPP/include)|' /nanocap/CMakeLists.txt

RUN dpkg-reconfigure --frontend noninteractive tzdata
RUN cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Unix Makefiles" .
RUN make

CMD ./nanocap --help
