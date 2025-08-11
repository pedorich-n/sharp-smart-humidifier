import logging
from typing import Any, List

import voluptuous as vol
from homeassistant.components.humidifier import PLATFORM_SCHEMA, HumidifierDeviceClass, HumidifierEntity
from homeassistant.const import SERVICE_SELECT_OPTION, SERVICE_TURN_OFF, SERVICE_TURN_ON
from homeassistant.core import CALLBACK_TYPE, Event, EventStateChangedData, HomeAssistant, State, callback
from homeassistant.helpers import config_validation as cv
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.event import async_track_state_change_event
from homeassistant.helpers.restore_state import RestoreEntity
from homeassistant.helpers.typing import ConfigType, DiscoveryInfoType

from .const import (
    CONF_CURRENT_HUMIDITY_ENTITY_ID,
    CONF_MODE_SELECTOR_ID,
    CONF_NAME,
    CONF_SWITCH_ID,
    DEVICE_MODE_TO_HUMIDIFIER_MODE,
    FIXED_MAX_HUMIDITY,
    FIXED_MIN_HUMIDITY,
    FIXED_TARGET_HUMIDITY,
    HUMIDIFIER_MODE_TO_DEVICE_MODE,
)

_LOGGER = logging.getLogger(__name__)

PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend(
    {
        vol.Required(CONF_NAME): cv.string,
        vol.Required(CONF_SWITCH_ID): cv.entity_id,
        vol.Required(CONF_MODE_SELECTOR_ID): cv.entity_id,
        vol.Required(CONF_CURRENT_HUMIDITY_ENTITY_ID): cv.entity_id,
    }
)


async def async_setup_platform(
    hass: HomeAssistant,
    config: ConfigType,
    async_add_entities: AddEntitiesCallback,
    discovery_info: DiscoveryInfoType | None = None,
) -> None:
    async_add_entities([SharpSmartHumidifier(hass, config)])


