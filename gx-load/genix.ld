OUTPUT_FORMAT("elf32-m68k")
ENTRY(_start)

SECTIONS {
  .rom 0x0 : {
    *(.vectbl);

    . = 0x200;
    *(.text .rodata);
  }

  .data 0xff0000 : AT(SIZEOF(.rom)) { *(.data); }
  .bss (0xff0000 + SIZEOF(.data)) : { *(.bss COMMON); }

  __data_load_beg = LOADADDR(.data);
  __data_load_end = LOADADDR(.data) + SIZEOF(.data);
  __data_rel = ADDR(.data);

  __bss_beg = ADDR(.bss);
  __bss_end = ADDR(.bss) + SIZEOF(.bss);
}
