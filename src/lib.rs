#![no_std]
#![feature(sync_unsafe_cell)]

extern crate alloc;

use core::alloc::GlobalAlloc;

#[global_allocator]
static GLOBAL_ALLOC: HkAlloc = HkAlloc;
struct HkAlloc;
unsafe impl GlobalAlloc for HkAlloc {
  unsafe fn alloc(&self, _layout: core::alloc::Layout) -> *mut u8 {
    todo!("interop")
  }

  unsafe fn dealloc(&self, _ptr: *mut u8, _layout: core::alloc::Layout) {
    todo!("interop")
  }
}

#[path = "interop/cpp_interop.rs"]
mod cpp_interop;
mod panic_handler;
mod processor;

#[unsafe(export_name = "test")]
pub extern "C" fn test(a: u32, b: u32) -> u32 {
  a * b
}
