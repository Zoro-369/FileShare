# Use a lightweight Debian base image with g++
FROM debian:sid


# Set the working directory
WORKDIR /app

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    iproute2 \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Copy the source code
COPY . .

# Create a build directory
# RUN mkdir -p build

# Compile the project
# RUN make

# Expose the discovery port (UDP)
EXPOSE 9999/udp

# Allow passing CLI arguments
CMD ["/bin/bash"]
