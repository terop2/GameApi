FROM ubuntu:24.10
RUN apt-get update \
  && apt-get install -y -qq --no-install-recommends \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6 \
    libsdl2-2.0-0 \
    libfreetype6 \
    libsdl2-mixer-2.0-0 \
    libglew2.2 \
    curl \
    libc6 \
    libgcc-s1
# Dependencies for glvnd and X11.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute
COPY ./gameapi_cmdline ./
COPY ./libGameApi.so /usr/lib/
COPY ./script.txt ./