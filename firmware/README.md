# Firmware

## Components

- [sharp_hv_r75_common](esphome_components/sharp_hv_r75_common/) - common library used by other `sharp_hv_r75` components.
- [sharp_hv_r75_operating_mode](esphome_components/sharp_hv_r75_operating_mode/) - detects current operating mode based on `duty_cycle` sensors connected to humidifier's LEDs.
- [sharp_hv_r75_water_level](esphome_components/sharp_hv_r75_water_level/) - detects current water level based on `duty_cycle` sensors connected to humidifier's LEDs.
- [sharp_hv_r75_operating_mode_select](esphome_components/sharp_hv_r75_operating_mode_select/) - `select` component with possible operating modes. Almost no custom logic.
- [sharp_hv_r75_operating_mode_control](esphome_components/sharp_hv_r75_operating_mode_control/) - "smart" controlling component. Based on the `sharp_hv_r75_operating_mode` and `button` components it can switch through operating modes until it reaches the target mode set by `sharp_hv_r75_operating_mode_select`.

</br>

> [!IMPORTANT]  
> Enabling fallback AP and/or Captive Portal causes ESPHome stability issues.
> See: https://community.home-assistant.io/t/cant-stop-esphome-restarting-with-weak-wifi/453821/21
