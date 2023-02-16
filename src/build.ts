import process from 'node:process'
import subprocess from 'node:child_process'
import fs from 'node:fs/promises'

void async function build() {
        process.stdout.write(`node-asm: build c++ native library. \n`);
        await fs.mkdir('_build');

        const build_x86 = subprocess.spawnSync('npm run build32', { shell: true, stdio: [ 'ignore', 'inherit', 'ignore' ] });

        build_x86.status === 0
                ? fs.copyFile('build/Release/asm.node', '_build/asm-x86.node')
                : console.log(`node-asm: failed to build x86 native module. exitcode: ${ build_x86.status }.`);

        const build_x64 = subprocess.spawnSync('npm run build64', { shell: true, stdio: [ 'ignore', 'inherit', 'ignore' ] });

        build_x64.status === 0
                ? fs.copyFile('build/Release/asm.node', '_build/asm-x64.node')
                : console.log(`node-asm: failed to build x64 native module. exitcode: ${ build_x64.status }.`);

        await fs.rm('build', { recursive: true });
        await fs.rename('_build', 'bin');

        process.stdout.write(`node-asm: c++ native library build compiled. \n`);
}()