# nativejson-benchmark
# build with: docker build -t nativejson-benchmark .
FROM debian:jessie

RUN buildDeps='build-essential gcc-multilib g++-multilib git-core curl ca-certificates php5-cli libboost-all-dev'; \
    set -x \
    && apt-get update && apt-get install --no-install-recommends -y $buildDeps

COPY . /nativejson-benchmark
WORKDIR /nativejson-benchmark
ENV PATH $PATH:/nativejson-benchmark/build

RUN set -x \
	&& cd /nativejson-benchmark/build \
	&& curl -L -s https://github.com/premake/premake-core/releases/download/v5.0.0-alpha7/premake-5.0.0-alpha7-linux.tar.gz | tar -xvz \
	&& chmod +x premake5 && chmod +x premake.sh && sync && /bin/sh -c ./premake.sh && ./machine.sh \
	&& cd /nativejson-benchmark && make \
	&& cd /nativejson-benchmark/bin \
	&& for t in *; do ./$t; done \
	&& cd /nativejson-benchmark/result && make

VOLUME ["/nativejson-benchmark/output"]
