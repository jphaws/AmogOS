#include <stdint-gcc.h>
#include "commonio.h"
#include "inline.h"

#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)

#define PS2_CHANNEL_1 0xAD
#define PS2_CHANNEL_2 0xA7
#define PS2_CONTROLLER_CONFIG 0x20
#define PS2_DISABLE_CONFIG_MASK_OR 0x21
#define PS2_DISABLE_CONFIG_MASK_AND 0xED
#define PS2_TEST_FIRST_PORT 0xAB
#define PS2_ENABLE_FIRST_PORT 0xAE




uint8_t ps2_poll_read(uint16_t port){
   uint8_t status = inb(PS2_STATUS);

   while (!(status & PS2_STATUS_OUTPUT))
      status = inb(PS2_STATUS);

   // printk("   Status: 0x%x\n", status);

   return inb(port);
}


void ps2_poll_write(uint16_t port, uint8_t val){
   uint8_t status = inb(PS2_STATUS);

   while (status & PS2_STATUS_INPUT)
      status = inb(PS2_STATUS);

   // printk("   Status: 0x%x\n", status);
   outb(port, val);
}

static void disable_devices(){

   printk("Start devices disabled: \n");
   ps2_poll_write(PS2_CMD, PS2_CHANNEL_1);

   ps2_poll_write(PS2_CMD, PS2_CHANNEL_2);
}

static void flush_output_buffer(){
   uint8_t status = inb(PS2_DATA);

   printk("Start buffer flushed:\n");
   printk("   Status: 0x%x\n", status);
   while (status & PS2_STATUS_OUTPUT)
      status = inb(PS2_DATA);
}

static void set_configuration_byte(){
   uint8_t config = inb(PS2_CONTROLLER_CONFIG);

   printk("Start configuration byte:\n");
   printk("   Configuration byte received: 0x%x\n", config);

   config |= PS2_DISABLE_CONFIG_MASK_OR;
   config &= PS2_DISABLE_CONFIG_MASK_AND;

   ps2_poll_write(PS2_CMD, config);
   printk("   Configuration byte sent: 0x%x\n", config);

}

static void controller_self_test(){
   uint8_t test;

   printk("Start controller self test:\n");

   ps2_poll_write(PS2_CMD, 0xAA);

   printk("   Sent PS2 controller self test byte\n");

   test = ps2_poll_read(PS2_DATA);

   if (test != 0x55)
      printk("   PS2 Controller failed self test\n");

   printk("   Self test byte: 0x%x\n", test);

}

static void interface_tests(){
   uint8_t test;

   printk("Start interface test:\n");

   ps2_poll_write(PS2_CMD, PS2_TEST_FIRST_PORT);

   printk("   Test byte sent\n");

   test = ps2_poll_read(PS2_DATA);

   if (test != 0)
      printk("   Interface test failed\n");
   printk("   Interface test byte: 0x%x\n", test);
}

static void enable_devices(){
   printk("Begin enable PS2 Controller:\n");
   ps2_poll_write(PS2_CMD, PS2_ENABLE_FIRST_PORT);
}

void initialize_PS2_controller(){
   printk("Begin PS2 Controller Initialization\n");
   disable_devices();
   flush_output_buffer();
   set_configuration_byte();
   controller_self_test();
   interface_tests();
   enable_devices();
   printk("Finished PS2 Controller Initialization\n");
}
