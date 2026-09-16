# Codespaces / Dev Container image for zephyr-os-labs.
#
# This is a build+run environment for Zephyr's own generic QEMU boards
# (qemu_cortex_m0, qemu_x86, qemu_cortex_rt) - the same boards the labs
# already target in the school's classroom setup. It does NOT emulate the
# real target hardware (Seeeduino XIAO / XIAO ESP32) with hardware
# fidelity - GitHub Codespaces runs in the cloud and has no USB access, so
# it could never flash a physical board anyway. Building/running for the
# real boards, and flashing them, stays a local workflow with the board
# plugged in over USB.
#
# Kept deliberately small (Codespaces free-tier usage-hours are per
# core-hour): only the two toolchains these QEMU boards need, not the full
# multi-arch Zephyr SDK, and no separate hardware-specific QEMU fork.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
	git cmake ninja-build gperf ccache dfu-util device-tree-compiler wget \
	python3-dev python3-pip python3-venv python3-setuptools python3-tk python3-wheel \
	xz-utils file make gcc g++ gcc-multilib g++-multilib libsdl2-dev libmagic1 \
	curl ca-certificates sudo \
	qemu-system-arm qemu-system-x86 \
	&& rm -rf /var/lib/apt/lists/*

# ubuntu:24.04 already ships a "ubuntu" user at uid 1000 (matches the uid
# GitHub Codespaces' own default user uses).
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu
USER ubuntu
WORKDIR /home/ubuntu

# An isolated Python environment for west and Zephyr's own Python deps, at
# ~/zephyrproject/.venv - the same path the Zephyr Getting Started guide and
# this repo's own lab READMEs already use, so the documented `source
# ~/zephyrproject/.venv/bin/activate` step works unchanged in Codespaces too
# (even though it's a no-op there, since PATH already includes it below).
RUN mkdir -p /home/ubuntu/zephyrproject && python3 -m venv /home/ubuntu/zephyrproject/.venv
ENV PATH="/home/ubuntu/zephyrproject/.venv/bin:${PATH}"
RUN pip install --no-cache-dir west

# --- A ready-to-build Zephyr workspace, baked into the image ---
# Kept outside the repo's own mount point (the repo becomes the Codespace's
# workspaceFolder separately, at container start) so students never wait on
# `west update` themselves. Pinned to a specific release, not `main`, so
# the labs don't shift under students mid-semester.
ARG ZEPHYR_VERSION=v4.2.0
ENV ZEPHYR_BASE=/home/ubuntu/zephyrproject/zephyr

RUN git clone --depth 1 --branch ${ZEPHYR_VERSION} \
	https://github.com/zephyrproject-rtos/zephyr /home/ubuntu/zephyrproject/zephyr \
	&& cd /home/ubuntu/zephyrproject/zephyr && west init -l --mf west.yml . \
	&& cd /home/ubuntu/zephyrproject && west update -o=--depth=1 -n \
	&& west zephyr-export \
	&& pip install --no-cache-dir -r zephyr/scripts/requirements.txt \
	&& west packages pip --install

# --- Only the toolchains the labs' boards actually need ---
# (Not `west sdk install` with no --toolchains filter, which pulls every
# architecture Zephyr supports - dozens of GB we'd never use here.)
# arm-zephyr-eabi covers qemu_cortex_m0 and qemu_cortex_rt (both ARM);
# x86_64-zephyr-elf covers qemu_x86.
ENV ZEPHYR_SDK_INSTALL_DIR=/home/ubuntu/zephyr-sdk
RUN cd /home/ubuntu/zephyrproject/zephyr \
	&& west sdk install --toolchains arm-zephyr-eabi x86_64-zephyr-elf \
	--install-dir ${ZEPHYR_SDK_INSTALL_DIR}
