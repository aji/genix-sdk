NAME = coolgame
OBJECTS = main.o vdp.o log.o
HEADERS = vdp.h

CROSS = m68k-unknown-elf-
LD = $(CROSS)ld
CC = $(CROSS)gcc
AS = $(CROSS)as
LDFLAGS =
CFLAGS = -g -O2 -m68000
ASFLAGS = -g

# Uncomment this if you're running in a gx-log patched Gens 2.15.5 and
# wish to view logging output (this will probably just screw with the
# YM2612 in weird ways if you run in an unpatched Gens)
# CFLAGS += -DGX_LOGGING

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
