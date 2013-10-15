#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <endian.h>
#include <stddef.h>
#include <getopt.h>

static void *debug(const char *msg, ...)
{
	char buf[2048];
	va_list va;

	va_start(va, msg);
	vsnprintf(buf, 2048, msg, va);
	va_end(va);

	fprintf(stderr, "gx-load: %s\n", buf);
}

static ssize_t qread(int fd, void *_buf, size_t nbyte, off_t offset)
{
	uint8_t *buf = _buf;
	ssize_t r, sz;

	for (r=0; r<nbyte; ) {
		sz = pread(fd, buf+r, nbyte-r, offset+r);
		if (sz < 0) {
			debug("pread() error");
			abort();
		}
		if (sz == 0)
			break;
		r += sz;
	}

	return sz;
}

static uint32_t qread_32_le(int fd, off_t offset)
{
	uint32_t v;
	if (qread(fd, &v, sizeof(v), offset) != sizeof(v)) {
		debug("qread() failed");
		abort();
	}
	return le32toh(v);
}

static uint16_t qread_16_le(int fd, off_t offset)
{
	uint16_t v;
	if (qread(fd, &v, sizeof(v), offset) != sizeof(v)) {
		debug("qread() failed");
		abort();
	}
	return le16toh(v);
}

static uint32_t qread_32_be(int fd, off_t offset)
{
	uint32_t v;
	if (qread(fd, &v, sizeof(v), offset) != sizeof(v)) {
		debug("qread() failed");
		abort();
	}
	return be32toh(v);
}

static uint16_t qread_16_be(int fd, off_t offset)
{
	uint16_t v;
	if (qread(fd, &v, sizeof(v), offset) != sizeof(v)) {
		debug("qread() failed");
		abort();
	}
	return be16toh(v);
}

static uint8_t qread_8(int fd, off_t offset)
{
	uint8_t v;
	if (qread(fd, &v, sizeof(v), offset) != sizeof(v)) {
		debug("qread() failed");
		abort();
	}
	return v;
}

static bool qread_le;

static uint32_t qread_32(int fd, off_t offset)
{
	if (qread_le)
		return qread_32_le(fd, offset);
	return qread_32_be(fd, offset);
}

static uint16_t qread_16(int fd, off_t offset)
{
	if (qread_le)
		return qread_16_le(fd, offset);
	return qread_16_be(fd, offset);
}

/*
 * ROM
 */

#define ROM_MAX_SIZE 0x400000

static uint8_t rom_data[ROM_MAX_SIZE];
static size_t rom_size = 0x200;

static size_t rom_field_p;

static uint32_t rom_entry = 0x200;
static char rom_firm[16] = "AJITEK 2013";
static char rom_name_dom[48] = "GENIX GX-LOAD";
static char rom_name_int[48] = "GENIX GX-LOAD";

static void rom_field_s(int sz, char pad, char *s)
{
	size_t i;

	i = rom_field_p;
	rom_field_p += sz;
	for (; i<rom_field_p; i++)
		rom_data[i] = *s ? (*s++) : pad;
}

static void rom_field_w(uint16_t v)
{
	v = htobe16(v);
	memcpy(rom_data + rom_field_p, &v, 2);
	rom_field_p += 2;
}

static void rom_field_l(uint32_t v)
{
	v = htobe32(v);
	memcpy(rom_data + rom_field_p, &v, 4);
	rom_field_p += 4;
}

static void rom_fields(void)
{
	rom_field_p = 0x0;

	debug("  Vector 0: 0x0");
	debug("  Vector 1: 0x%x", rom_entry);

	rom_field_l(0);
	rom_field_l(rom_entry);

	rom_field_p = 0x100;

	debug("  Firm: %.16s", rom_firm);
	debug("  Dom. Name: %.48s", rom_name_dom);
	debug("  Int. Name: %.48s", rom_name_int);

	rom_field_s(16, ' ', "SEGA GENESIS");
	rom_field_s(16, ' ', rom_firm);
	rom_field_s(48, ' ', rom_name_dom);
	rom_field_s(48, ' ', rom_name_int);
	rom_field_s(14, ' ', "GM 99999999-99");
	rom_field_w(0);
	rom_field_s(16, ' ', "J");
	rom_field_l(0);
	rom_field_l(rom_size);
	rom_field_l(0xff0000);
	rom_field_l(0xffffff);
	rom_field_s(4, ' ', "");
	rom_field_s(4, ' ', "");
	rom_field_s(4, ' ', "");
	rom_field_s(12, ' ', "");
	rom_field_s(40, ' ', "");
	rom_field_s(16, ' ', "JUE");
}

static void rom_emit(void)
{
	ssize_t wr, sz;

	if (rom_size > ROM_MAX_SIZE) {
		debug("Warning: rom_size is larger than ROM_MAX_SIZE!");
		rom_size = ROM_MAX_SIZE;
	}

	for (wr=0; wr<rom_size; ) {
		sz = write(1, rom_data + wr, rom_size - wr);
		if (sz <= 0) {
			debug("Error on write()");
			abort();
		}
		wr += sz;
	}
}

/*
 * ELF
 */

/* TODO: write my own definitions as needed */
#include <linux/elf.h>

static Elf32_Ehdr elf_hdr;

