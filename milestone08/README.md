# Milestone 8

## Virtual Page Allocator and Page Fault Handler
In this milestone you will add basic virtual address support to the kernel. This includes design of the address space, manipluating the CPU's multi-level page table, and minimially handling page faults.

   1. Design of your address space is a critical item that much happen prior to any coding. Without a clear design the chances of getting this to work correctly is a reasonable amount of time is low. Once you have designed your virtual address space draw a picture and keep it with you. It will help. The key design considerations are:
      - How do you access page frames? The kerenl occasionally needs to access ray page frames directly via physical address. How will this be handled? Some options are:
         - Keep a separate [Identity Page Table](http://wiki.osdev.org/Identity_Paging) for use with physical addresses (e.g., different address space).
         - Create an [Inverted Page](https://en.wikipedia.org/wiki/Page_table#Inverted_page_table) Table that maps a physical address to a virtual address. Every physical address needs to be mapped prior to using the point. This can get ugly, especially when memory references can cross page boundaries (e.g., memcpy).
         - For 64-bit address spaces only (e.g., what we are using in class with orders more virtual addresses than plausible physical memory), map the page frames at their associated phyiscal address in the virtual address space. This is similar to creating an identity mapping in the lower portion of the address space, leaving the remainder of the space for virtual addresses. 
      - Where and how large will the kernel stack(s) be? Your initial boot stack is too small to be used for deep nested function calls, so you will transition off that stack and into a kernel thread shortly. There may be more than one kernel thread.
      - Where will the kernel's heap be located? How much growth space do you need to provide before it runs in to one of the stacks or the user-level memory?
      - Where and how large will the kernel stacks be for your user-level threads. You system calls must run on their own stack for security, there each user-level threads has its own kernel stack.
      - Will the kernel virtual address space be mapped into every user-level process? (Hint: this simplifies a lot of things, I suggest it).
         - Which portion of the virtual address space is set aside for user processes?
         - Where in the address space will the user-level program text be loaded?
         - Where and how large will the user-level stack(s) be?
         - Where and how large will the heap be? The heap, and to a lesser extent the stacks, grow dynamically. Ensure there is plenty of room in the virtual address space for that to happen. 

   2. Once you have designed your virtual address space you need to create a few functions to manipulate the page table. You have some flexibility in the exact functions you pick, however you will code that can walk the multi-level page table, allocate new parts of the page table structure as needed, and finally return the address of an entry for a particular virtual address. Note that the page table structure itself uses physical addresses internally, not virtual addresses. One of the input parameters to your functions should be the a pointer to the page table structure to operate on. This will make allocation and modification of address spaces simpler when you have more than one virtual address space.

    After completing the page table helper function add functions that allocate and free virtual pages from the kernel heap. You are free to name these function as you see fit. Sample function declarations are:

   ```C
   extern void *MMU_alloc_page();
   extern void *MMU_alloc_pages(int num);
   extern void MMU_free_page(void *);
   extern void MMU_free_pages(void *, int num);
   ```

    Remember that all these functions deal in virtual addresses.

   3. Implement a page fault handler. You have one of two options here. The first option is less work. The second is slightly more work but will result in a substantial reduction in memory use from your OS:
      1. Print out an error message and HLT for every unhandled page fault. The error message must include the address that caused the fault (stored in CR2), the page table that is in use (store in CR3), and the fault code (passed in as a parameter to the ISR). This will really help with debugging memory problems.
      2. Implement a form of [demand paging](https://en.wikipedia.org/wiki/Demand_paging) for page frames. This waits until the page frame is truly needed before allocating it and results in substantially fewer wasted page frames. The steps involved are:
         - Do not actually allocate a page frame in your MMU_alloc_pages function. Just set up the page table entry. Use one of the available bits in the page table entry to indicate you want the fault handler to allocate the page on demand. This bit is important to prevent double-allocations and other unexpect page fault resolutions.
         - Your fault handler looks up the page table entry and checks the demand bit. If the demand bit is set:
            1. Call MMU_pf_alloc to find an empty page frame
            2. Clears the demand bit in the page table entry
            3. Updates the physical base field in the page table entry to refer to the new page frame 
         - If the demand bit isn't set, or the page table entry is invalid, or any other problem occur, print out the error message described in step (1) above 

Note: You may find it is easier to completely replace the page table you created in assembly during boot as part of initializing your memory management subsystem. 

