#include "HostFunctions.hpp"

static IM3Runtime currentRuntime = nullptr;

m3ApiRawFunction(log) {
  m3ApiGetArg(int32_t, ptr);
  m3ApiGetArg(int32_t, len);

  uint8_t* memory = m3_GetMemory(currentRuntime, nullptr, 0);
  std::string message(reinterpret_cast<char*>(memory + ptr), len);
  std::cout << "[script] " << message << "\n";

  m3ApiSuccess();
}

void linkCommonHostFunctions(IM3Module module, IM3Runtime runtime) {
  currentRuntime = runtime;

  M3Result result = m3_LinkRawFunction(module, "env", "log", "v(ii)", &log);
  if (result != m3Err_none) {
    throw std::runtime_error(std::string("Failed to link host function: ") + result);
  }
}