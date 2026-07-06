CC      = arm-none-eabi-gcc
TARGET  = adas_rtos
QEMU    = qemu-system-arm

CFLAGS  = -mcpu=cortex-m3 \
           -mthumb \
           -O1 \
           -ffreestanding \
           -nostdlib \
           -Wall \
           -g

LDFLAGS = -T linker.ld \
           -nostdlib \
           -nostartfiles \
           -Wl,--gc-sections

SRCS    = startup.s \
           main.c \
           hal/systick.c \
           kernel/kernel.c \
           kernel/queue.c \
           kernel/deadline.c \
           kernel/logger.c \
           kernel/watchdog.c \
           tasks/braking.c \
           tasks/radar.c \
           tasks/camera.c \
           tasks/dashboard.c

OBJS    = $(SRCS:.c=.o)
OBJS   := $(OBJS:.s=.o)

all: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $

qemu: $(TARGET).elf
	@echo "Starting QEMU — Press Ctrl+A then X to exit"
	$(QEMU) \
		-machine lm3s6965evb \
		-nographic \
		-serial mon:stdio \
		-kernel $(TARGET).elf

clean:
	find . -name "*.o" -delete
	rm -f $(TARGET).elf

.PHONY: all qemu clean