import process from 'node:process'
import path from 'node:path'
import { image_dos_header, image_nt_headers, image_export_directory, image_section_header } from './header'

export const _native_asm = require(path.resolve(`bin/asm-win-${ ['arm64', 'ppc64', 'x64', 's390x'].includes(process.arch) ? 'x64' : 'x86' }`));
export const dump = (address: number) => process.stdout.write(`0x${ address.toString(16).toUpperCase() }\n`);

export type handle = /* HANDLE POINTER */ number;
export type process = { exe: string, handle: handle, pid: number, ppid: number, dw_size: number, classbase: number, cnt_threads: number };

export function load_processes(): Array<process> {
        return _native_asm.load_processes();
}

export function open_process(target_process: string | number): process {
        const process = load_processes().find(process => 
                typeof target_process === 'string'
                        ? process.exe.toLowerCase() === target_process.toLowerCase()
                        : process.pid === target_process         
        );
        if(!process) throw new Error(`cannot find process '${ target_process }'.`);

        const handle = _native_asm.open_process(process.pid);
        if(!handle) throw new Error(`cannot open process handle '${ target_process }'.`);

        return { ...process, handle };
}

export function close_process(process: process): boolean {
        return _native_asm.close_handle(process.handle);
}

export function is_x86(process: process): boolean {
        return _native_asm.is_x86(process.handle);
}

export type module = { name: string, path: string, pid: number, address: number, size: number };

export function load_modules(process: process): Array<module> {
        return _native_asm.load_modules(process.pid);
}

export function find_module(process: process, target_module: string): module {
        const module = load_modules(process).find(module => module.name.toLowerCase() === target_module);
        if(!module) throw new Error(`cannot find module '${ target_module }' in process '${ process.exe }'.`);
        
        return module;
}

export function inject_module(process: process, library: string): boolean {
        const kernel32 = find_module(process, 'kernel32.dll');
        const procedure = find_procedure(process, kernel32, 'LoadLibraryA').address;
        
        return _native_asm.inject_module(process.handle, path.resolve(library), procedure);
}

export function eject_module(process: process, module: module | string): boolean {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;

        const kernel32 = find_module(process, 'kernel32.dll');
        const procedure = find_procedure(process, kernel32, 'FreeLibrary').address;

        return _native_asm.eject_module(process.handle, target_module.address, procedure);
}

export enum asm {
        int8  = 'int8' , int16  = 'int16' , int32  = 'int32' , int64  = 'int64' ,
        uint8 = 'uint8', uint16 = 'uint16', uint32 = 'uint32', uint64 = 'uint64',
        bool  = 'bool' , float  = 'float' , double = 'double', string = 'string',

        BYTE = 'uint8', WORD = 'uint16', DWORD = 'uint32', QWORD = 'uint64' 
}

export function read_memory<T = any>(process: process, address: number, type: asm): T {
        return _native_asm.read_memory(process.handle, address, type);
}

export function write_memory<T = any>(process: process, address: number, value: T, type: asm): void {
        if([ asm.int64, asm.uint64 ].includes(type) && typeof value !== 'bigint') {
                throw new Error(`value of data type 'int64' or 'uint64' must be 'bigint'.`);
        }
        
        if(type === asm.string) (<string> value) += '\0'
        return _native_asm.write_memory(process.handle, address, value, type);
}

export function get_pointer_address(process: process, base_address: number, ...offsets: Array<number>): number {
        return _native_asm.get_pointer_address(process.handle, base_address, offsets);
}

export function aob_scan(process: process, signature: string, offset: number = 0x0, extra: number = 0x0): number {
        for(const module of load_modules(process)) {
                try {
                        return aob_scan_module(process, module, signature, offset, extra);
                } catch { /* CANNOT FIND SIGNATURE PATTERN */ }
        }

        throw new Error(`cannot find signature pattern in process '${ process.exe }'.`);
}

export function aob_scan_module(process: process, module: module | string, signature: string, offset: number = 0x0, extra: number = 0x0): number {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;
        
        const found = _native_asm.aob_scan(process.handle, target_module.address, target_module.size, signature, offset, extra);
        if(!found) throw new Error(`cannot find signature pattern in module '${ target_module.name }' in process '${ process.exe }'.`);

        return found;
}

export function alloc(process: process, address: number, size: number, allocation: number, protection: number): number {
        return _native_asm.virtual_alloc(process.handle, address, size, allocation, protection);
}

export type procedure = { name: string, address: number }
export type procedure_argument = { type: asm, value: any }

