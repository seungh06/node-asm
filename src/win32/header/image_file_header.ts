import { process, asm, read_memory } from '../lib'

export class image_file_header {
        constructor(private readonly process: process, private readonly base: number) { }
        get Machine              () { return read_memory<number>(this.process, this.base + 0x00, asm.uint16) }
        get NumberOfSections     () { return read_memory<number>(this.process, this.base + 0x02, asm.uint16) }
        get TimeDateStamp        () { return read_memory<number>(this.process, this.base + 0x04, asm.uint32) }
        get PointerToSymbolTable () { return read_memory<number>(this.process, this.base + 0x08, asm.uint32) }
        get NumberOfSymbols      () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint32) }
        get SizeOfOptionalHeader () { return read_memory<number>(this.process, this.base + 0x10, asm.uint16) }
        get Characteristics      () { return read_memory<number>(this.process, this.base + 0x12, asm.uint16) }
}