# Sigurd
Propulsion Sensor Unit

# Prerequisites
Follow [zephyr getting started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)

# Build 

```bash
cd zephyr-firmware
west update
cd sigurd
west build -b sigurd app --pristine
west flash
```