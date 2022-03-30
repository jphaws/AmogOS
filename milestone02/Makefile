arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/os-$(arch).iso
fat32 := build/os-$(arch).img
owner := $(shell stat -c '%U' .)
group := $(shell stat -c '%G' .)

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
								 build/arch/$(arch)/%.o, $(assembly_source_files))
.PHONY: all clean run iso

all: $(kernel)

clean: 
			@rm -r build 2> /dev/null || true

run-iso: $(iso)
			@qemu-system-x86_64 -cdrom $(iso)

# Requries root to build fat32
run-fat32: $(fat32)
			@qemu-system-x86_64 -s -drive format=raw,file=$(fat32) -serial stdio

fat32: $(fat32)

# Requries root to build -> run as root
$(fat32): $(kernel) $(grub_cfg)
			@mkdir -p build/.img/boot/grub
			@cp $(kernel) build/.img/boot/kernel.bin
			@cp $(grub_cfg) build/.img/boot/grub
			@dd if=/dev/zero of=$(fat32) bs=512 count=32768
			@parted $(fat32) mklabel msdos
			@parted $(fat32) mkpart primary fat32 2048s 30720s
			@parted $(fat32) set 1 boot on
			@losetup /dev/loop0 $(fat32)
			@losetup /dev/loop1 $(fat32) -o 1048576
			@mkdosfs -F32 -f 2 /dev/loop1
			@mkdir -p /mnt/fatgrub
			@mount /dev/loop1 /mnt/fatgrub
			@grub-install --target=i386-pc --root-directory=/mnt/fatgrub --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop0
			@cp -r build/.img/* /mnt/fatgrub
			@umount /mnt/fatgrub
			@losetup -d /dev/loop0
			@losetup -d /dev/loop1
			@rm -r build/.img
			@rm -r /mnt/fatgrub
			@chown -R $(owner):$(group) build

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
			@mkdir -p build/isofiles/boot/grub
			@cp $(kernel) build/isofiles/boot/kernel.bin
			@cp $(grub_cfg) build/isofiles/boot/grub
			@grub-mkrescue -o $(iso) build/isofiles #2> /dev/null
			@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(linker_script)
			@/home/jaxon/opt/cross/bin/x86_64-elf-ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
			@mkdir -p $(shell dirname $@)
			@nasm -felf64 $< -o $@
