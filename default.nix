{
  pkgs ? import <nixpkgs> { },
}:
pkgs.stdenv.mkDerivation {
  pname = "ssq";
  version = "0.1.0";
  src = ./.;

  nativeBuildInputs = with pkgs; [
    cmake
  ];

  cmakeFlags = [
    "-DBUILD_SHARED_LIBS=ON"
  ];

  meta = with pkgs.lib; {
    description = "A modern Source Server Query protocol library";
    homepage = "https://codeberg.org/mindful-magnolia/ssq";
    license = licenses.mit;
  };
}
