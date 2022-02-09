OUTPUT_ARCH(m68k)
SEARCH_DIR(.)
__DYNAMIC  =  0;

/*
 * The memory map look like this:
 * +--------------------+ <- 0x00000000
 * | .text              |
 * |        _etext      |
 * |        ctor list   | the ctor and dtor lists are for
 * |        dtor list   | C++ support
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * +--------------------+ <- 0x00FF0000
 * | .data              | initialized data goes here
 * |        _edata      |
 * +--------------------+
 * | .bss               |
 * |        __bss_start | start of bss, cleared by crt0
 * |        _end        | start of heap, used by sbrk()
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * |        __stack     | top of stack
 * +--------------------+ <- 0x01000000
 */

MEMORY
{
    rom (rx) : ORIGIN = 0x00000000, LENGTH = 0x0000FFFF
    ram (wx) : ORIGIN = 0x01000000, LENGTH = 0x00400000
}

/*
 * Allocate the stack to be at the top of memory, since the stack
 * grows down
 */

PROVIDE (__stack = 0x013F0000);

SECTIONS
{
  .text 0x00000000 :
  {
    *(.text .text.*)
    . = ALIGN(0x4);
    *(.rodata .rodata.*)
    *(.gcc_except_table) 

    . = ALIGN(0x2);
    __INIT_SECTION__ = . ;
    LONG (0x4E560000)	/* linkw %fp,#0 */
    *(.init)
    SHORT (0x4E5E)	/* unlk %fp */
    SHORT (0x4E75)	/* rts */

    __FINI_SECTION__ = . ;
    LONG (0x4E560000)	/* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4E5E)	/* unlk %fp */
    SHORT (0x4E75)	/* rts */

    . = ALIGN(2);
    _etext = .;
    *(.lit)
  } > rom
  _stext = SIZEOF (.text);

  .data 0x01000000 :
  AT ( ADDR (.text) + SIZEOF (.text) )
  {
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.*)
    . = ALIGN(2);
    _edata = .;
  } > ram
  _sdata = SIZEOF (.data);

  .bss 0x01000000 + SIZEOF (.data) :
  {
    . = ALIGN(0x4);
    __bss_start = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(COMMON)
    _end =  ALIGN (0x8);
    __end = _end;
    end = _end;
  } > ram

  PROVIDE(__HEAP_START = 0x01100000 );

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }

  .eh_frame 0 (NOLOAD) :
  {
    *(.eh_frame)
  }
}