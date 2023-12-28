#include "water_level.h"

namespace esphome {
namespace sharp_hv_r75 {
namespace water_level {

static const char *const TAG = "sharp_hv_r75_water_level";

void SharpHVR75WaterLevelSensor::setup(){
    // ESP_LOGCONFIG(TAG, "Setting up Sharp HV-R75 Water Level '%s'...", this->get_name().c_str());
};

float SharpHVR75WaterLevelSensor::get_setup_priority() const { return setup_priority::DATA; };

void SharpHVR75WaterLevelSensor::dump_config() {
  LOG_SENSOR("", "Sharp HV-R75 Water Level:", this);

  const char *prefix = "    ";
  ESP_LOGCONFIG(TAG, "  Inputs:");
  LOG_SENSOR(prefix, "LED14, Bucket Empty", this->led_bucket_empty_);
  LOG_SENSOR(prefix, "LED15, Bucket 25%", this->led_bucket_low_);
  LOG_SENSOR(prefix, "LED16, Bucket 50%", this->led_bucket_mid_);
  LOG_SENSOR(prefix, "LED9, Bucket Full", this->led_bucket_full_);

  LOG_UPDATE_INTERVAL(this);
};

void SharpHVR75WaterLevelSensor::update() {
  const uint32_t now = micros();
  SharpHVR75LedBucketsStore store = this->get_buckets_duty_cycle();
  optional<WaterLevel> maybe_detected_level = this->try_detect_level(&store);

  if (maybe_detected_level.has_value()) {
    WaterLevel detected_level = maybe_detected_level.value();

    float new_state = this->level_enum_to_state(detected_level);
    if (this->status_has_error() || this->status_has_warning()) {
      this->status_clear_error();
      this->status_clear_warning();

      this->failed_to_compute_level_times_ = 0;
    }

    this->last_detection_attempts_.set_new_state_or_increment(detected_level);

    if (!this->last_detection_attempts_.is_seen_max_times()) {
      ESP_LOGD(TAG, "Last %i detected levels aren't the same, skipping publish_state...",
               this->last_detection_attempts_.get_max_seen_times());
      return;
    }

    if (this->last_published_level_.has_value() && this->last_published_level_ == detected_level) {
      ESP_LOGD(TAG, "Level hasn't changed since last update, skipping publish_state...");
      return;
    }

    ESP_LOGD(TAG, "Publishing new level: %f", new_state);
    this->publish_state(new_state);

    this->last_published_level_ = detected_level;
    this->last_update_ = now;

  } else {
    if (this->failed_to_compute_level_times_ >= this->max_level_detection_attempts_) {
      ESP_LOGE(TAG, "Failed to detect water level for state %s for %i+ times!", store.to_string().c_str(),
               this->failed_to_compute_level_times_);
      this->status_set_error();
      this->has_state_ = false;
    } else {
      this->failed_to_compute_level_times_++;
      ESP_LOGI(TAG, "Failed to detect water level for state %s!", store.to_string().c_str());
    }
  };
};

optional<WaterLevel> SharpHVR75WaterLevelSensor::try_detect_level(SharpHVR75LedBucketsStore *state) {
  bool is_empty = state->led_bucket_empty.is_active();
  bool is_low = state->led_bucket_low.is_active();
  bool is_mid = state->led_bucket_mid.is_active();
  bool is_full = state->led_bucket_full.is_active();

  if (is_empty && !is_low && !is_mid && !is_full) {
    return WaterLevel::WATER_EMPTY;
  } else if (!is_empty && is_low && !is_mid && !is_full) {
    return WaterLevel::WATER_LOW;
  } else if (!is_empty && !is_low && is_mid && !is_full) {
    return WaterLevel::WATER_MID;
  } else if (!is_empty && !is_low && !is_mid && is_full) {
    return WaterLevel::WATER_FULL;
  } else if (is_empty && is_low && is_mid && is_full) {
    return WaterLevel::WATER_OVERFLOW;
  } else {
    return {};
  }
};

SharpHVR75LedBucketsStore SharpHVR75WaterLevelSensor::get_buckets_duty_cycle() {
  return SharpHVR75LedBucketsStore{
      .led_bucket_empty = StatusLedValueStore::from_sensor(this->led_bucket_empty_, 0.5),
      .led_bucket_low = StatusLedValueStore::from_sensor(this->led_bucket_low_, 0.5),
      .led_bucket_mid = StatusLedValueStore::from_sensor(this->led_bucket_mid_, 0.5),
      .led_bucket_full = StatusLedValueStore::from_sensor(this->led_bucket_full_, 0.5),
  };
};

float SharpHVR75WaterLevelSensor::level_enum_to_state(WaterLevel level) {
  // clang-format off
  switch (level) {
    case WaterLevel::WATER_EMPTY: return 0.0;
    case WaterLevel::WATER_LOW: return 25.0;
    case WaterLevel::WATER_MID: return 50.0;
    case WaterLevel::WATER_FULL: return 100.0;
    case WaterLevel::WATER_OVERFLOW: return 101.0;
    default: return -1;
  }
  // clang-format on
}

}  // namespace water_level
}  // namespace sharp_hv_r75
}  // namespace esphome
