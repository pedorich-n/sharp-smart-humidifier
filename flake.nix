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
            export ESPHOME_NOGITIGNORE=true

            python --version
            platformio --version
            echo "ESPHome $(esphome version)"
          '';
        };
      };

      treefmt = {
        projectRootFile = ".root";
        flakeCheck = true;

        settings.formatter = {
          black.options = [ "--line-length=120" ];
        };

        programs = {
          # Nix
          nixpkgs-fmt.enable = true;

          # C++
          clang-format.enable = true;

          # Python
          black.enable = true;
          isort = {
            enable = true;
            profile = "black";
          };

          # YAML, etc
          prettier.enable = true;
          just.enable = true;
        };
      };
    };
  };
}

