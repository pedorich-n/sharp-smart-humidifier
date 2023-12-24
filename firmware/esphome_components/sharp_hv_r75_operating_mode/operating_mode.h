#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/duty_cycle/duty_cycle_sensor.h"

// TODO: don't use relative path?
#include "esphome/components/sharp_hv_r75_common/common.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode {

struct SharpHVr75CurrentModeStore {
  OperatingMode mode;
  bool is_ion;

  // std::string to_string() {
  //   return str_sprintf("{mode=%s, is_ion=%s}", OPERATING_MODE_TO_STRING.at(this->mode).c_str(),
  //                      this->is_ion ? "true" : "false");
  // }
};

inline bool operator==(const SharpHVr75CurrentModeStore &lhs, const SharpHVr75CurrentModeStore &rhs) {
  return lhs.mode == rhs.mode && lhs.is_ion == rhs.is_ion;
};

inline bool operator!=(const SharpHVr75CurrentModeStore &lhs, const SharpHVr75CurrentModeStore &rhs) {
  return lhs.mode != rhs.mode && lhs.is_ion != rhs.is_ion;
}

struct SharpHVR75LedModesStore {
  StatusLedValueStore led_mode_auto_duty_cycle;
  StatusLedValueStore led_mode_night_duty_cycle;
  StatusLedValueStore led_mode_eco_duty_cycle;
  StatusLedValueStore led_mode_fan_slow_duty_cycle;
  StatusLedValueStore led_mode_fan_fast_duty_cycle;
  StatusLedValueStore led_ion_duty_cycle;

  std::string to_string() {
    return str_sprintf("{auto=%.1f%%, night=%.1f%%, eco=%.1f%%, slow=%.1f%%, fast=%.1f%%, ion=%.1f%%}",
                       led_mode_auto_duty_cycle.value, led_mode_night_duty_cycle.value, led_mode_eco_duty_cycle.value,
                       led_mode_fan_slow_duty_cycle.value, led_mode_fan_fast_duty_cycle.value,
                       led_ion_duty_cycle.value);
  };
};

class SharpHVR75OperatingMode : public text_sensor::TextSensor, public PollingComponent {
 public:
  void set_led_mode_auto(duty_cycle::DutyCycleSensor *sensor) { led_mode_auto_ = sensor; };
  void set_led_mode_night(duty_cycle::DutyCycleSensor *sensor) { led_mode_night_ = sensor; };
  void set_led_mode_eco(duty_cycle::DutyCycleSensor *sensor) { led_mode_eco_ = sensor; };
  void set_led_mode_fan_slow(duty_cycle::DutyCycleSensor *sensor) { led_mode_fan_slow_ = sensor; };
  void set_led_mode_fan_fast(duty_cycle::DutyCycleSensor *sensor) { led_mode_fan_fast_ = sensor; };
  void set_led_ion(duty_cycle::DutyCycleSensor *sensor) { led_ion_ = sensor; };
  void set_max_mode_detection_attempts(uint8_t value) { max_mode_detection_attempts_ = value; };
  void set_detections_before_publish(uint8_t value) {
    detections_before_publish_ = value;
    last_detection_attempts_ = LastStateSeenTimes<SharpHVr75CurrentModeStore>((uint32_t) value);
  }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;
  void update() override;

 protected:
  SharpHVR75LedModesStore get_modes_duty_cycle();
  optional<SharpHVr75CurrentModeStore> try_detect_mode(SharpHVR75LedModesStore *store);

  duty_cycle::DutyCycleSensor *led_mode_auto_{nullptr};
  duty_cycle::DutyCycleSensor *led_mode_night_{nullptr};
  duty_cycle::DutyCycleSensor *led_mode_eco_{nullptr};
  duty_cycle::DutyCycleSensor *led_mode_fan_slow_{nullptr};
  duty_cycle::DutyCycleSensor *led_mode_fan_fast_{nullptr};
  duty_cycle::DutyCycleSensor *led_ion_{nullptr};

  optional<SharpHVr75CurrentModeStore> last_published_mode_{};
  uint8_t detections_before_publish_;
  LastStateSeenTimes<SharpHVr75CurrentModeStore> last_detection_attempts_{10};
  uint32_t last_update_{0};

  uint8_t failed_to_compute_level_times_{0};
  uint8_t max_mode_detection_attempts_{10};
};

}  // namespace operating_mode
}  // namespace sharp_hv_r75
}  // namespace esphome
