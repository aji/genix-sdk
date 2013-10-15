NAME = coolgame
OBJECTS = main.o vdp.o
HEADERS = vdp.h

CROSS = m68k-unknown-elf-
LD = $(CROSS)ld
CC = $(CROSS)gcc
AS = $(CROSS)as
LDFLAGS =
CFLAGS = -g -O2 -m68000
ASFLAGS = -g

default: all

GXLDIR = gx-load
GXLOAD = $(GXLDIR)/gx-load
GXLINK = $(GXLDIR)/genix.ld
GXRT0 = $(GXLDIR)/gxrt0.o

$(GXLOAD):
	make -C $(GXLDIR)

all: $(NAME).rom

$(NAME).rom: $(GXLOAD) $(NAME).elf
	$(GXLOAD) < $(NAME).elf > $(NAME).rom
$(NAME).elf: $(GXLINK) $(GXRT0) $(OBJECTS)
	$(LD) $(LDFLAGS) -T $(GXLINK) -o $(NAME).elf $(GXRT0) $(OBJECTS)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS)
	rm -f $(NAME).rom
	rm -f $(NAME).elf
