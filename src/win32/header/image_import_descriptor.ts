import { process, asm, read_memory } from '../lib'

export class image_import_descriptor {
        constructor(private readonly process: process, private readonly base: number) { }
        get OriginalFirstThunk () { return read_memory(this.process, this.base + 0x00, asm.uint32) }
        get TimeDateStamp      () { return read_memory(this.process, this.base + 0x04, asm.uint32) }
        get ForwarderChain     () { return read_memory(this.process, this.base + 0x08, asm.uint32) }
        get Name               () { return read_memory(this.process, this.base + 0x0c, asm.uint32) }
        get FirstThunk         () { return read_memory(this.process, this.base + 0x10, asm.uint32) }
}