ENTRY(entry)
SECTIONS
{
  . = 0x1000000;
  .text : { *(.entry) }
  .text : { *(.text) }

  . = 0x4000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
