use core::cell::OnceCell;

use alloc::vec::Vec;
use horizon_dbg_protocol::{
  APP_TOPICS, ENDPOINTS_LIST, SWITCH_TOPICS, UpdateSubscription, result::HorizonResult,
  subscription::UpdateSubscriptionMsg,
};
use postcard_rpc::{
  define_dispatch, endpoint, endpoints,
  header::VarHeader,
  server::{AsWireTxErrorKind, Sender, Server, SpawnContext, WireTx},
};
use serde::Serialize;

static mut INSTANCE: OnceCell<ProtocolServer> = OnceCell::new();

pub struct ProtocolServer {
  server: Server<T, R, Vec<u8>, ThisPatcher>,
}

impl ProtocolServer {
  pub fn initialize() {
    todo!()
    // if let Err(_) = unsafe { INSTANCE.set(ProtocolServer {}) } {
    //   unreachable!("protocol server was already initialized")
    // }
  }
}

endpoint!(TestEndpoint, (), (), "testawesome");

fn spawn_fn(spawner: &Spawner, second: ()) -> Result<(), ()> {
  Ok(())
}
fn test_endpoint(
  _ctx: &mut Context,
  header: VarHeader,
  something: UpdateSubscriptionMsg,
) -> Result<(), HorizonResult> {
  Ok(())
}
define_dispatch! {
  app: ThisPatcher;
  spawn_fn: spawn_fn;
  tx_impl: T;
  spawn_impl: Spawner;
  context: Context;
  endpoints: {
    list: ENDPOINTS_LIST;

    | EndpointTy    | kind    | handler        |
    | ----------    | ----    | -------        |
    | UpdateSubscription | blocking   | test_endpoint  |
  };
  topics_in: {
    list: APP_TOPICS;

    | TopicTy       | kind    | handler        |
    | - | - | - |
  };
  topics_out: {
    list: SWITCH_TOPICS;
  };

}

struct Error;
impl AsWireTxErrorKind for Error {
  fn as_kind(&self) -> postcard_rpc::server::WireTxErrorKind {
    postcard_rpc::server::WireTxErrorKind::Other
  }
}

struct T {}
impl WireTx for T {
  type Error = Error;

  async fn send<T: Serialize + ?Sized>(
    &self,
    hdr: postcard_rpc::header::VarHeader,
    msg: &T,
  ) -> Result<(), Self::Error> {
    let mut hdr_buf = [0; 9];
    let (before, _after) = hdr
      .write_to_slice(&mut hdr_buf)
      .expect("buffer is too long for header");
    self.send_raw(&before).await.map_err(|_| Error)?;

    todo!("serialize msg")
  }

  async fn send_raw(&self, buf: &[u8]) -> Result<(), Self::Error> {
    todo!("call to furity code {:?}", buf);
  }

  async fn send_log_str(
    &self,
    kkind: postcard_rpc::header::VarKeyKind,
    s: &str,
  ) -> Result<(), Self::Error> {
  }

  async fn send_log_fmt<'a>(
    &self,
    kkind: postcard_rpc::header::VarKeyKind,
    a: core::fmt::Arguments<'a>,
  ) -> Result<(), Self::Error> {
    todo!()
  }
}

struct Spawner {}

struct Context;
impl SpawnContext for Context {
  type SpawnCtxt = *mut u8;

  fn spawn_ctxt(&mut self) -> Self::SpawnCtxt {
    todo!()
  }
}
