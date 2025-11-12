FROM ubuntu:25.10
# Dependencies for glvnd and X11.
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
    libgcc-s1 \
    unzip \
    zip \
    wget \
    ca-certificates \
    ffmpeg \
    libsm6 \
    libxrender-dev \
    fonts-liberation \
    libatk1.0-0 \
    libcairo2 \
    libcups2 \
    libgtk-3-0 \
    libnspr4 \
    libnss3 \
    libpango-1.0-0 \
    libxcomposite1 \
    libxdamage1 \
    xdg-utils \
    libu2f-udev \
    libopencv-dev \
  && rm -rf /var/lib/apt/lists/*
#RUN apt-get install -y wget
RUN wget --no-check-certificate https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
RUN apt-get install -y ./google-chrome-stable_current_amd64.deb
RUN rm google-chrome-stable_current_amd64.deb
# Env vars for the nvidia-container-runtime.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute
ENV BUILDER_DOCKER_DIR /app/
COPY ./gameapi-builder_1.0-28.deb ./
RUN dpkg -i /gameapi-builder_1.0-28.deb
COPY ./chromium ./
ADD chromium /usr/share/chromium
