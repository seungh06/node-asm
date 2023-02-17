import { process, asm, read_memory, is_x86 }                from '../lib'
import { image_file_header }                                from './image_file_header'
import { image_optional_header32, image_optional_header64 } from './image_optional_header'

export type image_nt_headers32 = {
        Signature      : number
        FileHeader     : image_file_header
        OptionalHeader : image_optional_header32
}

export type image_nt_headers64 = {
        Signature      : number
        FileHeader     : image_file_header
        OptionalHeader : image_optional_header64
}

export function image_nt_headers(process: process, base: number): image_nt_headers32 | image_nt_headers64 {
        return (is_x86(process) ? image_nt_headers32 : image_nt_headers64)(process, base);
}

export function image_nt_headers32(process: process, base: number): image_nt_headers32 {
        return {
                Signature      : read_memory(process, base + 0x00, asm.uint32),
                FileHeader     : new image_file_header(process, base + 0x04),
                OptionalHeader : new image_optional_header32(process, base + 0x18)
        }
}

export function image_nt_headers64(process: process, base: number): image_nt_headers64 {
        return {
                Signature      : read_memory(process, base + 0x00, asm.uint32),
                FileHeader     : new image_file_header(process, base + 0x04),
                OptionalHeader : new image_optional_header64(process, base + 0x18)
        }
}