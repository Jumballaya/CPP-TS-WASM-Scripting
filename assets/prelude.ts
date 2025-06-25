@external("env", "log")
declare function log(ptr: i32, len: i32): void;

class console {
  static log(...messages: string[]): void {
    const message = messages.join("");
    const utf8 = String.UTF8.encode(message, true);
    const view = Uint8Array.wrap(utf8);
    log(<i32>view.dataStart, view.length - 1);
  }
}
