// use postcard_rpc::{
//   define_dispatch, endpoint,
//   server::{AsWireTxErrorKind, WireTx},
// };
// use serde::Serialize;

// static mut INSTANCE: OnceCell<ProtocolServer> = OnceCell::new();

// pub struct ProtocolServer {
//   server: Server,
// }

// impl ProtocolServer {
//   pub fn initialize() {
//     if let Err(_) = unsafe { INSTANCE.set(ProtocolServer {}) } {
//       unreachable!("protocol server was already initialized")
//     }
//   }
// }

// endpoint!(TestEndpoint, (), (), "testawesome");

// fn spawn_fn() {}
// fn test_endpoint(_ctx: &mut *mut u8, _: (), something: u32) -> () {}

// define_dispatch! {
//   app: ThisPatcher;
//   spawn_fn: spawn_fn;
//   tx_impl: T;
//   spawn_impl: Spawner;
//   context: *mut u8;
//   endpoints: {
//     list: ENDPOINT_LIST;

//     | EndpointTy    | kind    | handler        |
//     | ----------    | ----    | -------        |
//     | TestEndpoint  | spawn   | test_endpoint  |
//   };
//   topics_in: {
//     list: TOPICS_LIST;

//     | TopicTy       | kind    | handler        |
//     | - | - | - |
//   };
//   topics_out: {
//     list: TOPICS_LIST;
//   };

// }

// struct Error;
// impl AsWireTxErrorKind for Error {
//   fn as_kind(&self) -> postcard_rpc::server::WireTxErrorKind {
//     postcard_rpc::server::WireTxErrorKind::Other
//   }
// }

// struct T;
// impl WireTx for T {
//   type Error = Error;

//   async fn send<T: Serialize + ?Sized>(
//     &self,
//     hdr: postcard_rpc::header::VarHeader,
//     msg: &T,
//   ) -> Result<(), Self::Error> {
//     todo!()
//   }

//   async fn send_raw(&self, buf: &[u8]) -> Result<(), Self::Error> {
//     todo!()
//   }

//   async fn send_log_str(
//     &self,
//     kkind: postcard_rpc::header::VarKeyKind,
//     s: &str,
//   ) -> Result<(), Self::Error> {
//     todo!()
//   }

//   async fn send_log_fmt<'a>(
//     &self,
//     kkind: postcard_rpc::header::VarKeyKind,
//     a: core::fmt::Arguments<'a>,
//   ) -> Result<(), Self::Error> {
//     todo!()
//   }
// }

// struct Spawner {}
