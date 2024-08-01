#include "operating_mode.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode {

static const char *const TAG = "sharp_hv_r75_operating_mode";

void SharpHVR75OperatingMode::setup() {};

float SharpHVR75OperatingMode::get_setup_priority() const { return setup_priority::DATA; };

void SharpHVR75OperatingMode::dump_config() {
  LOG_TEXT_SENSOR("", "Sharp HV-R75 Operating Mode:", this);

  const char *prefix = "    ";
  ESP_LOGCONFIG(TAG, "  Inputs:");
  LOG_SENSOR(prefix, "LED3, Mode Auto", this->led_mode_auto_);
  LOG_SENSOR(prefix, "LED4, Mode Night", this->led_mode_night_);
  LOG_SENSOR(prefix, "LED5, Mode Eco", this->led_mode_eco_);
  LOG_SENSOR(prefix, "LED2, Mode Fan Slow", this->led_mode_fan_slow_);
  LOG_SENSOR(prefix, "LED13, Mode Fan Fast", this->led_mode_fan_fast_);
  LOG_SENSOR(prefix, "LED6, Ion", this->led_ion_);

  LOG_UPDATE_INTERVAL(this);
};

void SharpHVR75OperatingMode::update() {
  const uint32_t now = micros();
  SharpHVR75LedModesStore store = this->get_modes_duty_cycle();
  optional<SharpHVr75CurrentModeStore> maybe_detected_mode = this->try_detect_mode(&store);

  if (maybe_detected_mode.has_value()) {
    SharpHVr75CurrentModeStore detected_mode = maybe_detected_mode.value();
    std::string new_state = OPERATING_MODE_TO_STRING.at(detected_mode.mode);
    ESP_LOGD(TAG, "Detected mode: %s", new_state.c_str());

    if (this->status_has_error() || this->status_has_warning()) {
      this->status_clear_error();
      this->status_clear_warning();

      this->failed_to_compute_level_times_ = 0;
    }

    this->last_detection_attempts_.set_new_state_or_increment(detected_mode);

    if (!this->last_detection_attempts_.is_seen_max_times()) {
      ESP_LOGD(TAG, "Last %i detected modes aren't the same, skipping publish_state...",
               this->last_detection_attempts_.get_max_seen_times());
      return;
    }

    if (this->last_published_mode_.has_value() && this->last_published_mode_ == detected_mode) {
      ESP_LOGD(TAG, "Mode hasn't changed since last update, skipping publish_state...");
      return;
    }

    ESP_LOGD(TAG, "Publishing new mode: %s", new_state.c_str());
    this->publish_state(new_state);

    this->last_published_mode_ = detected_mode;
    this->last_update_ = now;

  } else {
    if (this->failed_to_compute_level_times_ >= this->max_mode_detection_attempts_) {
      ESP_LOGE(TAG, "Failed to detect operating mode for state %s for %i+ times!", store.to_string().c_str(),
               this->failed_to_compute_level_times_);
      this->status_set_error();
      this->has_state_ = false;
    } else {
      this->failed_to_compute_level_times_++;
      ESP_LOGI(TAG, "Failed to detect operating mode for state %s!", store.to_string().c_str());
    }
  };
}

SharpHVR75LedModesStore SharpHVR75OperatingMode::get_modes_duty_cycle() {
  return SharpHVR75LedModesStore{
      .led_mode_auto_duty_cycle = StatusLedValueStore::from_sensor(this->led_mode_auto_),
      .led_mode_night_duty_cycle = StatusLedValueStore::from_sensor(this->led_mode_night_),
      .led_mode_eco_duty_cycle = StatusLedValueStore::from_sensor(this->led_mode_eco_),
      .led_mode_fan_slow_duty_cycle = StatusLedValueStore::from_sensor(this->led_mode_fan_slow_),
      .led_mode_fan_fast_duty_cycle = StatusLedValueStore::from_sensor(this->led_mode_fan_fast_),
      .led_ion_duty_cycle = StatusLedValueStore::from_sensor(this->led_ion_)};
};

optional<SharpHVr75CurrentModeStore> SharpHVR75OperatingMode::try_detect_mode(SharpHVR75LedModesStore *state) {
  bool is_auto = state->led_mode_auto_duty_cycle.is_active();
  bool is_night = state->led_mode_night_duty_cycle.is_active();
  bool is_eco = state->led_mode_eco_duty_cycle.is_active();
  bool is_slow = state->led_mode_fan_slow_duty_cycle.is_active();
  bool is_fast = state->led_mode_fan_fast_duty_cycle.is_active();
  bool is_ion = state->led_ion_duty_cycle.is_active();

  optional<OperatingMode> mode = {};

  if (is_auto && !is_night && !is_eco && !is_slow && !is_fast) {
    mode = OperatingMode::AUTO;
  } else if (!is_auto && is_night && !is_eco && !is_slow && !is_fast) {
    mode = OperatingMode::NIGHT;
  } else if (!is_auto && !is_night && is_eco && !is_slow && !is_fast) {
    mode = OperatingMode::ECO;
  } else if (!is_auto && !is_night && !is_eco && is_slow && !is_fast) {
    mode = OperatingMode::FAN_SLOW;
  } else if (!is_auto && !is_night && !is_eco && !is_slow && is_fast) {
    mode = OperatingMode::FAN_FAST;
  } else if (!is_auto && !is_night && !is_eco && !is_slow && !is_fast && is_ion) {
    mode = OperatingMode::ION;
  } else if (!is_auto && !is_night && !is_eco && !is_slow && !is_fast && !is_ion) {
    mode = OperatingMode::OFF;
  } else {
    mode = {};
  }

  if (mode.has_value()) {
    return SharpHVr75CurrentModeStore{.mode = mode.value(), .is_ion = is_ion};
  } else {
    return {};
  }
};

}  // namespace operating_mode
}  // namespace sharp_hv_r75
}  // namespace esphome
