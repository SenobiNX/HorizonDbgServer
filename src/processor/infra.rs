#![allow(static_mut_refs)] // hehe

use core::{
  cell::UnsafeCell,
  pin::Pin,
  slice,
  task::{Context, Poll, RawWaker, RawWakerVTable, Waker},
};

use alloc::boxed::Box;
use maitake_sync::blocking::Mutex;

use crate::cpp_interop::mutex::SwitchFutex;

#[cxx::bridge(namespace = "hdbg::processor")]
mod _ffi {
  extern "Rust" {
    type ProtocolProcessor;

    #[Self = "ProtocolProcessor"]
    fn initialize();
    #[Self = "ProtocolProcessor"]
    unsafe fn process_data(data: *const u8, size: usize);
  }
}

static INSTANCE: Mutex<Option<ProtocolProcessor>, SwitchFutex> =
  Mutex::new_with_raw_mutex(None, SwitchFutex::new());

pub struct ProtocolProcessor {
  processor_future: Pin<Box<dyn Future<Output = ()> + Send>>,
}

struct State<'a> {
  current_buffer: &'a [u8],
}

impl ProtocolProcessor {
  // #[unsafe(export_name = "_ZN4hdbg9processor10initializeEv")]
  pub fn initialize() {
    let _old = INSTANCE.with_lock(|value| {
      value.replace(ProtocolProcessor {
        processor_future: Box::pin(super::process()),
      })
    });
  }

  // #[unsafe(export_name = "_ZN4hdbg9processor11processDataEPKhm")]
  pub fn process_data(data: *const u8, size: usize) {
    INSTANCE.with_lock(|value| {
      let value = value
        .as_mut()
        .expect("tried to process data while processor is not initialized");

      let state = UnsafeCell::new(State {
        current_buffer: unsafe { slice::from_raw_parts(data, size) },
      });

      let waker = unsafe { Waker::new(state.get().cast_const().cast(), &PROCESSING_WAKER) };
      
      if let Poll::Ready(()) = value
        .processor_future
        .as_mut()
        .poll(&mut Context::from_waker(&waker))
      {
        panic!("processing ended unexpectedly");
      }
    });
  }
}

unsafe fn clone(ctx: *const ()) -> RawWaker {
  RawWaker::new(ctx, &PROCESSING_WAKER)
}
unsafe fn wake(_ctx: *const ()) {}
const PROCESSING_WAKER: RawWakerVTable = RawWakerVTable::new(clone, wake, wake, drop);

pub(super) fn read(buf: &mut [u8]) -> Read<'_> {
  Read { buf }
}

pub struct Read<'a> {
  buf: &'a mut [u8],
}

impl Future for Read<'_> {
  type Output = ();

  fn poll(mut self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output> {
    let state = unsafe {
      cx.waker()
        .data()
        .cast::<State>()
        .cast_mut()
        .as_mut()
        .unwrap()
    };
    let amount_to_take = self.buf.len().min(state.current_buffer.len());
    let (write, _new) = self.buf.split_at_mut(amount_to_take);
    write.copy_from_slice(state.current_buffer);
    let slice = core::mem::take(&mut self.buf);
    self.buf = &mut slice[amount_to_take..];

    if amount_to_take < self.buf.len() {
      Poll::Pending
    } else {
      Poll::Ready(())
    }
  }
}
