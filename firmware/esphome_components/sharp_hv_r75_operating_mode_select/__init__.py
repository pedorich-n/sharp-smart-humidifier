import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.components.sharp_hv_r75_common import sharp_hv_r75_ns
from esphome.const import ENTITY_CATEGORY_CONFIG, ICON_FAN


operating_mode_select_ns = sharp_hv_r75_ns.namespace("operating_mode_select")
OperatingModeSelect = operating_mode_select_ns.class_("SharpHVR75OperatingModeSelect", select.Select, cg.Component)

CONFIG_SCHEMA = select.select_schema(OperatingModeSelect, icon=ICON_FAN, entity_category=ENTITY_CATEGORY_CONFIG).extend(
    cv.COMPONENT_SCHEMA
)


async def to_code(config):
    var = await select.new_select(config, options=[""])  # Options are set from `setup`
    await cg.register_component(var, config)
