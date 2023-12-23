{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    systems.url = "github:nix-systems/default";

    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
  };

  outputs = inputs@{ flake-parts, systems, self, ... }: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = import systems;
    imports = [ inputs.treefmt-nix.flakeModule ];

    perSystem = { system, pkgs, lib, ... }: {
      devShells = {
        default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            bashInteractive
            git
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

            eval "$(register-python-argcomplete esphome)"
          '';
        };
      };

      treefmt = {
        projectRootFile = ".root";
        flakeCheck = true;

        settings.formatter.just = {
          command = pkgs.just;
          options = [
            "--fmt"
            "--unstable"
            "--justfile"
          ];
          includes = [ "justfile" ];
        };

        programs = {
          # Nix
          nixpkgs-fmt.enable = true;

          # Other
          prettier.enable = true;
        };
      };
    };

  };
}