static int elf_load_ph(uint32_t at)
{
	Elf32_Phdr ph;
	char *type, rwx[4];
	size_t end;

	if (qread(0, &ph, sizeof(ph), at) != sizeof(ph)) {
		debug("qread() failed");
		return -1;
	}

#define swap_field(N) N = qread_le ? le32toh(N) : be32toh(N)
	swap_field(ph.p_type);
	swap_field(ph.p_offset);
	swap_field(ph.p_vaddr);
	swap_field(ph.p_paddr);
	swap_field(ph.p_filesz);
	swap_field(ph.p_memsz);
	swap_field(ph.p_flags);
	swap_field(ph.p_align);
#undef swap_field

	switch (ph.p_type) {
	case PT_NULL:    type = "  NULL:"; break;
	case PT_LOAD:    type = "  LOAD:"; break;
	case PT_DYNAMIC: type = "   DYN:"; break;
	case PT_INTERP:  type = "INTERP:"; break;
	case PT_NOTE:    type = "  NOTE:"; break;
	case PT_SHLIB:   type = " SHLIB:"; break;
	case PT_PHDR:    type = "  PHDR:"; break;
	default:         type = "  ????:";
	}

	rwx[0] = (ph.p_flags & PF_R) ? 'r' : '-';
	rwx[1] = (ph.p_flags & PF_W) ? 'w' : '-';
	rwx[2] = (ph.p_flags & PF_X) ? 'x' : '-';
	rwx[3] = '\0';

	/* we load at paddr, as experimentation suggests this is used to
	   mean the load address */

	debug("  %s %s 0x%08x+%08x => 0x%08x+%08x (rel %08x)", type, rwx,
	      ph.p_offset, ph.p_filesz,
	      ph.p_paddr, ph.p_memsz, ph.p_vaddr);

	if (ph.p_type != PT_LOAD)
		return 0;

	if (ph.p_paddr + ph.p_memsz > ROM_MAX_SIZE) {
		if (ph.p_filesz != 0) {
			debug("Non-empty segment to be loaded beyond ROM end");
			return -1;
		}
		return 0;
	}

	if (ph.p_flags & PF_W) {
		debug("Warning: segment is writable. "
		      "Program may not run as intended");
	}

	if (ph.p_filesz == 0)
		return 0;

	memset(rom_data + ph.p_paddr, 0, ph.p_memsz);
	if (qread(0, rom_data + ph.p_paddr, ph.p_filesz, ph.p_offset)
	    != ph.p_filesz) {
		debug("qread() failed");
		return -1;
	}

	if (ph.p_paddr + ph.p_memsz > rom_size)
		rom_size = ph.p_paddr + ph.p_memsz;

	return 0;
}

static int elf_parse(void)
{
	int i;
	size_t at;

	/* ident */
	if (qread_32_be(0, 0) != 0x7f454c46) {
		debug("This does not look like an ELF file");
		return -1;
	}
	if (qread_8(0, EI_CLASS) != ELFCLASS32) {
		debug("gx-load can only read 32 bit ELF");
		return -1;
	}

	switch (qread_8(0, EI_DATA)) {
	case ELFDATA2LSB: qread_le = true; break;
	case ELFDATA2MSB: qread_le = false; break;
	default:
		debug("Unknown ELF data type; cannot continue");
		return -1;
	}

	debug("Loading ELF header fields");
#define read_field(S, N) elf_hdr.N = qread_##S(0, offsetof(Elf32_Ehdr, N))
	read_field(16, e_type);
	read_field(16, e_machine);
	read_field(32, e_version);
	read_field(32, e_entry);
	read_field(32, e_phoff);
	read_field(32, e_shoff);
	read_field(32, e_flags);
	read_field(16, e_ehsize);
	read_field(16, e_phentsize);
	read_field(16, e_phnum);
	read_field(16, e_shentsize);
	read_field(16, e_shnum);
	read_field(16, e_shstrndx);
#undef read_field

	if (elf_hdr.e_type != ET_EXEC)
		debug("Warning: ELF input not of executable type");
	if (elf_hdr.e_machine != EM_68K)
		debug("Warning: ELF input not for Motorola 68000");

	if (elf_hdr.e_phnum == 0) {
		debug("ELF input has 0 program headers. Is this a joke?");
		return -1;
	}

	debug("Setting ROM entry point");
	rom_entry = elf_hdr.e_entry;

	debug("Loading program segments:");
	at = elf_hdr.e_phoff;
	for (i=0; i < elf_hdr.e_phnum; i++, at += elf_hdr.e_phentsize) {
		if (elf_load_ph(at) < 0) {
			debug("Failed to load program header %d, at 0x%x",
			      i, at);
			return -1;
		}
	}
	debug("Program segments loaded. ELF parsing finished.");

	return 0;
}

/*
 * entry
 */

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s OPTIONS < in.elf > out.bin\n", argv0);
	fprintf(stderr, "\nOPTIONS:\n");
	fprintf(stderr, "  -p NAME     Publisher/firm name\n");
	fprintf(stderr, "  -n NAME     The name of the ROM\n");
	fprintf(stderr, "  -h          Show this usage\n");
}

int main(int argc, char *argv[])
{
	int c;

	if (isatty(0) || isatty(1)) {
		usage(argv[0]);
		return 1;
	}

	while ((c = getopt(argc, argv, "p:n:h")) != -1) switch (c) {
	case 'p':
		strncpy(rom_firm, optarg, 16);
		break;

	case 'n':
		strncpy(rom_name_dom, optarg, 48);
		strncpy(rom_name_int, optarg, 48);
		break;

	case 'h':
		usage(argv[0]);
		return 0;
	default:
		usage(argv[0]);
		return 1;
	}

	debug("Parsing ELF input...");
	if (elf_parse() < 0) {
		debug("ELF parsing failed. No ROM output produced");
		return 1;
	}

	debug("Filling out ROM fields...");
	rom_fields();

	debug("Writing ROM");
	rom_emit();

	return 0;
}
