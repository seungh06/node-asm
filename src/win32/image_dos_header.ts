import { process, asm, read_memory } from '../asm'

export class image_dos_header {
        constructor(private readonly process: process, private readonly base: number) { }
        get e_magic     () { return read_memory<number>(this.process, this.base + 0x00, asm.uint16) }
        get e_cblp      () { return read_memory<number>(this.process, this.base + 0x02, asm.uint16) }
        get e_cp        () { return read_memory<number>(this.process, this.base + 0x04, asm.uint16) }
        get e_crlc      () { return read_memory<number>(this.process, this.base + 0x06, asm.uint16) }
        get e_cparhdr   () { return read_memory<number>(this.process, this.base + 0x08, asm.uint16) }
        get e_minalloc  () { return read_memory<number>(this.process, this.base + 0x0a, asm.uint16) }
        get e_maxalloc  () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint16) }
        get e_ss        () { return read_memory<number>(this.process, this.base + 0x0e, asm.uint16) }
        get e_sp        () { return read_memory<number>(this.process, this.base + 0x10, asm.uint16) }
        get e_csum      () { return read_memory<number>(this.process, this.base + 0x12, asm.uint16) }
        get e_ip        () { return read_memory<number>(this.process, this.base + 0x14, asm.uint16) }
        get e_cs        () { return read_memory<number>(this.process, this.base + 0x16, asm.uint16) }
        get e_lfarlc    () { return read_memory<number>(this.process, this.base + 0x18, asm.uint16) }
        get e_ovno      () { return read_memory<number>(this.process, this.base + 0x1a, asm.uint16) }
        get e_res       () { return 0 } // Array(4).fill(0).map((_, index) => read_memory(process, base + 0x1c + (index * 0x2), asm.int16))
        get e_oemid     () { return read_memory<number>(this.process, this.base + 0x24, asm.uint16) }
        get e_oeminfo   () { return read_memory<number>(this.process, this.base + 0x26, asm.uint16) }
        get e_res2      () { return 0 } // Array(10).fill(0).map((_, index) => read_memory(process, base + 0x28 + (index * 0x2), asm.int16))
        get e_lfanew    () { return read_memory<number>(this.process, this.base + 0x3c, asm.int32)  }
}