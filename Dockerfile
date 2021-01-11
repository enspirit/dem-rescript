FROM node:15.3

USER root
ENV HOME /dem
WORKDIR /dem

RUN apt-get update && \
  apt-get install -y build-essential python3-dev python3-pip \
  python3-setuptools python3-wheel python3-cffi libcairo2 libpango-1.0-0 \
  libpangocairo-1.0-0 libgdk-pixbuf2.0-0 libffi-dev shared-mime-info && \
  pip3 install --no-cache-dir weasyprint && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

RUN npm install -g @enspirit/dem-rescript

ADD bases/newcss/index.css  /dem/index.css
ADD bases/newcss/index.json /dem/index.json
ADD README.md /dem/index.md

ENTRYPOINT ["dem"]
