
FROM debian

ENV DEB_PKG="libxml2 libcurl3"
ENV DEB_PKG_TMP="build-essential git libxml2-dev libcurl4-openssl-dev"
# libssl-dev libpq-dev liblua5.1-0-dev libcurl4-openssl-dev libncurses5-dev libxml2-dev libsqlite3-dev"
# ENV DEB_PKG_DEP="libcurl3 libssl1.1 libpq5 liblua5.1-0 libncurses5 libxml2 libsqlite3-0"
# ENV DEB_PKG_DEP="libxml2"

# ENV KAM_INC_MOD="jsonrpcs xhttp diversion xmlrpc db_sqlite uac dialog tls dispatcher statsd htable sqlops db_postgres permissions app_lua http_client mqueue uac_redirect avpops sqlops http_client"
# ENV KAM_SKIP_MOD="uid_domain diversion smsops mediaproxy rtpproxy mqueue topoh app_jsdt tsilo"

RUN echo "building ccc" \
	&& apt-get update && apt-get -y install ${DEB_PKG} ${DEB_PKG_TMP} \
	&& mkdir /git && cd /git && git clone https://github.com/jchavanton/ccc.git && cd ccc \
	&& git submodule update --init && cd xmlrpc && ./configure && make && make install \
	&& cd .. && make

COPY entry.sh /
EXPOSE 4192

# Add Tini
ENV TINI_VERSION v0.17.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--", "/entry.sh"]
