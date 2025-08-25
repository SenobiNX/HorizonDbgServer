use core::panic::PanicInfo;

use crate::{cpp_interop::rust_panicked, println};

#[cfg_attr(not(test), panic_handler)]
fn panic_handler(info: &PanicInfo) -> ! {
  println!("a {:?}", info);
  unsafe { rust_panicked() };
}
