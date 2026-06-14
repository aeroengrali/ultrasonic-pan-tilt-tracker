# Ultrasonic Pan-Tilt Object-Tracking System (Arduino)

An embedded **scan-and-track** system on Arduino that sweeps for the nearest
object and locks onto it. Three **HC-SR04 ultrasonic range sensors** (left,
centre, right) feed a control loop that drives a **dual-servo pan/tilt** head
to centre the target, with an LED status indicator. When no target is in
range the head returns to a raster search sweep.

**Author:** Ali Murtaza · **Type:** Digital Systems Design Lab project · **Period:** 2024
**Platform:** Arduino Mega 2560 (C/C++, `Servo` library) · 3× HC-SR04 ultrasonic sensors (5 V, 2-400 cm), 2× hobby servos (pan 0-180°, tilt 0-90°), LED.

## How it works
- **Sense:** the three HC-SR04 sensors give left/centre/right distances via trigger/echo timing.
- **Decide:** the loop compares the three ranges to find the bearing of the closest object and whether it is within the tracking window.
- **Act:** the pan servo turns toward the side with the nearest return; the tilt servo holds/adjusts elevation; the LED signals "target acquired."
- **Search:** with no target, the head performs a raster scan across its range until something enters detection distance.

## Files
- `DSD_Project.ino` - complete Arduino sketch (sensor polling, decision logic, servo actuation, search/track state handling).
- A demonstration video of the working hardware is available on request / in the project showcase.

## What this demonstrates
Embedded real-time sensing, multi-sensor fusion for bearing estimation,
closed-loop actuator control on a microcontroller, and a search/track state
machine - all on constrained hardware.

> Note: this is an ultrasonic (sonar) tracker on **Arduino**. It does not use
> STM32 or ESP32.

## License
MIT - see [LICENSE](LICENSE).
