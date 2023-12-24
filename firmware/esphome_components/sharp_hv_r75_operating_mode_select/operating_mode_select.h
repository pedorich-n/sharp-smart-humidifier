#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#include "esphome/components/select/select.h"

#include "esphome/components/sharp_hv_r75_common/common.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace operating_mode_select {

class SharpHVR75OperatingModeSelect : public select::Select, public Component {
 public:
  void set_manual_callback(std::function<void(OperatingMode)> &&callback) { manual_callback_ = std::move(callback); };
  void update_state(const std::string &value);

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  std::function<void(OperatingMode)> manual_callback_;
};

}  // namespace operating_mode_select
}  // namespace sharp_hv_r75
}  // namespace esphome
