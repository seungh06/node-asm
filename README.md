## **Node-ASM**: Read and Write, Execute.

[![nodejs](https://img.shields.io/badge/NodeJS-339933?style=for-the-badge&logo=Node.js&logoColor=fff)](https://nodejs.org/)
[![typescript](https://img.shields.io/badge/TypeScript-3178C6?style=for-the-badge&logo=TypeScript&logoColor=fff)](https://www.typescriptlang.org/)
[![C++](https://img.shields.io/badge/cpp-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=fff)](https://www.typescriptlang.org/)
[![license](https://img.shields.io/badge/license-MIT-9999FF?style=for-the-badge)](/LICENSE)
[![npm](https://img.shields.io/badge/download-NPM-CB3837?style=for-the-badge)](https://www.npmjs.com/package/node-asm)

### 🕹 Install
```bash
 $ yarn add node-asm
 $ npm install node-asm
```

## 🔥 Features
- TypeScript based, zero included dependencies. 📦
- Support x86 and x64 binary and functions depending on the process architecture.
- List all open processes and get process info specified by pid or name.
- Load modules associated with a process and find specific module.
- Inject and eject dll module.
- Read and write process memory with defined types.
- Pointer address calculation, array of bytes pattern scanning.
- Load all procedures in specific module.
- Call procedure with auto-generated shellcode according to arguments. ✨
- Load all sections in module and find specific section as structure.
- Windows PE format header implementations.

## 📌 About Node Addon
Node-asm internally uses C++ native modules through [**Node Addon API**](https://github.com/nodejs/node-addon-api) and [**Node-gyp**](https://github.com/nodejs/node-gyp), but distributed package include binary files that compiled for x86 and x64 architectures and depending on the **process environment** of NodeJS, the binary file of the appropriate architecture is used.

⭐ So you can use **node-asm** normally without installing **Node Addon API** and **Node-gyp**. but if you want to modify or rebuild node-asm package, the above modules are required.

## 📝 Usage

- ### 🕹️ Process
``` typescript
 const processes: Array<process> = load_processes(); 
 
 const process: process = open_process('notepad.exe'); // Process name or PID. 
 const is_32: boolean = is_x86(process);
 
 close_process(process); // Release process handle.
```
#### Process Structure
```typescript
 { exe: string, handle: handle, pid: number, ppid: number, dw_size: number, classbase: number, cnt_threads: number }
```

- ### 🗃️ Module
``` typescript
 const modules: Array<module> = load_modules(process);
 const module: module = find_module(process, 'user32.dll'); // Find user32 module in process.
```
``` typescript
 const success: boolean = inject_module(process, 'injection.dll'); // Absolute or relative module path.
 
 const success: boolean = eject_module(process, 'user32.dll');
 const success: boolean = eject_module(process, module);
```

#### Module Structure
```typescript
 { name: string, path: string, pid: number, address: number, size: number }
```

- ### 📄 Memory
``` typescript
 const output: any = read_memory(process, 0x1234, asm.int8);
 const output: number = read_memory<number>(process, 0x1234, asm.int32); // Type casting.
```

``` typescript
 write_memory(process, 0x1234, 'Hello World', asm.string);
 write_memory<number>(process, 0x1234, 100, asm.int32); // Value type casting.
  
 write_memory<number>(process, 0x1234, 'Hello World', asm.string); // ERROR!
```

```typescript
 // function get_pointer_address(process: process, base_address: number, ...offsets: Array<number>)
 
 const output: number = get_pointer_address(process, module.address + 0x032A5328, 0x198, 0x2D0, 0x2F8);
```

```typescript
 const signature = '8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF';
 const extra = 0x4, offset = 0x3;
 
 const output: number = aob_scan(process, signature, extra, offset); // scan all modules.
 const output: number = aob_scan_module(process, module, signature, extra, offset);
```

```typescript
 const allocated: number = alloc(process, address, size, allocation, protection);
```

#### Defined Types
```typescript
 asm.int8, asm.int16, asm.int32, asm.int64, asm.uint8, asm.uint16, asm.uint32, asm.uint64
 asm.bool, asm.float, asm.double, asm.string,
 
 asm.BYTE = asm.uint8, asm.WORD = asm.uint16, asm.DWORD = asm.uint32, asm.QWORD = asm.uint64
```

- ### 🔌 Procedure
``` typescript
 const procedures: Array<procedure> = load_procedures(process, module);
 const procedure: procedure = find_procedure(process, module, 'MessageBoxA');
```

``` typescript
 /* int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType); */
 const exitcode: number = call_procedure(
         process, procedure.address,
         [
                 { type: asm.int8  , value: 0x0             },
                 { type: asm.string, value: "Hello World"   },
                 { type: asm.string, value: "From node-asm" },
                 { type: asm.int8  , value: 0x1             },
         ], // Array<procedure_argument>
         true // dump shellcode. default is false.
);
```

#### Procedure Structure & Procedure Argument Structure
```typescript
 { name: string, address: number }, { type: asm, value: any }
```
## 🔐 Windows PE Headers
```typescript
 const dos: image_dos_header = new image_dos_header(process, module.address);
 const nt: image_nt_header32 | image_nt_header64 = image_nt_headers(process, module.address + dos.e_lfanew);
 
 const machine = nt.FileHeader.Machine;
 console.log(machine === 0x8664 ? 'x64' : 'x86');
```
Node-asm implements the **Windows PE headers** to execute tasks such as **load_procedures** and **load_sections**. Also users can use exported implementations in `node-asm/header` path.

Headers with architecture-dependent structures such as `image_nt_headers` and `image_optional_headers` support functions that return appropriate structures depending on the **target process**.
> See more kinds and properties of header [here](/src/win32).
## 📋 License
Distributed under the MIT License. See ```LICENSE``` for more information.
