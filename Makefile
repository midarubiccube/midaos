SHELL=/bin/bash
TARGET = kernel.elf
OBJS = acpi/acpi.o acpi/pm_timer.o acpi/registers.o \
       apic/apic.o apic/apictimer.o apic/ioapic.o \
			 ata/fis.o ata/ahci.o ata/port.o ata/registers.o ata/SATAController.o \
			 interrupt/interrupt.o \
			 serial/serial.o \
       usb/memory.o usb/device.o usb/xhci/ring.o usb/xhci/trb.o usb/xhci/xhci.o \
       usb/xhci/port.o usb/xhci/device.o usb/xhci/devmgr.o usb/xhci/registers.o \
       usb/classdriver/base.o usb/classdriver/hid.o usb/classdriver/keyboard.o \
       usb/classdriver/mouse.o \
			 pci/pci.o \
			 x64/idt.o x64/segment.o x64/paging.o x64/cpuid.o \
			 main.o graphics.o mouse.o font.o hankaku.o newlib_support.o console.o \
       asmfunc.o libcxx_support.o logger.o  memory_manager.o window.o \
			 layer.o timer.o frame_buffer.o keyboard.o task.o terminal.o 

DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

CPPFLAGS += -I.
CFLAGS   += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone
CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone \
            -fno-exceptions -fno-rtti -std=c++17
LDFLAGS  += --entry KernelMain -z norelro --image-base 0x100000 --static


.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

.PHONY: install
install: $(TARGET)
	sudo mkdir -p /mnt/usbmem
	sudo mount -t drvfs F: /mnt/usbmem
	sudo cp kernel.elf /mnt/usbmem/
	sudo umount /mnt/usbmem
	sudo rm -r /mnt/usbmem

.PHONY: run
run: $(TARGET) 
	/home/mida/osbook/devenv/run_qemu.sh /home/mida/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi /home/mida/workspace/kernel/kernel.elf

.PHONY: img
img: $(TARGET)
	/home/mida/osbook/devenv/make_image.sh disk.iso mnt /home/mida/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi /home/mida/workspace/kernel/kernel.elf


kernel.elf: $(OBJS)  Makefile
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS) -lc -lc++ -lc++abi

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.%.d: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.c Makefile
	clang $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.%.d: %.c
	clang $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.asm Makefile
	nasm -f elf64 -o $@ $<

hankaku.bin: hankaku.txt
	../tools/makefont.py -o $@ $<

hankaku.o: hankaku.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

.%.d: %.bin
	touch $@

.PHONY: depends
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)
