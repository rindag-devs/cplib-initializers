{
  description = "cplib-initializers";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

  outputs =
    { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      devShells = forAllSystems (
        system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          python = pkgs.python3.withPackages (ps: [
            ps.pytest
            ps.pytest-xdist
          ]);
        in
        {
          default = pkgs.mkShell {
            packages = with pkgs; [
              catch2_3
              clang-tools
              cmake
              gcc
              gersemi
              git
              just
              ninja
              python
              ruff
            ];
          };
        }
      );
    };
}
