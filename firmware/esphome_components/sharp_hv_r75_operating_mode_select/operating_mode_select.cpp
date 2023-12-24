#include "operating_mode_select.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode_select {

static const char *const TAG = "sharp_hv_r75_operating_mode_select";

void SharpHVR75OperatingModeSelect::setup() {
  std::vector<std::string> options;
  for (const auto &pair : OPERATING_MODE_TO_STRING) {
    options.push_back(pair.second);
  }

  this->traits.set_options(options);
};

float SharpHVR75OperatingModeSelect::get_setup_priority() const { return setup_priority::DATA; };

void SharpHVR75OperatingModeSelect::dump_config() { LOG_SELECT(TAG, "Sharp HV-R75 Operating Mode Select", this); };

void SharpHVR75OperatingModeSelect::control(const std::string &value) {
  optional<OperatingMode> maybe_new_state = try_get_mode(value);
  if (maybe_new_state.has_value()) {
    this->manual_callback_(maybe_new_state.value());
    this->publish_state(value);
  } else {
    ESP_LOGW(TAG, "Failed to convert value %s to Enum!", value.c_str());
  };
};

void SharpHVR75OperatingModeSelect::update_state(const std::string &value) { this->publish_state(value); };

}  // namespace operating_mode_select
}  // namespace sharp_hv_r75
}  // namespace esphome
