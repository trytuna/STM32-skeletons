MEMORY
{
    flash (rx) : ORIGIN = 0x8000000, LENGTH = 256K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 40K
}

_ram_end = ORIGIN(ram) + LENGTH(ram);

/* The Stack starts at the end of the RAM */
/* 0x20000000 + 40K => 0x20000000 + (40 * 1024) => 0x2000a000 */
/* Verify with objdump -D firmware.elf that `ivt` is at 0x8000000 and first value (the Stack Pointer SP) is 0x2000a000 */
_stack = _ram_end; 

SECTIONS
{
    .text : ALIGN(4)
    {
        KEEP(*(.ivt));
        *(.text*);
        . = ALIGN(4);
    } > flash
}
