.global smp_initializer_begin
smp_initializer_begin:

.incbin "obj/smpinit.bin"

.global smp_initializer_end
smp_initializer_end:
