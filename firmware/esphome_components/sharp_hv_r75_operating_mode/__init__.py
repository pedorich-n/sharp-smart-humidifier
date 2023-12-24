import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC, CONF_ID

# from esphome.components.sharp_hv_r75_common import sharp_hv_r75_ns

CONF_LED_MODE_AUTO = "sensor_mode_auto"
CONF_LED_MODE_NIGHT = "sensor_mode_night"
CONF_LED_MODE_ECO = "sensor_mode_eco"
CONF_LED_MODE_FAN_SLOW = "sensor_mode_fan_slow"
CONF_LED_MODE_FAN_FAST = "sensor_mode_fan_fast"
CONF_LED_ION = "sensor_ion"

CONF_MAX_MODE_DETECTION_ATTEMPTS = "max_mode_detection_attempts"
CONF_DETECTIONS_BEFORE_PUBLISH = "detections_before_publish"

duty_cycle_ns = cg.esphome_ns.namespace("duty_cycle")
DutyCycleSensor = duty_cycle_ns.class_("DutyCycleSensor", sensor.Sensor, cg.PollingComponent)

sharp_hv_r75_ns = cg.esphome_ns.namespace("sharp_hv_r75")
operating_mode_ns = sharp_hv_r75_ns.namespace("operating_mode")
OperatingMode = operating_mode_ns.class_("SharpHVR75OperatingMode", text_sensor.TextSensor, cg.PollingComponent)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(OperatingMode, icon="mdi:air-humidifier", entity_category=ENTITY_CATEGORY_DIAGNOSTIC)
    .extend({cv.Required(CONF_LED_MODE_AUTO): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_MODE_NIGHT): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_MODE_ECO): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_MODE_FAN_SLOW): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_MODE_FAN_FAST): cv.use_id(DutyCycleSensor)})
    .extend({cv.Required(CONF_LED_ION): cv.use_id(DutyCycleSensor)})
    .extend({cv.Optional(CONF_MAX_MODE_DETECTION_ATTEMPTS, default=10): cv.uint8_t})
    .extend({cv.Optional(CONF_DETECTIONS_BEFORE_PUBLISH, default=2): cv.uint8_t})
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)
    # var = cg.new_Pvariable(config[CONF_ID])
    # await cg.register_component(var, config)

    mode_auto = await cg.get_variable(config[CONF_LED_MODE_AUTO])
    cg.add(var.set_led_mode_auto(mode_auto))

    mode_night = await cg.get_variable(config[CONF_LED_MODE_NIGHT])
    cg.add(var.set_led_mode_night(mode_night))

    mode_eco = await cg.get_variable(config[CONF_LED_MODE_ECO])
    cg.add(var.set_led_mode_eco(mode_eco))

    mode_fan_slow = await cg.get_variable(config[CONF_LED_MODE_FAN_SLOW])
    cg.add(var.set_led_mode_fan_slow(mode_fan_slow))

    mode_fan_fast = await cg.get_variable(config[CONF_LED_MODE_FAN_FAST])
    cg.add(var.set_led_mode_fan_fast(mode_fan_fast))

    ion = await cg.get_variable(config[CONF_LED_ION])
    cg.add(var.set_led_ion(ion))

    max_mode_detection_attempts = config[CONF_MAX_MODE_DETECTION_ATTEMPTS]
    cg.add(var.set_max_mode_detection_attempts(max_mode_detection_attempts))

    detections_before_publish = config[CONF_DETECTIONS_BEFORE_PUBLISH]
    cg.add(var.set_detections_before_publish(detections_before_publish))