export function load_procedures(process: process, module: module | string): Array<procedure> {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;

        const dos_header = new image_dos_header(process, target_module.address);
        if(dos_header.e_magic !== 0x5A4D) throw new Error(`cannot load IMAGE DOS HEADER of module '${ target_module.name }'.`);

        const nt_header = image_nt_headers(process, target_module.address + dos_header.e_lfanew);
        if(nt_header.Signature !== 0x4550) throw new Error(`cannot load IMAGE NT HEADERS of module '${ target_module.name }'.`);

        const { 
                NumberOfNames, AddressOfNames, AddressOfOrdinals, AddressOfFunctions 
        } = new image_export_directory(process, target_module.address + nt_header.OptionalHeader.DataDirectory[0].VirtualAddress);

        const procedures: Array<procedure> = [];
        for(let index = 0; index < NumberOfNames; index ++) {
                const name_rav = read_memory(process, target_module.address + AddressOfNames + index * 0x4, asm.int32);
                const name = read_memory(process, target_module.address + name_rav, asm.string);

                const ordinals = read_memory(process, target_module.address + AddressOfOrdinals + index * 0x2, asm.int16);
                const procedure_rav = read_memory(process, target_module.address + AddressOfFunctions + ordinals * 0x4, asm.int32);

                procedures.push({ name: name, address: target_module.address + procedure_rav });
        }

        return procedures;
}

export function find_procedure(process: process, module: module | string, procedure: string): procedure {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;

        const dos_header = new image_dos_header(process, target_module.address);
        if(dos_header.e_magic !== 0x5A4D) throw new Error(`cannot load IMAGE DOS HEADER of module '${ target_module.name }'.`);

        const nt_header = image_nt_headers(process, target_module.address + dos_header.e_lfanew);
        if(nt_header.Signature !== 0x4550) throw new Error(`cannot load IMAGE NT HEADERS of module '${ target_module.name }'.`);

        const { 
                NumberOfNames, AddressOfNames, AddressOfOrdinals, AddressOfFunctions 
        } = new image_export_directory(process, target_module.address + nt_header.OptionalHeader.DataDirectory[0].VirtualAddress);

        for(let index = 0; index < NumberOfNames; index ++) {
                const name_rav = read_memory(process, target_module.address + AddressOfNames + index * 0x4, asm.int32);
                const name = read_memory(process, target_module.address + name_rav, asm.string);
                if(name !== procedure) continue;

                const ordinals = read_memory(process, target_module.address + AddressOfOrdinals + index * 0x2, asm.int16);
                const procedure_rav = read_memory(process, target_module.address + AddressOfFunctions + (ordinals * 4), asm.int32);

                return { name, address: target_module.address + procedure_rav };
        }

        throw new Error(`cannot find procedure '${ procedure }' in module '${ target_module.name }'.`);
}

export function call_procedure(process: process, procedure: number, args: Array<procedure_argument>, dump_shellcode: boolean = false): number {
        return (is_x86(process) ? call_procedure_x86 : call_procedure_x64)(process, procedure, args, dump_shellcode);
}

export function call_procedure_x86(process: process, procedure: number, args: Array<procedure_argument>, dump_shellcode: boolean = false): number {
        return _native_asm.call_procedure_x86(process.handle, procedure, args.reverse(), dump_shellcode);
}

export function call_procedure_x64(process: process, procedure: number, args: Array<procedure_argument>, dump_shellcode: boolean = false): number {
        return _native_asm.call_procedure_x64(process.handle, procedure, [ ...args.splice(0, 4), ...args.reverse() ], dump_shellcode);
}

export function load_sections(process: process, module: module | string): Array<image_section_header> {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;

        const dos_header = new image_dos_header(process, target_module.address);
        if(dos_header.e_magic !== 0x5A4D) throw new Error(`cannot load IMAGE DOS HEADER of module '${ target_module.name }'.`);

        const nt_header = image_nt_headers(process, target_module.address + dos_header.e_lfanew);
        if(nt_header.Signature !== 0x4550) throw new Error(`cannot load IMAGE NT HEADERS of module '${ target_module.name }'.`);

        const sections: Array<image_section_header> = [];
        const offset = dos_header.e_lfanew + 24 + nt_header.FileHeader.SizeOfOptionalHeader;

        for(let index = 0; index < nt_header.FileHeader.NumberOfSections; index ++)
                sections.push(new image_section_header(process, target_module.address + offset + 40 * index));

        return sections;
}

export function find_section(process: process, module: module | string, section: string): image_section_header {
        const target_module = typeof module === 'string' ? find_module(process, module) : module;

        const dos_header = new image_dos_header(process, target_module.address);
        if(dos_header.e_magic !== 0x5A4D) throw new Error(`cannot load IMAGE DOS HEADER of module '${ target_module.name }'.`);

        const nt_header = image_nt_headers(process, target_module.address + dos_header.e_lfanew);
        if(nt_header.Signature !== 0x4550) throw new Error(`cannot load IMAGE NT HEADERS of module '${ target_module.name }'.`);

        const offset = dos_header.e_lfanew + 24 + nt_header.FileHeader.SizeOfOptionalHeader;

        for(let index = 0; index < nt_header.FileHeader.NumberOfSections; index ++) {
                const section_header = new image_section_header(process, target_module.address + offset + 40 * index)
                if(section_header.Name === section) return section_header;
        }

        throw new Error(`cannot find section '${ section }' in module '${ target_module.name }'.`);
}