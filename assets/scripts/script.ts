export function onUpdate(dt: f32): void {
  let x = "Hello World";
  x += "!!!";
  logMessage(x);
}

function logMessage(message: string): void {
  // Encode as Uint8Array directly
  let utf8 = String.UTF8.encode(message, true); // Returns ArrayBuffer
  let view = Uint8Array.wrap(utf8); // Create a typed array view
  let ptr = view.dataStart; // Correct memory pointer
  log(<i32>ptr, view.length - 1); // Exclude null terminator
}
