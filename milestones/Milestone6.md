# Milestone 6

## Interrupt-Driven Serial Output

For this milestone you will add TX-only serial support. This is most useful for debugging because qemu provides you with a scroll buffer for serial output! The expectations for your serial driver are:

  - It is interrupt based. For the serial device in particular you need to read the IIR register, determine which type of event triggered the interrupt, and respond appropiately. In addition to handling the TX condition you need to handle and clear the LINE interrupt condition.
  - Your driver must use a producer-consumer design, where the SER_write() function is the producer and the interrupt handler is the consumer. Note that the SER_write() function needs to trigger writing the first byte if the shared buffer is empty.
  - Your driver must handle missed interrupts. To accomplish this you need to poll the serial device once every call to SER_write() to determine if the "busy" soft-state in the driver is accurate.
  - SER_write should be callable from either inside or outside an ISR.
  - An example (but not required) API is:

    ```C
    extern void SER_init(void);
    extern int SER_write(const char *buff, int len);
    ```

  - **Remember to be very careful about race conditions. Use CLI and STI correctly!**
  - Once SER_write is working update printk() so it prints to both the VGA console and the serial port.
  - To run your OS in qemu with a serial port, add a "-serial stdio" option to the command line:

    ```
    qemu-system-x86_64 -drive format=raw,file=fat.img -serial stdio
    ```


