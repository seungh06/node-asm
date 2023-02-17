import { process, asm, read_memory } from '../lib'

export class image_export_directory {
        constructor(private readonly process: process, private readonly base: number) { }
        get Characteristics    () { return read_memory<number>(this.process, this.base + 0x00, asm.uint32) }
        get TimeDateStamp      () { return read_memory<number>(this.process, this.base + 0x04, asm.uint32) }
        get MajorVersion       () { return read_memory<number>(this.process, this.base + 0x08, asm.uint16) }
        get MinorVersion       () { return read_memory<number>(this.process, this.base + 0x0a, asm.uint16) }
        get Name               () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint32) }
        get Base               () { return read_memory<number>(this.process, this.base + 0x10, asm.uint32) }
        get NumberOfFunctions  () { return read_memory<number>(this.process, this.base + 0x14, asm.uint32) }
        get NumberOfNames      () { return read_memory<number>(this.process, this.base + 0x18, asm.uint32) }
        get AddressOfFunctions () { return read_memory<number>(this.process, this.base + 0x1c, asm.uint32) }
        get AddressOfNames     () { return read_memory<number>(this.process, this.base + 0x20, asm.uint32) }
        get AddressOfOrdinals  () { return read_memory<number>(this.process, this.base + 0x24, asm.uint32) }
}