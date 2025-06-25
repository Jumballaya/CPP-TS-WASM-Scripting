#include "ScriptInstance.hpp"

#include <stdexcept>
#include <string>

#include "HostFunctions.hpp"

ScriptInstance::ScriptInstance(ScriptInstanceHandle handle, IM3Environment env, const LoadedScript& script)
    : _handle(handle) {
  runtime = m3_NewRuntime(env, 64 * 1024, nullptr);
  if (!runtime) {
    throw std::runtime_error("unable to create wasm runtime for a script instance");
  }

  M3Result result = m3_LoadModule(runtime, script.module);
  if (result != m3Err_none) {
    throw std::runtime_error(std::string("unable to load wasm module into runtime: ") + result);
  }

  linkHostFunction(script.module);
  m3_RunStart(script.module);

  // Get the Lifecycle methods
  result = m3_FindFunction(&onUpdate, runtime, "onUpdate");
  if (result != m3Err_none) {
    throw std::runtime_error("onUpdate function not found in script.");
  }
}

void ScriptInstance::update(float dt) {
  if (!onUpdate) return;

  std::string dtStr = std::to_string(dt);
  const char* argv[2] = {dtStr.c_str(), nullptr};

  M3Result result = m3_CallArgv(onUpdate, 1, argv);
  if (result != m3Err_none) {
    throw std::runtime_error(std::string("Error calling onUpdate: ") + result);
  }
}

void ScriptInstance::linkHostFunction(IM3Module module) {
  linkCommonHostFunctions(module, runtime);
}