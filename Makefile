SHELL = /bin/bash

define newline


endef

EMIT = echo -e '$(subst $(newline),\n,$(1))'

define DOCKERFILE_ESY
# start from node image so we can install esy from npm

FROM node:10.13-alpine as build

ENV TERM=dumb \
		LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib

RUN mkdir /esy
WORKDIR /esy

ENV NPM_CONFIG_PREFIX=/esy
RUN npm install -g --unsafe-perm esy@0.4.3

# now that we have esy installed we need a proper runtime

FROM alpine:3.8 as esy

ENV TERM=dumb \
		LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib

WORKDIR /

COPY --from=build /esy /esy

RUN apk add --no-cache \
		ca-certificates wget \
		bash curl perl-utils \
		git patch gcc g++ musl-dev make m4

RUN wget -q -O /etc/apk/keys/sgerrand.rsa.pub https://alpine-pkgs.sgerrand.com/sgerrand.rsa.pub
RUN wget https://github.com/sgerrand/alpine-pkg-glibc/releases/download/2.28-r0/glibc-2.28-r0.apk
RUN apk add --no-cache glibc-2.28-r0.apk

ENV PATH=/esy/bin:$$PATH
endef

define DOCKERIGNORE
.git
node_modules
_esy
esyd
endef

define DOCKERFILE_DEPS
FROM $(1)
RUN mkdir /app
WORKDIR /app
COPY package.json package.json
COPY esy.lock esy.lock
RUN esy fetch
RUN esy true
RUN rm -rf /root/.esy/3/b /root/.esy/3/s
endef

define DOCKERFILE_APP
FROM $(1)
COPY --from=$(2) /root/.esy/ /root/.esy
RUN mkdir /app
WORKDIR /app
COPY . .
RUN esy fetch
endef

define ESYD
#!/bin/bash

APP_IMAGE_ID="$(1)"

docker run -v "$$PWD/.docker/store:/app/_esy/default/store" -it "$$APP_IMAGE_ID" esy "$$@"
endef

define USAGE
Welcome to esy-docker!

This is a set of make rules to produce docker images for esy projects.

You can execute the following targets:

  esy-docker-build    Builds an application
  esy-docker-shell    Builds an application and executes bash in a container

To make those targets available create a Makefile withe following lines:

  include /path/to/esy-docker.mk

endef
export USAGE

.DEFAULT: print-usage

print-usage:
	@$(call EMIT,$(USAGE))

.docker:
	@mkdir -p $(@)

.PHONY: .docker/Dockerfile.esy
.docker/Dockerfile.esy: .docker
	@$(call EMIT,$(DOCKERFILE_ESY)) > $(@)

.PHONY: Dockerfile.deps
.docker/Dockerfile.deps: .docker .docker/image.esy
	@$(call EMIT,$(call DOCKERFILE_DEPS,$(shell cat .docker/image.esy))) > $(@)

.PHONY: Dockerfile.app
.docker/Dockerfile.app: .docker .docker/image.deps
	@$(call EMIT,$(call DOCKERFILE_APP,$(shell cat .docker/image.esy),$(shell cat .docker/image.deps))) > $(@)

.dockerignore:
	@$(call EMIT,$(DOCKERIGNORE)) > $(@)

.docker/image.esy: .docker .dockerignore .docker/Dockerfile.esy
	@docker build . -f .docker/Dockerfile.esy --iidfile $(@)

.docker/image.deps: .docker .dockerignore .docker/Dockerfile.deps
	@docker build . -f .docker/Dockerfile.deps --iidfile $(@)

.docker/image.app: .docker .dockerignore .docker/Dockerfile.app
	@docker build . -f .docker/Dockerfile.app --iidfile $(@)

esyd: .docker/image.app
	@$(call EMIT,$(call ESYD,$(shell cat .docker/image.app))) > $(@)
	@chmod +x $(@)

esy-docker-shell-esy: .docker/image.esy
	@docker run -it $$(cat .docker/image.esy) /bin/bash

esy-docker-build: .docker/image.app
	@docker run -it $$(cat .docker/image.app) esy build

esy-docker-shell: .docker/image.app
	@docker run -it $$(cat .docker/image.app) /bin/bash

build: esy-docker-build
shell: esy-docker-shell
