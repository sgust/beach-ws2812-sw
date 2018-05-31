# Compiler & Linker
CC=arm-none-eabi-gcc

# Options for specific architecture
ARCH_FLAGS=-mthumb -mcpu=cortex-m3 -DSTM32F10X_MD=1

OBJ=startup_ARMCM3.o main.o systick.o ws2812b.o

CFLAGS=	-mthumb -mcpu=cortex-m3 -DSTM32F10X_MD=1 \
	-Wall -Os -std=c11 -flto -fno-builtin \
	-ffunction-sections -fdata-sections \
	-D__STARTUP_CLEAR_BSS -D__START=_start -D__NO_SYSTEM_INIT \
	-D__STACK_SIZE=0x1000

LFLAGS=--specs=nano.specs --specs=nosys.specs -L. -T nokeep.ld

DEPENDFLAGS=-Y.

beach-ws2812.hex: beach-ws2812.axf
	arm-none-eabi-objcopy $< -Oihex $@

beach-ws2812.axf: $(OBJ)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.S
	$(CC) $< $(CFLAGS) -c -o $@

clean: 
	rm -f *.o

distclean: clean
	rm -f beach-ws2812.axf beach-ws2812.hex

upload: beach-ws2812.hex
	stm32flash -v -w $< /dev/ttyUSB0

depend:
	makedepend $(DEPENDFLAGS) -- $(CFLAGS) -- $(SOURCES)

# DO NOT DELETE
