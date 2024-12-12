# shell.nix
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.glslang
    pkgs.vulkan-headers
    pkgs.vulkan-loader
    pkgs.glfw
    pkgs.eigen
    pkgs.cmake
  ];
}
