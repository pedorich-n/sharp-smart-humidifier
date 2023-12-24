firmware_file := 'firmware/humidifier_s2.yaml'
esphome_command_in_nix := replace('esphome %CMD% %FILE%', '%FILE%', firmware_file)
esphome_command_with_dev := replace('nix develop -c bash -c "%DEV_CMD%"', '%DEV_CMD%', esphome_command_in_nix)
esphome_command := if env_var_or_default('INSIDE_NIX_DEVELOP', 'false') == 'true' { esphome_command_in_nix } else { esphome_command_with_dev }
git_rev := `git describe --tags --always --dirty`
git_rev_substitution := replace('--substitution "git_rev" "%GIT_REV%"', '%GIT_REV%', git_rev)

dev:
    nix develop

compile:
    {{ replace(esphome_command, '%CMD%', git_rev_substitution + ' compile') }}

run:
    {{ replace(esphome_command, '%CMD%', git_rev_substitution + ' run') }}

clean-mqtt:
    {{ replace(esphome_command, '%CMD%', 'clean-mqtt') }}

logs:
    {{ replace(esphome_command, '%CMD%', 'logs') }}
