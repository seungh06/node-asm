import { process, asm, read_memory, is_x86 } from '../asm'
import { image_data_directory }              from './image_data_directory'

export const IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;
export function image_optional_header(process: process, base: number): image_optional_header32 | image_optional_header64 {
        return new (is_x86(process) ? image_optional_header32 : image_optional_header64)(process, base);
}

export class image_optional_header32 {
        constructor(private readonly process: process, private readonly base: number) { }
        get Magic                       () { return read_memory<number>(this.process, this.base + 0x00, asm.uint16) }
        get MajorLinkerVersion          () { return read_memory<number>(this.process, this.base + 0x02, asm.uint8)  }
        get MinorLinkerVersion          () { return read_memory<number>(this.process, this.base + 0x03, asm.uint8)  }
        get SizeOfCode                  () { return read_memory<number>(this.process, this.base + 0x04, asm.uint32) }
        get SizeOfInitializedData       () { return read_memory<number>(this.process, this.base + 0x08, asm.uint32) }
        get SizeOfUninitializedData     () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint32) }
        get AddressOfEntryPoint         () { return read_memory<number>(this.process, this.base + 0x10, asm.uint32) }
        get BaseOfCode                  () { return read_memory<number>(this.process, this.base + 0x14, asm.uint32) }
        get BaseOfData                  () { return read_memory<number>(this.process, this.base + 0x18, asm.uint32) }
        get ImageBase                   () { return read_memory<number>(this.process, this.base + 0x1c, asm.uint32) }
        get SectionAlignment            () { return read_memory<number>(this.process, this.base + 0x20, asm.uint32) }
        get FileAlignment               () { return read_memory<number>(this.process, this.base + 0x24, asm.uint32) }
        get MajorOperatingSystemVersion () { return read_memory<number>(this.process, this.base + 0x28, asm.uint16) }
        get MinorOperatingSystemVersion () { return read_memory<number>(this.process, this.base + 0x2a, asm.uint16) }
        get MajorImageVersion           () { return read_memory<number>(this.process, this.base + 0x2c, asm.uint16) }
        get MinorImageVersion           () { return read_memory<number>(this.process, this.base + 0x2e, asm.uint16) }
        get MajorSubsystemVersion       () { return read_memory<number>(this.process, this.base + 0x30, asm.uint16) }
        get MinorSubsystemVersion       () { return read_memory<number>(this.process, this.base + 0x32, asm.uint16) }
        get Win32VersionValue           () { return read_memory<number>(this.process, this.base + 0x34, asm.uint32) }
        get SizeOfImage                 () { return read_memory<number>(this.process, this.base + 0x38, asm.uint32) }
        get SizeOfHeaders               () { return read_memory<number>(this.process, this.base + 0x3c, asm.uint32) }
        get CheckSum                    () { return read_memory<number>(this.process, this.base + 0x40, asm.uint32) }
        get Subsystem                   () { return read_memory<number>(this.process, this.base + 0x44, asm.uint16) }
        get DllCharacteristics          () { return read_memory<number>(this.process, this.base + 0x46, asm.uint16) }
        get SizeOfStackReserve          () { return read_memory<number>(this.process, this.base + 0x48, asm.uint32) }
        get SizeOfStackCommit           () { return read_memory<number>(this.process, this.base + 0x4c, asm.uint32) }
        get SizeOfHeapReserve           () { return read_memory<number>(this.process, this.base + 0x50, asm.uint32) }
        get SizeOfHeapCommit            () { return read_memory<number>(this.process, this.base + 0x54, asm.uint32) }
        get LoaderFlags                 () { return read_memory<number>(this.process, this.base + 0x58, asm.uint32) }
        get NumberOfRvaAndSizes         () { return read_memory<number>(this.process, this.base + 0x5c, asm.uint32) }
        get DataDirectory               () { 
                return Array(IMAGE_NUMBEROF_DIRECTORY_ENTRIES).fill(0)
                        .map((_, index) => new image_data_directory(this.process, this.base + 0x60 + index * 0x08)) 
        }
}

export class image_optional_header64 {
        constructor(private readonly process: process, private readonly base: number) { }
        get Magic                       () { return read_memory<number>(this.process, this.base + 0x00, asm.uint16) }
        get MajorLinkerVersion          () { return read_memory<number>(this.process, this.base + 0x02, asm.uint8)  }
        get MinorLinkerVersion          () { return read_memory<number>(this.process, this.base + 0x03, asm.uint8)  }
        get SizeOfCode                  () { return read_memory<number>(this.process, this.base + 0x04, asm.uint32) }
        get SizeOfInitializedData       () { return read_memory<number>(this.process, this.base + 0x08, asm.uint32) }
        get SizeOfUninitializedData     () { return read_memory<number>(this.process, this.base + 0x0c, asm.uint32) }
        get BaseOfCode                  () { return read_memory<number>(this.process, this.base + 0x14, asm.uint32) }
        get ImageBase                   () { return read_memory<number>(this.process, this.base + 0x18, asm.uint64) }
        get SectionAlignment            () { return read_memory<number>(this.process, this.base + 0x20, asm.uint32) }
        get FileAlignment               () { return read_memory<number>(this.process, this.base + 0x24, asm.uint32) }
        get MajorOperatingSystemVersion () { return read_memory<number>(this.process, this.base + 0x28, asm.uint16) }
        get MinorOperatingSystemVersion () { return read_memory<number>(this.process, this.base + 0x2A, asm.uint16) }
        get MajorImageVersion           () { return read_memory<number>(this.process, this.base + 0x2C, asm.uint16) }
        get MinorImageVersion           () { return read_memory<number>(this.process, this.base + 0x2E, asm.uint16) }
        get MajorSubsystemVersion       () { return read_memory<number>(this.process, this.base + 0x30, asm.uint16) }
        get MinorSubsystemVersion       () { return read_memory<number>(this.process, this.base + 0x32, asm.uint16) }
        get Win32VersionValue           () { return read_memory<number>(this.process, this.base + 0x34, asm.uint32) }
        get SizeOfImage                 () { return read_memory<number>(this.process, this.base + 0x38, asm.uint32) }
        get SizeOfHeaders               () { return read_memory<number>(this.process, this.base + 0x3C, asm.uint32) }
        get CheckSum                    () { return read_memory<number>(this.process, this.base + 0x40, asm.uint32) }
        get Subsystem                   () { return read_memory<number>(this.process, this.base + 0x44, asm.uint16) }
        get DllCharacteristics          () { return read_memory<number>(this.process, this.base + 0x46, asm.uint16) }
        get SizeOfStackReserve          () { return read_memory<number>(this.process, this.base + 0x48, asm.uint64) }
        get SizeOfStackCommit           () { return read_memory<number>(this.process, this.base + 0x50, asm.uint64) }
        get SizeOfHeapReserve           () { return read_memory<number>(this.process, this.base + 0x58, asm.uint64) }
        get SizeOfHeapCommit            () { return read_memory<number>(this.process, this.base + 0x60, asm.uint64) }
        get LoaderFlags                 () { return read_memory<number>(this.process, this.base + 0x68, asm.uint32) } 
        get NumberOfRvaAndSizes         () { return read_memory<number>(this.process, this.base + 0x6C, asm.uint32) }
        get DataDirectory               () { 
                return Array(IMAGE_NUMBEROF_DIRECTORY_ENTRIES).fill(0)
                        .map((_, index) => new image_data_directory(this.process, this.base + 0x70 + index * 0x08)) 
        }
}