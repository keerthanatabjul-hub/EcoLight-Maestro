# -my-arm-project
# EcoLight Maestro: Real-Time IoT-Enabled Adaptive Smart Street Light Controller

## Project Overview
EcoLight Maestro is an embedded C project developed for ARM/LPC-based microcontrollers to automate street light control based on time, light intensity, motion detection, and system fault conditions.

The project is designed to reduce energy consumption by controlling street light brightness intelligently instead of keeping lights ON at full intensity throughout the night.

## Key Features
- Automatic street light control based on LDR/light intensity
- Motion-based intensity adjustment
- Real-time scheduling logic
- PWM-based brightness control
- LCD-based status display
- Fault monitoring and indication
- Modular embedded C code structure
- Keil uVision project support

## Modules
- `main.c` - Main program entry
- `app.c` - Application-level logic
- `streetlight.c` - Street light control logic
- `motion.c` - Motion detection handling
- `adc.c` - LDR/ADC reading
- `pwm.c` - Brightness control using PWM
- `lcd.c` - LCD display interface
- `timer.c` - Timer delay and scheduling support
- `fault.c` - Fault detection logic
- `config.c` - Configuration handling

## Tools Used
- Embedded C
- Keil uVision
- ARM LPC Microcontroller
- GitHub

## Future Enhancements
- UART-based debug monitoring
- EEPROM/Flash configuration storage
- Calibration menu for LDR thresholds
- Data logging/event history
- IoT/GSM/cloud monitoring
- AI-based adaptive lighting prediction

## Author
Developed by Tabjul Laxmi Keerthana
