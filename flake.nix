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
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [ 
            bashInteractive 
            python3
            platformio-core
            platformio
            esphome
            python3Packages.argcomplete
          ];

          shellHook = ''
            ${lib.getExe pkgs.python3} --version
            ${lib.getExe' pkgs.platformio-core "platformio"} --version
            echo "ESPHome $(${lib.getExe pkgs.esphome} version)"

            eval "$(_PLATFORMIO_COMPLETE=bash_source ${lib.getExe' pkgs.platformio-core "platformio"})"
            eval "$(_PIO_COMPLETE=bash_source ${lib.getExe' pkgs.platformio-core "pio"})"
            eval "$(register-python-argcomplete ${lib.getExe pkgs.esphome})"
          '';
        };
      };
    };
  };
}

