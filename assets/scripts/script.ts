export function onUpdate(dt: f32): void {
  logMessage("Hello World");
}

function logMessage(message: string): void {
  const ptr = changetype<usize>(message);
  log(<i32>ptr, message.length << 1);
}
