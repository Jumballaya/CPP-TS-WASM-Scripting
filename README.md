# AssemblyScript Scripting System

Why use Lua or C# when you could use WASM and write in TypeScript! All the benefits of using TS along with static typing and compilation.

## Requirements

- nodeJS and npm
  - Tested at nodeJS version `v22.15.0`
  - Tested at npm `10.9.2`
- AssemblyScript at `0.28.2`
  - `npm install -g assemblyscript`
- CMake `3.20`+

## Build and Run

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./wasm_system
```
