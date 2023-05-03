FROM ubuntu:22.04

RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
  apt-get -qq install libwpewebkit-1.0-dev build-essential

COPY . /opt/webkit-content-filter-validator

WORKDIR /opt/webkit-content-filter-validator

RUN make webkit-content-filter-validator

CMD /opt/webkit-content-filter-validator/webkit-content-filter-validator
