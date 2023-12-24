#pragma once

#include <map>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esphome/components/duty_cycle/duty_cycle_sensor.h"

namespace esphome {
namespace sharp_hv_r75 {

class SharpHVR75CommonComponent : public Component {
 public:
  void setup() override { return; };
  float get_setup_priority() const override { return setup_priority::DATA; };
  void dump_config() override { return; };
};

struct StatusLedValueStore {
  float value;
  float detection_threshold_ = 1.0;

  bool is_active() { return value >= detection_threshold_; };

  // TODO: figure out struct initialization
  static StatusLedValueStore from_sensor(duty_cycle::DutyCycleSensor *sensor, float detection_threshold = 1.0) {
    StatusLedValueStore result;
    result.value = sensor->get_state();
    result.detection_threshold_ = detection_threshold;
    return result;
  };
};

enum OperatingMode { OFF, AUTO, NIGHT, ECO, FAN_SLOW, FAN_FAST, ION };

// clang-format off
const std::map<OperatingMode, std::string> OPERATING_MODE_TO_STRING = {
    {OperatingMode::OFF, "Off"},
    {OperatingMode::AUTO, "Auto"},
    {OperatingMode::NIGHT, "Night"},
    {OperatingMode::ECO, "Eco"},
    {OperatingMode::FAN_SLOW, "Fan Slow"}, 
    {OperatingMode::FAN_FAST, "Fan Fast"},
    {OperatingMode::ION, "Ion"},
};

const std::map<std::string, OperatingMode> STRING_TO_OPERATING_MODE = {
    {"Off", OperatingMode::OFF},
    {"Auto", OperatingMode::AUTO},
    {"Night", OperatingMode::NIGHT},
    {"Eco", OperatingMode::ECO},
    {"Fan Slow", OperatingMode::FAN_SLOW},
    {"Fan Fast", OperatingMode::FAN_FAST},
    {"Ion", OperatingMode::ION},
};
// clang-format on

static optional<OperatingMode> try_get_mode(std::string key) {
  if (STRING_TO_OPERATING_MODE.count(key) == 1) {
    return STRING_TO_OPERATING_MODE.at(key);
  } else {
    return {};
  }
}

template<typename T> class LastStateSeenTimes {
 public:
  LastStateSeenTimes(uint32_t max_seen_times) : max_seen_times_(max_seen_times){};

  optional<T> get_last_state() { return this->last_state_; }
  uint32_t get_seen_times() { return this->seen_times_; };
  bool is_seen_max_times() { return this->seen_times_ == this->max_seen_times_; };
  uint32_t get_max_seen_times() { return this->max_seen_times_; };

  void safe_increment() {
    if (this->seen_times_ < this->max_seen_times_) {
      this->seen_times_++;
    }
  }

  void set_new_state(const T state) {
    this->seen_times_ = 0;
    this->last_state_ = state;
  }

  void set_new_state_or_increment(const T new_state) {
    if (this->last_state_.has_value() && this->last_state_ == new_state) {
      this->safe_increment();
    } else {
      this->set_new_state(new_state);
    }
  }

 private:
  optional<T> last_state_{};
  uint32_t seen_times_{0};
  uint32_t max_seen_times_;
};

}  // namespace sharp_hv_r75
}  // namespace esphome
