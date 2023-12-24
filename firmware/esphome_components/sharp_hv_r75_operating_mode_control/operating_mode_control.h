#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esphome/components/button/button.h"

#include "esphome/components/sharp_hv_r75_common/common.h"
#include "esphome/components/sharp_hv_r75_operating_mode/operating_mode.h"
#include "esphome/components/sharp_hv_r75_operating_mode_select/operating_mode_select.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode_control {

class SharpHVR75OperatingModeControl : public PollingComponent {
 public:
  // clang-format off
  void set_current_operating_mode_sensor(operating_mode::SharpHVR75OperatingMode *sensor) { current_operating_mode_sensor_ = sensor; };
  void set_target_operating_mode_select(operating_mode_select::SharpHVR75OperatingModeSelect *select) { target_operating_mode_select_ = select; };
  // clang-format on
  void set_on_off_button(button::Button *button) { on_off_button_ = button; };
  void set_mode_select_button(button::Button *button) { mode_select_button_ = button; };
  void set_max_loops_since_any_button_pressed(uint8_t value) { max_loops_since_any_button_pressed_ = value; };

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;
  void update() override;

 protected:
  optional<OperatingMode> try_get_current_mode();
  void reset_state_after_update();

  operating_mode::SharpHVR75OperatingMode *current_operating_mode_sensor_{nullptr};
  operating_mode_select::SharpHVR75OperatingModeSelect *target_operating_mode_select_{nullptr};
  optional<OperatingMode> maybe_target_mode_{};

  button::Button *on_off_button_{nullptr};
  button::Button *mode_select_button_{nullptr};

  optional<OperatingMode> maybe_previous_loop_operating_mode_{};

  uint8_t loops_since_any_button_pressed_{0};
  uint8_t max_loops_since_any_button_pressed_;

  // uint32_t loops_passed_{0};
};

}  // namespace operating_mode_control
}  // namespace sharp_hv_r75
}  // namespace esphome
