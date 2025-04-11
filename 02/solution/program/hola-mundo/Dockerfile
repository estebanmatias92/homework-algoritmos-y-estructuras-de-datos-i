# Stage 1: Build stage 
FROM gcc:11 AS build
WORKDIR /app
COPY . .
RUN g++ -o ./build/main ./src/main.cpp

# Stage 2: Dev stage (for interactive development)
FROM gcc:11 AS dev
WORKDIR /app
# Copy only the source code (not the compiled binary)
COPY . .