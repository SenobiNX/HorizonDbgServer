pub mod mutex;

use core::fmt;

unsafe extern "C" {
  fn rust_log(text: *const u8, size: usize);
  pub fn rust_panicked() -> !;
}

pub fn log(text: &str) {
  unsafe { rust_log(text.as_ptr(), text.len()) };
}

#[doc(hidden)]
pub struct Formatter;
impl fmt::Write for Formatter {
  fn write_str(&mut self, s: &str) -> fmt::Result {
    log(s);
    Ok(())
  }
}

#[macro_export]
macro_rules! println {
  ($($tree: tt)*) => {{
    use ::core::fmt::Write;
    // this will never return an error
    unsafe {
      ::core::write!(&mut $crate::cpp_interop::Formatter, $($tree)*).unwrap_unchecked()
    };
  }};
}
