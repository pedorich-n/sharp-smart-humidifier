{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    systems.url = "github:nix-systems/default";
  };

  outputs = inputs@{ flake-parts, systems, self, ... }: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = import systems;

    perSystem = { system, pkgs, lib, ... }: {
      devShells = {
        default =
          pkgs.mkShell {
            nativeBuildInputs = (with pkgs; [
              bashInteractive
              poetry
            ]) ++ (with pkgs.python3Packages; [
              argcomplete
              venvShellHook
            ]);

            venvDir = "./.venv";

            postVenvCreation = ''
              unset SOURCE_DATE_EPOCH

              poetry env use .venv/bin/python
              poetry install --no-root
            '';

            postShellHook = ''
              eval "$(register-python-argcomplete esphome)"
              poetry env info
            '';
          };
      };
    };
  };
}

