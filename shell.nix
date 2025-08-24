{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  packages = [
    pkgs.coreutils
    pkgs.flex
    pkgs.bison
  ];

  shellHook = ''
    echo "let's B."
  '';
}
