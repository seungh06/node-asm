import { process, asm, read_memory } from '../asm'

export class image_section_header {
        constructor(private readonly process: process, private readonly base: number) { }
        get Name                 () { return read_memory<string>(this.process, this.base + 0x00, asm.string)  }
        get PhysicalAddress      () { return read_memory<number>(this.process, this.base + 0x08, asm.uint32)  }
        get VirtualSize          () { return read_memory<number>(this.process, this.base + 0x08, asm.uint32)  }
        get VirtualAddress       () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint32)  }
        get SizeOfRawData        () { return read_memory<number>(this.process, this.base + 0x10, asm.uint32)  }
        get PointerToRawData     () { return read_memory<number>(this.process, this.base + 0x14, asm.uint32)  }
        get PointerToRelocations () { return read_memory<number>(this.process, this.base + 0x18, asm.uint32)  }
        get PointerToLinenumbers () { return read_memory<number>(this.process, this.base + 0x1c, asm.uint32)  }
        get NumberOfRelocations  () { return read_memory<number>(this.process, this.base + 0x20, asm.uint16)  }
        get NumberOfLinenumbers  () { return read_memory<number>(this.process, this.base + 0x22, asm.uint16)  }
        get Characteristics      () { return read_memory<number>(this.process, this.base + 0x24, asm.uint32)  }
}