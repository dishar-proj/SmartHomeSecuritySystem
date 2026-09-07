# Smart Home Security System

> A low-cost, multi-sensor home security prototype that combines light, distance, and pressure sensing with ESP32-based sensor fusion and mobile alerts.

The Smart Home Security System was developed to explore how multiple low-cost sensors can work together to improve intrusion detection while reducing reliance on a single sensing method.

The system combines a light-dependent resistor (LDR), HC-SR04 ultrasonic sensor, force-sensitive resistor (FSR), analog signal-processing circuitry, an ESP32 microcontroller, and IoT connectivity through Blynk. Rather than treating each sensor independently, the system fuses their outputs to generate a more reliable intrusion estimate and send real-time alerts to a mobile device.

Technical Report: https://docs.google.com/document/d/1ALwpuyzm_y-HGJbWGW_1bsQN4a0hkKHCoau3NXK7akg/edit?usp=sharing 

## The challenge

Many home security systems rely on a single sensing method, such as motion detection, magnetic door sensors, or cameras.

A single-sensor approach can create reliability problems when environmental conditions interfere with detection. This project explored whether several inexpensive sensors measuring different physical properties could be combined into a more robust entry-monitoring system.

The design needed to:

- Detect potential entry using multiple sensing methods
- Minimize false triggers from environmental noise
- Provide redundancy if one sensor fails to detect an intruder
- Process both analog and digital sensor signals
- Combine sensor readings into a single intrusion estimate
- Alert the homeowner remotely
- Remain relatively low-cost and simple to reproduce

## Our solution

The Smart Home Security System uses three complementary sensors positioned around an entryway:

- **Light-dependent resistor (LDR)** — Detects when a light beam across the entrance is interrupted
- **HC-SR04 ultrasonic sensor** — Detects a decrease in measured distance as a person enters the sensing area
- **Force-sensitive resistor (FSR)** — Detects pressure caused by physical contact or body weight

Each sensor measures a different physical property, providing multiple indicators of human presence.

The signals are processed using analog circuitry and an ESP32 microcontroller. The ESP32 converts sensor outputs into individual detection states, combines them using a Kalman-filter-based sensor-fusion algorithm, and determines whether the overall confidence is high enough to trigger an alarm.

When an intrusion is detected, the system activates a local buzzer and updates the Blynk mobile interface with sensor status and a push notification.

## System architecture

```text
LDR Break-Beam Sensor
        |
        v
Wheatstone Bridge
        |
        v
Differential Amplifier
        |
        v
Low-Pass Filter
        |
        +-------------------+
                            |
HC-SR04 Ultrasonic Sensor   |
        |                   |
        v                   |
Distance + Hysteresis       |
        |                   |
        +-------------------+
                            |
FSR Pressure Sensor         |
        |                   |
        v                   |
Voltage Divider             |
        |                   |
        v                   |
Low-Pass Filter             |
        |                   |
        v                   |
Buffer Amplifier            |
        |                   |
        +-------------------+
                            |
                            v
                         ESP32
                            |
                            v
                    Sensor Fusion
                       (Kalman)
                            |
                            v
                   Intrusion Decision
                      /          \
                     v            v
                  Buzzer       Blynk IoT
                                  |
                                  v
                           Mobile Alert
```

## Features

- **Multi-sensor intrusion detection** — Combines optical, distance, and force sensing instead of relying on a single input.
- **Sensor redundancy** — Allows the system to continue detecting potential entry even if one sensing method is unreliable in a particular situation.
- **Analog signal processing** — Uses Wheatstone bridges, voltage dividers, operational amplifiers, and low-pass filters to condition raw sensor outputs.
- **Kalman-filter sensor fusion** — Combines individual sensor detection states into a unified intrusion estimate.
- **False-trigger reduction** — Uses filtering, hysteresis, threshold tuning, and multi-sensor logic to improve system stability.
- **ESP32-based control** — Handles ADC conversion, sensor logic, fusion, alarm control, and network communication.
- **IoT monitoring** — Sends real-time sensor states and intrusion alerts through the Blynk mobile platform.
- **Local alarm output** — Activates a buzzer when the sensor-fusion threshold indicates an intrusion.
- **Low-cost design** — Uses inexpensive sensors and commonly available embedded components.

## Sensor design

### Light-dependent resistor

The LDR is used as part of a break-beam system.

A red light source is directed toward the sensor during normal operation. When a person interrupts the beam, the resistance of the LDR changes.

The signal-processing chain includes:

```text
Light Beam
    |
    v
LDR
    |
    v
Wheatstone Bridge
    |
    v
Differential Amplifier
    |
    v
Low-Pass Filter
    |
    v
ESP32 ADC
```

The low-pass filter reduces the effect of rapid fluctuations such as brief changes in ambient lighting.

A calibrated voltage threshold is then used by the ESP32 to determine whether the beam has been interrupted.

### Ultrasonic sensor

The HC-SR04 provides a second, non-contact method of detecting a person near the entryway.

The sensor sends an ultrasonic pulse and measures the time required for the reflected signal to return.

Distance is calculated from the pulse travel time.

A voltage divider protects the ESP32 by reducing the sensor's 5 V echo signal before it reaches the 3.3 V GPIO input.

The software uses separate detection and clearing thresholds to introduce hysteresis and reduce rapid switching around the boundary condition.

### Force-sensitive resistor

The FSR adds a third sensing method by detecting physical pressure.

Its resistance changes when force is applied. The signal is processed using:

