# Milestone 5

## Handle Interrupts
In this milestone your OS will be modified to handle interrupts. This is critical because many of the other hardware devices you will use later depend on interrupts. It is also the only way to get notified when the CPU triggers an error conditions (fault). This top level steps and order in which your should implement + test them are:

    1.  Incorporate a generic interrupt management layer into your OS.
      - Your assembly ISR should be set up once during kernel initialization.
      - Provide a C interface for setting / clearing IRQs at any point. You are not required to exactly implement this interface, however one example interface is:

      ```C
        extern void IRQ_init(void);
        extern void IRQ_set_mask(int irq);
        extern void IRQ_clear_mask(int irq);
        extern int IRQ_get_mask(int IRQline);
        extern void IRQ_end_of_interrupt(int irq);

        typedef void (*irq_handler_t)(int, int, void*);
        extern void IRQ_set_handler(int irq, irq_handler_t handler, void *arg);
        ```

        Make sure you define macros or functions that wrap the CLI and STI instructions.
    2. Install and initialize an IDT such that all interrupts get directed to your interrupt management code.
        Remember to save all the volatile registers in your assembly ISR prior to calling your C handler code. Per calling conventions the C code will not save those registers.
        Refer to the [calling conventions](https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64) to determine how parameters are passed into the C function.
        I strongly encourage you to use interrupt gates so interrupts are automatically disabled while your ISR is running. However you are free to use trap gates. Just make an explicit decision and understand how it impacts the rest of your interrupt handling code.
    3. Register a keyboard interrupt and use it to test your interrupt management code.
        Look out for race conditions with your VGA code that occur when the VGA code is executing, an interrupt occurs, and the VGA code is called inside the ISR. For example, the global current cursor position will be a problem. Code that uses/updates it, such as printing a single character or scrolling the buffer, will need to be protected with CLI and STI.
        At this point you can remove the keyboard polling loop added in a previous step.
    4. Make sure you print an error message for any unhandled interrupts. You should also halt the CPU to prevent it from executing subsequent instructions.
    5. Set up and configure a TSS so that #GP, #DF, and #PF all have their own interrupt stacks.

**Remember to be very careful about race conditions. Use CLI and STI correctly!**

Refer to the Lecture Notes from 02-2, the Lecture Notes from 03-1, and the assigned reading for the same days for more details on setting up interrupts.

