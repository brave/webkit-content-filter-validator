ARG BASE_IMAGE

FROM ${BASE_IMAGE}

ARG FUNCTION_DIR=/var/task/

RUN DEBIAN_FRONTEND=noninteractive apt-get update &&\
  apt-get -qq install python3-pip curl

RUN pip3 install --target ${FUNCTION_DIR} awslambdaric &&\
    curl -Lo /usr/local/bin/aws-lambda-rie https://github.com/aws/aws-lambda-runtime-interface-emulator/releases/latest/download/aws-lambda-rie &&\
    chmod +x /usr/local/bin/aws-lambda-rie

COPY * ${FUNCTION_DIR}

WORKDIR ${FUNCTION_DIR}

ENTRYPOINT [ "./ric.sh" ]

CMD [ "app.handler" ]