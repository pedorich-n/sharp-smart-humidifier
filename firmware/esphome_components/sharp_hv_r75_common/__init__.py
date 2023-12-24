import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID


duty_cycle_ns = cg.esphome_ns.namespace("duty_cycle")
DutyCycleSensor = duty_cycle_ns.class_("DutyCycleSensor", sensor.Sensor, cg.PollingComponent)


sharp_hv_r75_ns = cg.esphome_ns.namespace("sharp_hv_r75")
CommonComponent = sharp_hv_r75_ns.class_("SharpHVR75CommonComponent", cg.Component)
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(CommonComponent),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
