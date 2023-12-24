{
  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
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
            just
            python3
            python3Packages.argcomplete

            esphome
            platformio-core
            platformio
          ];

          shellHook = ''
            export INSIDE_NIX_DEVELOP=true
            ${lib.getExe pkgs.python3} --version
            ${lib.getExe pkgs.platformio-core} --version
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

