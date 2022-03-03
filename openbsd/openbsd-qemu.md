# Requirements

- qemu 6.1.0 compiled with riscv64-softmmu
- miniroot70.img from OpenBSD
- u-boot-qemu-riscv64-2021.07.txz package

# Build

Create virtual disk to install onto

    qemu-img create -f qcow2 opensbsd-riscv64-root.qcow2 10G

# Run qemu

- I to install
- vt220 terminal
- hostname netzero
- no network
- prax.hepazulian.net
- no DNS
- enable ssh
- no user
- allow root ssh
- sd1 root disk
- whole disk
- auto layout
- location disk
- disk not mounted
- 7.0/riscv
- a:
- all sets
- continue without verification
- Europe/London tz

# Use qemu-run

Configuration /locker/vm/netzero.yaml

- Change sshd to rootlogin without-password
- Create /etc/hostname.vio0
