FROM alpine:latest

RUN apk add --no-cache \
    build-base \
    cmake \
    llvm14-dev \
    llvm14-static \
    clang \
    clang-dev \
    libffi-dev \
    git \
    bash

WORKDIR /xypher

COPY . .

RUN ./build.sh

ENV PATH="/xypher/build/bin:${PATH}"
ENV LD_LIBRARY_PATH="/xypher/build/bin:${LD_LIBRARY_PATH}"

CMD ["xypc", "--help"]

