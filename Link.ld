FLASH_START = 0x00000000;
DATA_FLASH_START = 0x40100000;
ID_CODE_START = 0x01010018;

/* Define memory regions. */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 0x10000
  RAM (rwx)       : ORIGIN = 0x20004000, LENGTH = 0x4000
  DATA_FLASH (rx) : ORIGIN = 0x40100000, LENGTH = 0x1000
  ID_CODE (rx)    : ORIGIN = 0x01010018, LENGTH = 0x20
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be DEFINED in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        __tz_FLASH_S = ABSOLUTE(FLASH_START);
        __ROM_Start = .;

        /* Even though the vector table is not 256 entries (1KB) long, we still allocate that much
         * space because ROM registers are at address 0x400 and there is very little space
         * in between. */
        KEEP(*(.fixed_vectors*))
        KEEP(*(.application_vectors*))
        __Vectors_End = .;

        /* ROM Registers start at address 0x00000400 for devices that do not have the OPTION_SETTING region. */
        . = __ROM_Start + 0x400;
        KEEP(*(.rom_registers*))

        /* Reserving 0x100 bytes of space for ROM registers. */
        . = __ROM_Start + 0x500;
        
        /* Allocate flash write-boundary-aligned 
         * space for sce9 wrapped public keys for mcuboot if the module is used.
         */
		. = ALIGN(128);
        
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        *(.rodata*)
        
        /*rt-thead FinSH*/
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /*rt-thead auto init*/
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        __ROM_End = .;
    } > FLASH = 0xFF

    __Vectors_Size = __Vectors_End - __Vectors;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    __etext = .;

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
        . = ALIGN(4);

        /* Don't use *(.data*) because it will place data meant for .data_flash in this section. */
        *(.data.*)
        *(.data)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);

        /* All data end */
        __data_end__ = .;

    } > RAM AT > FLASH


    /* TrustZone Secure Gateway Stubs Section. */
    .gnu.sgstubs : ALIGN (1024)
    {
        . = ALIGN(1024);
        __tz_FLASH_C = ALIGN(1024);
        _start_sg = .;
        *(.gnu.sgstubs*)
        . = ALIGN(32);
        _end_sg = .;
    } > FLASH

    __tz_FLASH_N = ALIGN(32768);
    FLASH_NS_IMAGE_START = __tz_FLASH_N;

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        __noinit_end = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM

    /* Stacks are stored in this section. */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(8);
        __StackLimit = .;
        /* Main stack */
        KEEP(*(.stack))
        __StackTop = .;
        /* Thread stacks */
        KEEP(*(.stack*))
        __StackTopAll = .;
    } > RAM

    PROVIDE(__stack = __StackTopAll);

    /* This symbol represents the end of user allocated RAM. The RAM after this symbol can be used
       at run time for things such as ThreadX memory pool allocations. */
    __RAM_segment_used_end__ = ALIGN(__StackTopAll , 4);

    /* Data flash. */
    .data_flash :
    {
        . = ORIGIN(DATA_FLASH);
        __tz_DATA_FLASH_S = .;
        __Data_Flash_Start = .;
        KEEP(*(.data_flash*))
        __Data_Flash_End = .;

        __tz_DATA_FLASH_N = ABSOLUTE(0x40100000 + 0x1000);
    } > DATA_FLASH

    /* Note: There are no secure/non-secure boundaries for ID_CODE.  These symbols are provided for the RA configuration tool. */
    __tz_ID_CODE_S = ORIGIN(ID_CODE);

    .id_code :
    {
        __ID_Code_Start = .;
        KEEP(*(.id_code*))
        __ID_Code_End = .;
    } > ID_CODE

    /* Note: There are no secure/non-secure boundaries for ID_CODE.  These symbols are provided for the RA configuration tool. */
    __tz_ID_CODE_N = __ID_Code_End;

}
