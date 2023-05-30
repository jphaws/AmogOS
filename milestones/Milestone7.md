# Milestone 7

## Parse Multiboot Tags and Page Frame Allocator

Multiboot2 passes information about the underlying computer using tags. These tags contain two pieces of valuable information:

   - The region(s) of physical memory present in the machine.
   - The memory locations occupied by your kernel's text and data segments. 

The objective of this lab is to parse the tags, extract this information, and use it to write a page frame allocator.

The page frame allocator is the most basic building block of a kernel memory management system. It's sole responsibility is the find an unused page frame (physical address) and return it. It uses two sources for frames:

   - The page frames as described by the multiboot MMAP tag, minus the frames used by the kerenl as described in the ELF section header.
   - Page frames that have been returned to the free pool. 

The free page frame pool is typically structured as a linked list using the first few bytes of the unused page frame to store the necessary pointers.

How you store the MMAP tags is up to you. However I strongly discourage you from adding all the page frames discovered from MMAP into the free list. This results in a very slow system startup time.

This step is a precursor to implementing memory management, as any memory manager needs to know how much physical RAM is available, where it is located, and which pieces are already used.

You will need two main references for this task. The first is the [multiboot specification.](http://nongnu.askapache.com/grub/phcoder/multiboot.pdf) The specification lists the exact structure of the tags and details how they tags are passed into the OS. From the tags you need to extract the list of physical memory regions and the location of the ELF section headers. You will need to store the memory regions in an efficient data structure for use by your memory manager.

The second reference you need is the [ELF format description.](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) This tells you the format of the ELF section header. The ELF section header contains information on which parts of memory your kernel is currently using. It is very important to avoid avoid allocating this kernel memory to any other kernel tasks or user level processes.

A more detailed list of steps for this milestone is:

   - Ensure the value in %EBX when the protected mode code begins is passed into your kmain function. This value is a pointer to the tag structure.
   - Use the tag pointer to read through all tag entries
   - Process all MMAP tags by adding the described regions to a data structure in your memory management code.
   - Process the ELF tag
      - Loop through each entry in the section header
      - For each load entry determine the start / end addresses of the destination and exclude from the memory manager. 
   - Implement the following two (or similar) functions to allocate and free a page frame:

   ```C
   extern void *MMU_pf_alloc(void);
   extern void MMU_pf_free(void *pf);
   ```

### Testing
Test your page allocator by allocating and freeing a few pages in a well defined sequence. Print out the page addresses as they are allocated / freed. Ensure your freed pages are reused as appropriate.

Stress test your page allocator by allocating all pages in the system one at a time. Write a bit pattern to all 4K in each page. The bit pattern should be different for each page (e.g., use the page address as the bit pattern, repeated as necessary to fill the page). After writing the bit pattern read and verify the entire pattern. Only produce output if there is an error.

I expect to see both of these as part of your demo.