```text
FSR
 |
 v
Voltage Divider
 |
 v
Low-Pass Filter
 |
 v
Buffer Amplifier
 |
 v
ESP32 ADC
```

The low-pass filter helps reduce false triggers caused by vibration or brief changes in applied pressure.

A voltage threshold is then used to determine whether sufficient force has been detected.

## Sensor fusion

Instead of triggering the alarm directly from any single sensor, the system combines all three measurements using a Kalman-filter-based fusion algorithm.

Each sensor contributes a binary detection state:

```text
LDR         -> ldrDetect
Ultrasonic  -> usDetect
FSR         -> fsrDetect
```

The Kalman filter combines these readings while accounting for measurement uncertainty.

The FSR is assigned slightly higher confidence because it measures direct physical contact, while the LDR and ultrasonic sensor provide complementary non-contact measurements.

The resulting state estimate is compared against an intrusion threshold.

If the estimate exceeds the trigger threshold:

```text
Alarm ON
```

If the estimate later drops below a lower clearing threshold:

```text
Alarm OFF
```

Using separate trigger and clear thresholds adds hysteresis and helps prevent the system from rapidly switching states.

## IoT monitoring

The system uses Blynk to provide remote monitoring through a mobile device.

The application displays the status of each sensor:

- LDR detection
- Ultrasonic detection
- FSR detection

It also displays the overall alarm state.

When an intrusion is detected, the system:

- Changes the alarm indicator
- Updates the mobile dashboard
- Sends a push notification
- Activates the local buzzer

This allows the homeowner to monitor the system remotely rather than relying only on a local alarm.

## Technology

- ESP32
- Arduino IDE
- C / C++
- Blynk IoT
- HC-SR04 Ultrasonic Sensor
- Light-Dependent Resistor
- Force-Sensitive Resistor
- LM324 Operational Amplifier
- Wheatstone Bridge
- Voltage Dividers
- RC Low-Pass Filters
- Kalman Filtering
- ADC Signal Processing
- Embedded Systems
- Sensor Fusion
- IoT

## Hardware

Core components include:

- ESP32 microcontroller
- HC-SR04 ultrasonic sensor
- LDR
- Force-sensitive resistor
- Red light source
- LM324 operational amplifier
- Magnetic buzzer
- 7805 voltage regulator
- 9 V battery
- Resistors and capacitors
- Breadboards and jumper wires

The complete prototype was designed with an estimated component cost of approximately **$40.75**.

## Getting started

Clone the repository:

```bash
git clone https://github.com/dishar-proj/SmartHomeSecuritySystem.git
cd SmartHomeSecuritySystem
```

Open the primary microcontroller file in the Arduino IDE.

Before uploading the program, configure the Blynk credentials in the source code:

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

Then configure the Wi-Fi credentials:

```cpp
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
```

Connect the ESP32 and corresponding sensor circuitry according to the hardware configuration.

Upload the program to the ESP32 through the Arduino IDE.

Once connected, the system will continuously:

1. Read the LDR
2. Measure ultrasonic distance
3. Read the FSR
4. Convert raw sensor data into detection states
5. Fuse the sensor readings
6. Determine the alarm state
7. Update the Blynk dashboard
8. Trigger an alert if intrusion confidence exceeds the threshold

## GPIO configuration

| GPIO | Configuration | Function |
| --- | --- | --- |
| GPIO 36 | ADC | LDR output |
| GPIO 13 | Output | HC-SR04 trigger |
| GPIO 14 | Input | HC-SR04 echo |
| GPIO 32 | ADC | FSR output |
| GPIO 5 | Output | Alarm buzzer |

## Project results

The completed prototype successfully demonstrated multi-sensor intrusion detection across different lighting conditions, distances, and physical interactions.

Each sensing method contributed a different form of information:

- The LDR detected interruption of the light beam
- The ultrasonic sensor detected changes in distance
- The FSR detected applied pressure
- The sensor-fusion algorithm combined all three into a single intrusion estimate

The redundant design helped compensate for limitations in individual sensors.

For example, if an intruder avoided stepping on the FSR, the elevated LDR and ultrasonic sensors could still detect movement through the entrance.

## Limitations

Several limitations were identified during testing:

- Ambient lighting can affect LDR readings
- A person can potentially avoid direct contact with the FSR
- The ultrasonic sensor can introduce occasional measurement delay
- Very rapid movement through the detection zone may be harder to detect
- The prototype wiring and sensors are not enclosed for long-term household deployment

The multi-sensor architecture reduces the impact of several of these limitations but does not eliminate them completely.

## Future improvements

Future versions could improve the system through:

- Multiple FSR sensors to cover a larger floor area
- More secure mounting of the light source
- Protective housing for exposed circuitry
- Improved filtering for faster intrusion detection
- Additional software filtering and averaging
- More robust sensor calibration
- Separate sensor positions for distinguishing entry from exit
- PCB integration
- Battery backup
- A custom mobile or web dashboard
- Camera integration for intrusion verification

## Important note

The Smart Home Security System is an educational engineering prototype.

It was designed to demonstrate sensor selection, analog signal processing, embedded control, sensor fusion, and IoT integration.

It is not a certified security product and should not be relied upon as a replacement for professionally installed or safety-certified home-security equipment.

## Acknowledgments

The Smart Home Security System was developed as an independent embedded-systems project at the University of Georgia.

The project brought together sensor physics, analog circuit design, microcontroller programming, signal processing, sensor fusion, and IoT communication to create an end-to-end physical security prototype.
