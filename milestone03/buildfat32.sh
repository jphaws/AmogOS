#!/bin/bash

set -e

fat32="$1"
kernel="$2"
grub_cfg="$3"
arch="$4"

mkdir -p build/.img/boot/grub
cp "$kernel" build/.img/boot/kernel.bin
cp "$grub_cfg" build/.img/boot/grub

dd if=/dev/zero of="$fat32" bs=512 count=32768
parted "$fat32" mklabel msdos
parted "$fat32" mkpart primary fat32 2048s 30720s
parted "$fat32" set 1 boot on

sudo losetup /dev/loop0 "$fat32"
sudo losetup /dev/loop1 "$fat32" -o 1048576

sudo mkdosfs -F32 -f 2 /dev/loop1

sudo mkdir -p /mnt/fatgrub
sudo mount /dev/loop1 /mnt/fatgrub

sudo grub-install --target=i386-pc --root-directory=/mnt/fatgrub --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0
sudo cp -r build/.img/* /mnt/fatgrub

sudo umount /mnt/fatgrub
sudo losetup -d /dev/loop0
sudo losetup -d /dev/loop1
rm -r build/.img
sudo rm -r /mnt/fatgrub
