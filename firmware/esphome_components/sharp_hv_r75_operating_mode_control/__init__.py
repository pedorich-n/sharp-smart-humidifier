import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select, text_sensor, button
from esphome.components.sharp_hv_r75_common import sharp_hv_r75_ns
from esphome.const import CONF_ID

DEPENDENCIES = ["sharp_hv_r75_operating_mode_select", "sharp_hv_r75_operating_mode"]

CONF_OPERATING_MODE = "sensor_operating_mode"
CONF_OPERATING_MODE_SELECT = "select_operating_mode"
CONF_BUTTON_ON_OFF = "button_on_off"
CONF_BUTTON_MODE = "button_mode"
CONF_MAX_LOOPS_SINCE_ANY_BUTTON_PRESSED = "max_loops_since_any_button_pressed"

operating_mode_control_ns = sharp_hv_r75_ns.namespace("operating_mode_control")
OperatingModeControl = operating_mode_control_ns.class_("SharpHVR75OperatingModeControl", cg.PollingComponent)

operating_mode_ns = sharp_hv_r75_ns.namespace("operating_mode")
OperatingMode = operating_mode_ns.class_("SharpHVR75OperatingMode", text_sensor.TextSensor, cg.PollingComponent)

operating_mode_select_ns = sharp_hv_r75_ns.namespace("operating_mode_select")
OperatingModeSelect = operating_mode_select_ns.class_("SharpHVR75OperatingModeSelect", select.Select, cg.Component)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(OperatingModeControl)})
    .extend({cv.Required(CONF_OPERATING_MODE): cv.use_id(OperatingMode)})
    .extend({cv.Required(CONF_OPERATING_MODE_SELECT): cv.use_id(OperatingModeSelect)})
    .extend({cv.Required(CONF_BUTTON_ON_OFF): cv.use_id(button.Button)})
    .extend({cv.Required(CONF_BUTTON_MODE): cv.use_id(button.Button)})
    .extend({cv.Optional(CONF_MAX_LOOPS_SINCE_ANY_BUTTON_PRESSED, default=10): cv.uint8_t})
    .extend(cv.polling_component_schema("5s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    operating_mode = await cg.get_variable(config[CONF_OPERATING_MODE])
    cg.add(var.set_current_operating_mode_sensor(operating_mode))

    operating_mode_select = await cg.get_variable(config[CONF_OPERATING_MODE_SELECT])
    cg.add(var.set_target_operating_mode_select(operating_mode_select))

    button_on_off = await cg.get_variable(config[CONF_BUTTON_ON_OFF])
    cg.add(var.set_on_off_button(button_on_off))


    button_mode = await cg.get_variable(config[CONF_BUTTON_MODE])
    cg.add(var.set_mode_select_button(button_mode))

    max_loops_since_any_button_pressed = config[CONF_MAX_LOOPS_SINCE_ANY_BUTTON_PRESSED]
    cg.add(var.set_max_loops_since_any_button_pressed(max_loops_since_any_button_pressed))
