import process from 'node:process'
import path from 'node:path'
import subprocess from 'node:child_process'
import fstream from 'node:fs'

export const _native_asm = require(path.resolve('.', `bin/asm-linux-${ ['arm64', 'ppc64', 'x64', 's390x'].includes(process.arch) ? 'x64' : 'x86' }`));
export const dump = (address: number) => process.stdout.write(`0x${ address.toString(16).toUpperCase() }\n`);
export const invalid_handle: handle = -1;

export type handle = /* PID IN LINUX */ number;
export type process = { exe: string, handle: handle, pid: number, ppid: number, threads: number };

export function load_processes(): Array<process> {
        return fstream.readdirSync('/proc', { withFileTypes: true })
                .filter(data => data.isDirectory() && /^\d+$/.test(data.name))

        .map(function mapper(dirent) {
                const stat = fstream.readFileSync(`/proc/${dirent.name}/stat`).toString().split(' ');
                return { exe: stat[1].slice(1, -1), handle: invalid_handle, pid: parseInt(stat[0]), ppid: parseInt(stat[3]), threads: parseInt(stat[19]) }
        })
}

export function open_process(target_process: string | number): process {
        const process = load_processes().find(process => 
                typeof target_process === 'string'
                        ? process.exe.toLowerCase() === target_process.toLowerCase()
                        : process.pid === target_process         
        );
        if(!process) throw new Error(`cannot find process '${ target_process }'.`);

        return { ...process, handle: process.pid };
}

export function close_process(process: process): boolean {
        return (process.handle = -1) === invalid_handle;
}

export function is_x86(process: process): boolean {
        if(process.handle === invalid_handle) throw new Error(`process handle of '${ process.exe }' is not valid.`);

        const stdout = subprocess.execSync(`file -L /proc/${ process.handle }/exe`).toString();
        return stdout.match(/(32|64)(?=-bit)/)!![0] === '32';
}

export type module = { name: string, path: string, pid: number, address: number };

export function load_modules(process: process): Array<module> {
        if(process.handle === invalid_handle) throw new Error(`process handle of '${ process.exe }' is not valid.`);

        const dependencies = subprocess.execSync(`ldd /proc/${ process.handle }/exe`).toString().trim();
        const modules: Array<module> = [];

        for(const dependency of dependencies.split('\n')) {

                const include = dependency.includes('=>');
                const matches = dependency.match(
                        include 
                                ? /(.+?)\s=>\s(.+)\s\((.+)\)/ 
                                : /(.+?)\s\((.+)\)/
                );
                if(!matches) throw new Error(`cannot find modules in process '${ process.exe }'.`);

                modules.push({ 
                        name: path.basename(matches[1].trim()), path: matches[ include ? 2 : 1 ].trim(), pid: process.pid, 
                        address: parseInt(matches[ include ? 3 : 2 ]) 
                });
        }
        return modules;
}

export function find_module(process: process, target_module: string) {
        const module = load_modules(process).find(module => module.name.toLowerCase() === target_module.toLowerCase());
        if(!module) throw new Error(`cannot find module '${ target_module }' in process '${ process.exe }'.`);
        
        return module;
}