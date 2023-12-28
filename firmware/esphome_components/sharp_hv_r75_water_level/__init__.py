import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components.sharp_hv_r75_common import DutyCycleSensor, sharp_hv_r75_ns
from esphome.const import STATE_CLASS_MEASUREMENT, UNIT_PERCENT

CONF_LED_BUCKET_EMPTY = "sensor_bucket_empty"
CONF_LED_BUCKET_LOW = "sensor_bucket_low"
CONF_LED_BUCKET_MID = "sensor_bucket_mid"
CONF_LED_BUCKET_FULL = "sensor_bucket_full"

CONF_MAX_LEVEL_DETECTION_ATTEMPTS = "max_level_detection_attempts"
CONF_DETECTIONS_BEFORE_PUBLISH = "detections_before_publish"

sharp_hv_r75_water_level_ns = sharp_hv_r75_ns.namespace("water_level")
WaterLevelSensor = sharp_hv_r75_water_level_ns.class_("SharpHVR75WaterLevelSensor", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        WaterLevelSensor,
        unit_of_measurement=UNIT_PERCENT,
        icon="mdi:cup-water",
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend({cv.Required(CONF_LED_BUCKET_EMPTY): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_BUCKET_LOW): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_BUCKET_MID): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_BUCKET_FULL): cv.use_id(DutyCycleSensor)})
    .extend({cv.Optional(CONF_MAX_LEVEL_DETECTION_ATTEMPTS, default=10): cv.uint8_t})
    .extend({cv.Optional(CONF_DETECTIONS_BEFORE_PUBLISH, default=2): cv.uint8_t})
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    bucket_empty = await cg.get_variable(config[CONF_LED_BUCKET_EMPTY])
    cg.add(var.set_led_bucket_empty(bucket_empty))

    bucket_low = await cg.get_variable(config[CONF_LED_BUCKET_LOW])
    cg.add(var.set_led_bucket_low(bucket_low))

    bucket_mid = await cg.get_variable(config[CONF_LED_BUCKET_MID])
    cg.add(var.set_led_bucket_mid(bucket_mid))

    bucket_full = await cg.get_variable(config[CONF_LED_BUCKET_FULL])
    cg.add(var.set_led_bucket_full(bucket_full))

    max_level_detection_attempts = config[CONF_MAX_LEVEL_DETECTION_ATTEMPTS]
    cg.add(var.set_max_level_detection_attempts(max_level_detection_attempts))

    detections_before_publish = config[CONF_DETECTIONS_BEFORE_PUBLISH]
    cg.add(var.set_detections_before_publish(detections_before_publish))
