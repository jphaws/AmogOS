# Milestone 2
## Boot your OS in long mode
The objective here is to get your OS to boot in protected mode with GRUB and multiboot, then switch to long mode. Proof is outputting a message to the VGA console. Ensure you are up-to-date on all class readings. The following two tutorials will guide you through accomplishing this milestone:

 1. A minimal x86 kernel - Takes you through a boot overview view multiboot, demonstrates the special steps needs to compile your kernel, packages your kernel into an ISO image, boots it, and then helps you write a makefile to streamline the process.
    - Note 1 - Don't skip the makefile! It will be invaluable all quarter.
    - Note 2 - Consider using a FAT32 disk image instead of ISO. This will be necessary later in the quarter when you implement FAT32 support for your OS.
    - Note 3 - The tutorial is focused on Rust, but this portion is language agnostic.
 2. Entering Long Mode - Takes you through transitioning the CPU into long mode, including creation of a starting stack, page table, and global descriptor table.
    - Note 1 - The tutorial is focused on Rust, but this portion is language agnostic.

### Testing
The demo of this milestone is straightforward. I'm looking for the hard-coded "OK" output (or similar) to the VGA console. I'll also briefly review your assembly to ensure you are actually in long mode.
