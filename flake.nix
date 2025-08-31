{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    fenix = {
      url = "github:nix-community/fenix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    self.submodules = true;
  };
  outputs = {
    self,
    nixpkgs,
    flake-utils,
    fenix,
  }:
    flake-utils.lib.eachDefaultSystem
    (
      system: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [
            fenix.overlays.default
          ];
        };
        build = import ./sys/nix pkgs;
      in
        with pkgs; rec {
          formatter = pkgs.alejandra;
          devShells.default = mkShell.override {stdenv = stdenvNoCC;} {
            buildInputs =
              build.dependencies
              ++ [
                inetutils
                pkgs.fenix.complete.toolchain
              ];
            CC = "clang";
            CXX = "clang++";
          };
        }
    );
}
