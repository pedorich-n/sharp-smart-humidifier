import homeassistant.components.humidifier.const as humidifier_const

DOMAIN = "sharp_smart_humidifier"

CONF_NAME = "name"
CONF_ON_OFF_BUTTON_ID = "on_off_button_id"
CONF_MODE_SELECTOR_ID = "mode_selector_id"
CONF_CURRENT_HUMIDITY_ENTITY_ID = "current_humidity_entity_id"

# Fixed humidity values, as the device does not support humidity control
FIXED_TARGET_HUMIDITY = 50
FIXED_MIN_HUMIDITY = 50
FIXED_MAX_HUMIDITY = 50

MODE_AUTO = "Auto"
MODE_NIGHT = "Night"
MODE_ECO = "Eco"
MODE_FAN_SLOW = "Fan Slow"
MODE_FAN_FAST = "Fan Fast"
MODE_ION = "Ion"

DEVICE_MODE_TO_HUMIDIFIER_MODE = {
    MODE_AUTO: humidifier_const.MODE_AUTO,
    MODE_NIGHT: humidifier_const.MODE_SLEEP,
    MODE_ECO: humidifier_const.MODE_ECO,
    MODE_FAN_SLOW: humidifier_const.MODE_COMFORT,
    MODE_FAN_FAST: humidifier_const.MODE_BOOST,
}

HUMIDIFIER_MODE_TO_DEVICE_MODE = {
    humidifier_const.MODE_AUTO: MODE_AUTO,
    humidifier_const.MODE_SLEEP: MODE_NIGHT,
    humidifier_const.MODE_ECO: MODE_ECO,
    humidifier_const.MODE_COMFORT: MODE_FAN_SLOW,
    humidifier_const.MODE_BOOST: MODE_FAN_FAST,
}