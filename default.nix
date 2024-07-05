{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  pname = "libssq";
  version = "3.0.1";
  src = ./.;

  nativeBuildInputs = with pkgs; [ cmake gtest ];

  cmakeFlags = [ "-DBUILD_SHARED_LIBS=ON" ];

  meta = with pkgs.lib; {
    description = "An up-to-date Source Server Query protocol library written in C99";
    homepage = "https://github.com/BinaryAlien/libssq";
    license = licenses.mit;
  };
}
