use std::env::current_dir;

fn main() {
  let loc = current_dir().unwrap().join("lib/std");
  cxx_build::bridge("src/processor/infra.rs")
    .std("c++23")
    .flags([
      loc.join("llvm-project/build/include/c++/v1"),
      loc.join("musl/include"),
      loc.join("musl/arch/generic"),
      loc.join("musl/arch/aarch64"),
      loc.join("musl/obj/include"),
    ])
    .compile("cxxbridge-demo");
}
