/* memory map */
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 3840K
    RAM (xrw)       : ORIGIN = 0x24000000, LENGTH = 832K
    ARM.__at_0xC1000000(xrw) : ORIGIN = 0xC1000000, LENGTH = 512K
    ARM.__at_0xC1080000(xrw) : ORIGIN = 0xC1080000, LENGTH = 512K
    DTCM(rw) : ORIGIN = 0x20000000, LENGTH = 128K
    ITCM(x) : ORIGIN = 0x00000000, LENGTH = 64K
}

ENTRY(Reset_Handler)

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 32K;
  
/* ISR vectors */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors))
    . = ALIGN(4);
    __Vectors_End = .;
    __Vectors_Size = __Vectors_End - __gVectors;
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7) 
    *(.glue_7t)
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .ARM.extab :
  { 
     *(.ARM.extab* .gnu.linkonce.armextab.*) 
  } >FLASH
  
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .ARM.attributes : { *(.ARM.attributes) } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  .stack ORIGIN(RAM):
  {
    PROVIDE( _heap_end = . ); 
    . = __stack_size;  
    PROVIDE( _sp = . ); 
  } >RAM

  /* provide some necessary symbols for startup file to initialize data */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    /* the symbol ��_sdata�� will be defined at the data section end start */
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    /* the symbol ��_edata�� will be defined at the data section end */
    _edata = .;
  } >RAM AT> FLASH

  . = ALIGN(4);
  .bss (NOLOAD):
  {
    /* the symbol ��_sbss�� will be defined at the bss section start */
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    /* the symbol ��_ebss�� will be defined at the bss section end */
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  . = ALIGN(8);
  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );


  
  .ARM.__at_0xC1000000 (NOLOAD) : ALIGN(4)
  {
    . = ALIGN(4);
    *(.ARM.__at_0xC1000000)
    *(.ARM.__at_0xC1000000.*)
    . = ALIGN(4);
    __ARM.__at_0xC1000000_free__ = .;
  } > ARM.__at_0xC1000000

  .ARM.__at_0xC1080000 (NOLOAD) : ALIGN(4)
  {
    . = ALIGN(4);
    *(.ARM.__at_0xC1080000)
    *(.ARM.__at_0xC1080000.*)
    . = ALIGN(4);
    __ARM.__at_0xC1080000_free__ = .;
  } > ARM.__at_0xC1080000

  .DTCM (NOLOAD) : ALIGN(4)
  {
    . = ALIGN(4);
    *(.DTCM)
    *(.DTCM.*)
    . = ALIGN(4);
    __DTCM_free__ = .;
  } > DTCM


}

 /* input sections */
GROUP(libgcc.a libc.a libm.a libnosys.a)
