{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    clang
    clang-tools
    cmake
    gdb
    gnumake
  ];
}
