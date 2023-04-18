FROM ubuntu:22.04

RUN apt update
RUN apt install -y libwpewebkit-1.0-dev build-essential

COPY . /opt/webkit-content-filter-validator
WORKDIR /opt/webkit-content-filter-validator

RUN make webkit-content-filter-validator

CMD /opt/webkit-content-filter-validator/webkit-content-filter-validator
