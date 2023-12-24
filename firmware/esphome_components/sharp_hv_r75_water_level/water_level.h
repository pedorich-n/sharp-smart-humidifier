#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/duty_cycle/duty_cycle_sensor.h"

#include "esphome/components/sharp_hv_r75_common/common.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace water_level {

enum WaterLevel { WATER_EMPTY, WATER_LOW, WATER_MID, WATER_FULL, WATER_OVERFLOW };

struct SharpHVR75LedBucketsStore {
  StatusLedValueStore led_bucket_empty;
  StatusLedValueStore led_bucket_low;
  StatusLedValueStore led_bucket_mid;
  StatusLedValueStore led_bucket_full;

  std::string to_string() {
    return str_sprintf("{empty=%.1f%%, low=%.1f%%, mid=%.1f%%, full=%.1f%%}", led_bucket_empty.value,
                       led_bucket_low.value, led_bucket_mid.value, led_bucket_full.value);
  };
};

class SharpHVR75WaterLevelSensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_led_bucket_empty(duty_cycle::DutyCycleSensor *sensor) { led_bucket_empty_ = sensor; };
  void set_led_bucket_low(duty_cycle::DutyCycleSensor *sensor) { led_bucket_low_ = sensor; };
  void set_led_bucket_mid(duty_cycle::DutyCycleSensor *sensor) { led_bucket_mid_ = sensor; };
  void set_led_bucket_full(duty_cycle::DutyCycleSensor *sensor) { led_bucket_full_ = sensor; };
  void set_max_level_detection_attempts(uint8_t value) { max_level_detection_attempts_ = value; };
  void set_detections_before_publish(uint8_t value) {
    detections_before_publish_ = value;
    last_detection_attempts_ = LastStateSeenTimes<WaterLevel>((uint32_t) value);
  }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;
  void update() override;

 protected:
  SharpHVR75LedBucketsStore get_buckets_duty_cycle();
  optional<WaterLevel> try_detect_level(SharpHVR75LedBucketsStore *store);
  float level_enum_to_state(WaterLevel level);

  duty_cycle::DutyCycleSensor *led_bucket_empty_{nullptr};
  duty_cycle::DutyCycleSensor *led_bucket_low_{nullptr};
  duty_cycle::DutyCycleSensor *led_bucket_mid_{nullptr};
  duty_cycle::DutyCycleSensor *led_bucket_full_{nullptr};

  optional<WaterLevel> last_published_level_{};
  uint8_t detections_before_publish_;
  LastStateSeenTimes<WaterLevel> last_detection_attempts_{10};
  uint32_t last_update_{0};

  uint8_t failed_to_compute_level_times_{0};
  uint8_t max_level_detection_attempts_{10};
};
}  // namespace water_level
}  // namespace sharp_hv_r75
}  // namespace esphome
