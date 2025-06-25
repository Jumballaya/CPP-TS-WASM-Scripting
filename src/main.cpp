#include <wasm3.h>

#include <fstream>
#include <iostream>
#include <vector>

// Simple helper to read a WASM file into memory
std::vector<uint8_t> loadWasmFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) throw std::runtime_error("Failed to open " + path);
  size_t size = file.tellg();
  std::vector<uint8_t> buf(size);
  file.seekg(0);
  file.read(reinterpret_cast<char*>(buf.data()), size);
  return buf;
}

// Global pointer to runtime so our host function can access memory
static IM3Runtime wasmRuntime = nullptr;

// Host function that AssemblyScript will import as `log(ptr: i32, len: i32)`
m3ApiRawFunction(log) {
  m3ApiGetArg(int32_t, ptr);
  m3ApiGetArg(int32_t, len);

  // Access the WASM module's linear memory
  uint8_t* memory = m3_GetMemory(wasmRuntime, nullptr, 0);
  std::string message(reinterpret_cast<char*>(memory + ptr), len);
  std::cout << "[script] " << message << "\n";

  m3ApiSuccess();
}

int main() {
  try {
    auto wasmBinary = loadWasmFile("./assets/wasm/script.wasm");

    IM3Environment env = m3_NewEnvironment();
    IM3Runtime runtime = m3_NewRuntime(env, 64 * 1024, nullptr);
    wasmRuntime = runtime;  // for use in log()

    IM3Module module;
    m3_ParseModule(env, &module, wasmBinary.data(), wasmBinary.size());
    m3_LoadModule(runtime, module);

    // Link the log function: no return (v), two int args (ii)
    m3_LinkRawFunction(module, "env", "log", "v(ii)", &log);

    // Optional: initialize module if there's a start function
    m3_RunStart(module);

    IM3Function onUpdate;
    m3_FindFunction(&onUpdate, runtime, "onUpdate");

    const char* argv[2] = {"0.016", nullptr};  // Pass dt = 0.016 as string
    M3Result result = m3_CallArgv(onUpdate, 1, argv);
    if (result != m3Err_none) {
      std::cerr << "Error calling onUpdate: " << result << "\n";
    }

    m3_FreeRuntime(runtime);
    m3_FreeEnvironment(env);
  } catch (const std::exception& ex) {
    std::cerr << "Fatal: " << ex.what() << "\n";
    return 1;
  }
  return 0;
}