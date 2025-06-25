# Stage 1: Base image for building and development setup
FROM gcc:11 AS base_env

ARG SRC_HOST_DIR=./src
ARG SRC_CONTAINER_DIR=/app/src
ARG BUILD_DIR=/app/build
ARG EXECUTABLE_NAME=main

WORKDIR /app
RUN mkdir -p ${BUILD_DIR}

COPY ${SRC_HOST_DIR}/ ${SRC_CONTAINER_DIR}/

# Stage 2: Builder - compiles the application
FROM base_env AS builder

RUN g++ ${SRC_CONTAINER_DIR}/${EXECUTABLE_NAME}.cpp -o ${BUILD_DIR}/${EXECUTABLE_NAME}

# Stage 3: Development - interactive environment with pre-built binaries
FROM base_env AS development

COPY --from=builder ${BUILD_DIR}/ ${BUILD_DIR}/

CMD ["bash"]

# Stage 4: Production/Runtime - smallest image for deployment
FROM alpine:latest AS production

RUN apk add --no-cache libstdc++

WORKDIR /app

COPY --from=builder ${BUILD_DIR}/ ${BUILD_DIR}/

ENTRYPOINT ["/app/build/main"]