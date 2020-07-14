FROM node:12

RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app

COPY . /usr/src/app

RUN apt add --no-cache libpq
RUN esy install

CMD esy b dune exec backend/App.exe 
