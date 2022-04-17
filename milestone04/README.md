# Milestone 4

## Polling Keyboard Driver
The next step is to set up basic keyboard input. You will use the keyboard to provide test input data while you develop other parts of your kernel. This task is split into two pieces:

   - Configure the PS/2 controller. The controller is the built-in hardware device that talks to the keyboard. There are a few different types of controllers out. For this class you can assume a basic 8042 PS/2 controller. The PS/2 controller uses port-based I/O with addresses 0x60 (data) and 0x64 (command). The general flow of initilization is:
      1. Disable port 1 (see the command table in the previous link)
      2. Disable port 2
      3. Read byte 0 from RAM (current configuration)
      4. Modify the bits in the configuration so the first clock and first interrupt are enabled, but the second clk + interrupt are not.
      5. Write the configuration byte back to the controller. Note that you will need to poll the status bit to determine when it is safe to write the configuration byte.
   - Configure the keyboard. See the PS/2 Keyboard page for a listing of the various commands and scan codes associated with the keyboard. All data is sent to the keyboard by writing to the PS/2 controller's data port. The general initialization flow is:
      1. Reset the keyboard
      2. Set the keyboard to a known scan code.
      3. Enable the keyboard.
    Note that you may need to poll the status bit so you don't send data too quickly (keyboards are slow).
   - Read data and print it out. The "main loop" of your kernel should call a function to read one keystroke and then print it to the screen with printk. This will require you to poll the keyboard to determine when a byte is ready to read. It also requires converting scan codes into ASCII characters.

