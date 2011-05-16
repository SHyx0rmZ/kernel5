OUTPUT_ARCH("i386:x86-64")
OUTPUT_FORMAT("binary")

SECTIONS
{
    .smpinit 0x8000 : AT(0x8000)
    {
        *(.smpinit)
        *(.text)
        *(.data)
    }
}
