/* GNU LD Manual https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/ld.html */

ENTRY(isr_reset)

MEMORY
{
    flash (rx) : ORIGIN = 0x8000000, LENGTH = 512K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

_ram_end = ORIGIN(ram) + LENGTH(ram);

/* The Stack starts at the end of the RAM */
/* 0x20000000 + 96K => 0x20000000 + (96 * 1024) => 0x20018000 */
/* Verify with objdump --disassemble=ivt .build/firmware.elf that `ivt` is at 0x8000000 and first value (the Stack Pointer SP) is 0x20018000 */
_stack = _ram_end;

SECTIONS
{
    /* .text Section - contains the executable machine code of the program, including all compiled functions, instructions and IVT (interrupt vector table) */
    .text : ALIGN(4)
    {
        KEEP(*(.ivt));
        *(.text*)
        . = ALIGN(4);
    } > flash

    /* .data Section - contains initialized global and static variables that are loaded into RAM from read-only memory at program startup */
    .data : ALIGN(4)
    {
        _data_load = LOADADDR(.data); /* defines _data_load to load address of .data section from flash */
        _data_start = .;
        *(.data*)
        . = ALIGN(4);
        _data_end = .;
    } > ram AT > flash /* places .data section to ram at runtime (VMA => Virtual Memory Address) and loads it from flash (LMA => Load Memory Address)  */

    /* .bss Section - reserves space for uninitialized static and global variables that are zero-initialized at program startup. */
    .bss : ALIGN(4)
    {
        _bss_start = .;
        *(.bss*)
        . = ALIGN(4);
        _bss_end = .;
        PROVIDE(end = .);
    } > ram

    /* .init_array Section - init functions that are executed before main() */
    .init_array : ALIGN(4)
    {
        _init_array_start = .;
        KEEP(*(.init_array*))
        . = ALIGN(4);
        _init_array_end = .;
    } > flash
}
