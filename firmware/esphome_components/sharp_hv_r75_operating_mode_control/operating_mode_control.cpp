#include "operating_mode_control.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode_control {

static const char *const TAG = "sharp_hv_r75_operating_mode_control";

void SharpHVR75OperatingModeControl::setup() {
  ESP_LOGD(TAG, "Setting up Operating Mode Control...");

  // When the new mode gets selected, set `maybe_target_mode_`
  this->target_operating_mode_select_->set_manual_callback([this](OperatingMode new_target_mode) {
    ESP_LOGD(TAG, "In custom callback, updating target_mode to %s",
             OPERATING_MODE_TO_STRING.at(new_target_mode).c_str());
    this->maybe_target_mode_ = new_target_mode;
  });

  // When the current state gets changed via physical button, update the Select's state
  // without calling `select.manual_callback`
  this->current_operating_mode_sensor_->add_on_state_callback(
      [this](std::string current_mode) { this->target_operating_mode_select_->update_state(current_mode); });

  // Set the initial selected option to the current state if available
  if (this->current_operating_mode_sensor_->has_state()) {
    std::string current_mode_text = this->current_operating_mode_sensor_->get_state();
    this->target_operating_mode_select_->update_state(current_mode_text);
  } else {
    ESP_LOGW(TAG, "Failed to get the initial value from %s! Sensor is not ready yet.",
             this->current_operating_mode_sensor_->get_name().c_str());
  }
};

float SharpHVR75OperatingModeControl::get_setup_priority() const { return setup_priority::LATE; };

void SharpHVR75OperatingModeControl::dump_config() {
  ESP_LOGCONFIG(TAG, "Sharp HV-R75 Operating Mode Control");
  LOG_UPDATE_INTERVAL(this);

  const char *prefix = "    ";
  ESP_LOGCONFIG(TAG, "  Inputs:");
  LOG_TEXT_SENSOR(prefix, "Operating Mode Sensor", this->current_operating_mode_sensor_);
  LOG_SELECT(prefix, "Target Mode Select", this->target_operating_mode_select_);
  LOG_BUTTON(prefix, "ON/OFF Button", this->on_off_button_);
  LOG_BUTTON(prefix, "Mode Select Button", this->mode_select_button_);
};

void SharpHVR75OperatingModeControl::update() {
  if (!this->maybe_target_mode_.has_value()) {
    ESP_LOGD(TAG, "No target_mode, skipping update...");
    return;
  }

  optional<OperatingMode> maybe_current_mode = this->try_get_current_mode();
  if (!maybe_current_mode.has_value()) {
    ESP_LOGW(TAG, "No current_mode, skipping update...");
    return;
  }

  OperatingMode current_mode = maybe_current_mode.value();
  OperatingMode target_mode = this->maybe_target_mode_.value();

  ESP_LOGD(TAG, "Current Mode: %s, Target Mode: %s", OPERATING_MODE_TO_STRING.at(current_mode).c_str(),
           OPERATING_MODE_TO_STRING.at(target_mode).c_str());

  // Target mode reached; Nothing to do
  if (current_mode == target_mode) {
    ESP_LOGD(TAG, "Target mode %s reached.", OPERATING_MODE_TO_STRING.at(target_mode).c_str());
    this->reset_state_after_update();
    return;
  }

  // Target mode hasn't been reached yet...
  // One of the buttons was pressed some loops ago, but the mode hasn't changed yet
  if (this->maybe_previous_loop_operating_mode_ == current_mode) {
    ESP_LOGW(TAG, "On/Off or Mode button was pressed '%i' loops ago, but the target_mode '%s' hasn't been reached yet",
             this->loops_since_any_button_pressed_, OPERATING_MODE_TO_STRING.at(target_mode).c_str());
    if (this->loops_since_any_button_pressed_ < this->max_loops_since_any_button_pressed_) {
      this->loops_since_any_button_pressed_++;
    } else {
      ESP_LOGW(TAG, "Waited for %i loops since last button press, but mode hasn't changed; Giving up trying",
               this->loops_since_any_button_pressed_);
      this->reset_state_after_update();
      this->target_operating_mode_select_->publish_state(OPERATING_MODE_TO_STRING.at(current_mode));
    }

    return;
  }

  this->maybe_previous_loop_operating_mode_ = current_mode;
  // If the device needs to be turned on or off, press "On/Off" button. Otherwise press "Mode" button
  if ((current_mode == OperatingMode::OFF && target_mode != OperatingMode::OFF) ||
      (current_mode != OperatingMode::OFF && target_mode == OperatingMode::OFF)) {
    ESP_LOGD(TAG, "Pressing On/Off button...");
    this->on_off_button_->press();
  } else {
    ESP_LOGD(TAG, "Pressing Mode button...");
    this->mode_select_button_->press();
  };

  this->loops_since_any_button_pressed_ = 1;
};

void SharpHVR75OperatingModeControl::reset_state_after_update() {
  this->maybe_target_mode_.reset();
  this->maybe_previous_loop_operating_mode_.reset();
  this->loops_since_any_button_pressed_ = 0;
};

optional<OperatingMode> SharpHVR75OperatingModeControl::try_get_current_mode() {
  if (this->current_operating_mode_sensor_->has_state()) {
    std::string current_mode_text = this->current_operating_mode_sensor_->get_state();
    ESP_LOGD(TAG, "Got state from the sensor: %s", current_mode_text.c_str());
    optional<OperatingMode> maybe_current_mode = try_get_mode(current_mode_text);

    return maybe_current_mode;
  } else {
    ESP_LOGW(TAG, "No state in %s!", this->current_operating_mode_sensor_->get_name().c_str());
    return {};
  }
};

}  // namespace operating_mode_control
}  // namespace sharp_hv_r75
}  // namespace esphome
