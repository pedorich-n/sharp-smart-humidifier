export FIRMWARE_FILENAME := "humidifier_s2.yaml"
develop_command := 'nix develop -c bash -c "cd firmware && ${COMMAND}"'

develop:
    nix develop

compile:
    COMMAND="esphome compile ${FIRMWARE_FILENAME}" && {{ develop_command }}

run:
    COMMAND="esphome --substitution git_hash $(git describe --tags --always --dirty) run ${FIRMWARE_FILENAME}" && {{ develop_command }}

clean-mqtt:
    COMMAND="esphome clean-mqtt ${FIRMWARE_FILENAME}" && {{ develop_command }}
