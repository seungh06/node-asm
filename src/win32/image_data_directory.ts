import { process, asm, read_memory } from '../asm'

export class image_data_directory {
        constructor(private readonly process: process, private readonly base: number) { }
        get VirtualAddress () { return read_memory<number>(this.process, this.base + 0x00, asm.uint32) }
        get Size           () { return read_memory<number>(this.process, this.base + 0x04, asm.uint32) }
}