#include <wasm3.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

#include "script/ScriptHandle.hpp"
#include "script/ScriptInstanceHandle.hpp"
#include "script/ScriptManager.hpp"

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

int main() {
  ScriptManager scriptManager;

  auto wasmBinary = loadWasmFile("./assets/wasm/script.wasm");

  // Load the script and create an instance
  ScriptHandle scriptHandle = scriptManager.loadScript(wasmBinary);
  ScriptInstanceHandle instanceHandle = scriptManager.createInstance(scriptHandle);

  while (true) {
    try {
      // Run update
      float dt = 0.016f;
      scriptManager.updateInstance(instanceHandle, dt);

    } catch (const std::exception& ex) {
      std::cerr << "Fatal: " << ex.what() << "\n";
      return 1;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(16666));
  }

  return 0;
}