// directly lifted from https://github.com/NightProg/cxx_linker <3
use mangler::mangle;
use proc_macro::TokenStream;
use quote::quote;

/// A procedural macro for linking Rust functions with C++ using Itanium name mangling.
#[proc_macro_attribute]
pub fn cxx_export(attr: TokenStream, token_stream: TokenStream) -> TokenStream {
  let func = syn::parse_macro_input!(token_stream as syn::ItemFn);
  let attr = syn::parse_macro_input!(attr as syn::LitStr);
  let mangled = mangle(attr.value());

  quote! {
    #[unsafe(export_name = #mangled)]
    #func
  }
  .into()
}
