FROM node:15

USER root
ENV HOME /home/app
WORKDIR /home/app

RUN apt-get update && \
  apt-get install -y build-essential python3-dev python3-pip \
  python3-setuptools python3-wheel python3-cffi libcairo2 libpango-1.0-0 \
  libpangocairo-1.0-0 libgdk-pixbuf2.0-0 libffi-dev shared-mime-info && \
  pip3 install --no-cache-dir weasyprint && \
  apt-get clean

RUN npm install -g @enspirit/dem-bs

ADD bases/markdown-css.css /home/app/index.css
ADD bases/empty.json /home/app/index.json
ADD README.md /home/app/index.md

CMD ["dem"]
