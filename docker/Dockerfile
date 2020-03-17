
FROM debian:stretch
ENV VERSION="0.0.6"
ENV DEB_PKG="libxml2 libcurl3"
ENV DEB_PKG_TMP="build-essential git libxml2-dev libcurl4-openssl-dev"

RUN echo "building CCA (call control agent)" \
	&& apt-get update && apt-get -y install ${DEB_PKG} ${DEB_PKG_TMP} \
	&& mkdir /git && cd /git && git clone https://github.com/jchavanton/cca.git && cd cca \
	&& git checkout ${VERSION} \
	&& git submodule update --init && cd xmlrpc && ./configure && make && make install \
	&& cd .. && make && make install \
	&& cd .. && rm -rf cca \
	&& apt-get remove -y ${DEB_PKG_TMP} \
	&& apt-get clean && apt-get autoclean && apt-get -y autoremove

COPY Dockerfile /
COPY entry.sh /
EXPOSE 4192

# Add Tini
ENV TINI_VERSION v0.17.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--", "/entry.sh"]