class SharpSmartHumidifier(HumidifierEntity, RestoreEntity):
    def __init__(self, hass: HomeAssistant, config: ConfigType):
        self.hass = hass
        self._config = config

        self._attr_name = config[CONF_NAME]
        self._attr_unique_id = f"sharp_hv_r75_smart_humidifier_{config[CONF_NAME].replace(' ', '_').lower()}"
        self._attr_device_class = HumidifierDeviceClass.HUMIDIFIER

        self._attr_target_humidity = FIXED_TARGET_HUMIDITY
        self._attr_min_humidity = FIXED_MIN_HUMIDITY
        self._attr_max_humidity = FIXED_MAX_HUMIDITY
        self._attr_available_modes = list(DEVICE_MODE_TO_HUMIDIFIER_MODE.values())

        self._attr_is_on = False
        self._attr_current_humidity = None
        self._attr_mode = None

        self._switch_id = config[CONF_SWITCH_ID]
        self._mode_selector_id = config[CONF_MODE_SELECTOR_ID]
        self._current_humidity_entity_id = config[CONF_CURRENT_HUMIDITY_ENTITY_ID]

        self._unsubscribe_listeners: List[CALLBACK_TYPE] = []

    async def async_added_to_hass(self) -> None:
        await super().async_added_to_hass()

        # Restore previous state
        if (last_state := await self.async_get_last_state()) is not None:
            self._attr_is_on = last_state.state == "on"
            if last_state.attributes.get("mode"):
                self._attr_mode = last_state.attributes["mode"]

        # Track state changes for all relevant entities
        self._unsubscribe_listeners.append(async_track_state_change_event(self.hass, [self._switch_id], self._async_switch_state_changed))
        self._unsubscribe_listeners.append(
            async_track_state_change_event(self.hass, [self._mode_selector_id], self._async_mode_state_changed)
        )
        self._unsubscribe_listeners.append(
            async_track_state_change_event(self.hass, [self._current_humidity_entity_id], self._async_humidity_state_changed)
        )

        # Initialize state from entities
        await self._async_init_from_entities()

    async def async_will_remove_from_hass(self) -> None:
        for unsubscribe in self._unsubscribe_listeners:
            unsubscribe()

    # region: Callbacks
    @callback
    def _async_switch_state_changed(self, event: Event[EventStateChangedData]) -> None:
        new_state = event.data.get("new_state")
        if new_state is None:
            return

        self._update_switch_state(new_state)
        self.async_write_ha_state()

    @callback
    def _async_mode_state_changed(self, event: Event[EventStateChangedData]) -> None:
        """Handle mode selector state changes."""
        new_state = event.data.get("new_state")
        if new_state is None:
            return

        self._update_mode_state(new_state)
        self.async_write_ha_state()

    @callback
    def _async_humidity_state_changed(self, event: Event[EventStateChangedData]) -> None:
        """Handle humidity sensor state changes."""
        new_state = event.data.get("new_state")
        if new_state is None or new_state.state in ["unavailable", "unknown"]:
            return

        self._update_humidity_state(new_state)
        self.async_write_ha_state()

    # endregion: Callbacks

    # region: Control Methods
    async def async_turn_on(self, **kwargs: Any) -> None:
        """Turn the humidifier on."""
        try:
            await self.hass.services.async_call("switch", SERVICE_TURN_ON, {"entity_id": self._switch_id}, blocking=True)
            self._attr_is_on = True
            self.async_write_ha_state()
        except Exception as err:
            _LOGGER.error("Failed to turn on %s: %s", self._switch_id, err)

    async def async_turn_off(self, **kwargs: Any) -> None:
        """Turn the humidifier off."""
        try:
            await self.hass.services.async_call("switch", SERVICE_TURN_OFF, {"entity_id": self._switch_id}, blocking=True)
            self._attr_is_on = False
            self.async_write_ha_state()
        except Exception as err:
            _LOGGER.error("Failed to turn off %s: %s", self._switch_id, err)

    async def async_set_humidity(self, humidity: int) -> None:
        """Set target humidity (not supported - fixed at 50%)."""
        _LOGGER.warning("Setting target humidity is not supported - fixed at %d%%", FIXED_TARGET_HUMIDITY)

    async def async_set_mode(self, mode: str) -> None:
        """Set the humidifier mode."""
        if mode not in HUMIDIFIER_MODE_TO_DEVICE_MODE:
            _LOGGER.error("Unsupported mode: %s", mode)
            return

        device_mode = HUMIDIFIER_MODE_TO_DEVICE_MODE[mode]
        try:
            await self.hass.services.async_call(
                "select",
                SERVICE_SELECT_OPTION,
                {
                    "entity_id": self._mode_selector_id,
                    "option": device_mode,
                },
                blocking=True,
            )
            self._attr_mode = mode
            self.async_write_ha_state()
        except Exception as err:
            _LOGGER.error("Failed to set mode %s: %s", mode, err)

    # endregion: Control Methods

    # region: Private
    def _update_switch_state(self, state: State) -> None:
        if state is None:
            return

        self._attr_is_on = state.state == "on"
        self._attr_available = state.state not in ["unavailable", "unknown"]

    def _update_mode_state(self, state: State) -> None:
        if state.state not in DEVICE_MODE_TO_HUMIDIFIER_MODE:
            _LOGGER.debug("Received unsupported mode state: %s", state.state)
            return

        self._attr_mode = DEVICE_MODE_TO_HUMIDIFIER_MODE[state.state]
        self._attr_available = True

    def _update_humidity_state(self, state: State) -> None:
        if state.state in ["unavailable", "unknown"]:
            self._attr_current_humidity = None
            return

        try:
            self._attr_current_humidity = int(float(state.state))
        except (ValueError, TypeError):
            _LOGGER.warning("Invalid humidity value from %s: %s", self._current_humidity_entity_id, state.state)

    async def _async_init_from_entities(self) -> None:
        """Init humidifier state based on tracked entities."""

        # Update power state from switch
        switch_state = self.hass.states.get(self._switch_id)
        if switch_state:
            self._update_switch_state(switch_state)

        # Update mode from selector
        mode_state = self.hass.states.get(self._mode_selector_id)
        if mode_state:
            self._update_mode_state(mode_state)

        # Update current humidity from sensor
        humidity_state = self.hass.states.get(self._current_humidity_entity_id)
        if humidity_state:
            self._update_humidity_state(humidity_state)

        self.async_write_ha_state()

    # endregion: Private
