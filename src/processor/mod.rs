mod infra;

use alloc::{vec::Vec, vec};
use horizon_dbg_protocol::frame::Header;
use zerocopy::{FromBytes, IntoBytes};

use crate::processor::infra::read;

async fn process() {
  let mut buffer: Vec<u8> = vec![0; 0x10000];
  loop {
    let mut header = [0u8; size_of::<Header>()];
    read(header.as_mut_bytes()).await;
    let header = Header::ref_from_bytes(&header).unwrap();

    read(&mut buffer[..header.length as usize]);
    
  };
}
