{
  description = "An up-to-date Source Server Query protocol library written in C99";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        packages.default = self.packages.${system}.libssq;

        packages.libssq = pkgs.stdenv.mkDerivation {
          pname = "libssq";
          version = "3.0.0";
          src = self;

          nativeBuildInputs = [ pkgs.cmake ];

          cmakeFlags = [ "-DBUILD_SHARED_LIBS=ON" ];

          meta = with pkgs.lib; {
            description = "An up-to-date Source Server Query protocol library written in C99";
            homepage = "https://github.com/BinaryAlien/libssq";
            license = licenses.mit;
          };
        };

        devShells.default = pkgs.mkShell {
          packages = [ pkgs.ccls ] ++ self.packages.${system}.libssq.nativeBuildInputs;
        };
      }
    );
}
