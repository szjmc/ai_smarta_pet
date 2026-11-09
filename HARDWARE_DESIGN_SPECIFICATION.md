# AI Smart Pet Prototype - Hardware Description
# Role: AI Hardware Architect

## 1. PCB Layer Stackup (4-Layer)
- **Layer 1 (Top)**: Signal (MCU, Camera, Sensors, LEDs)
- **Layer 2 (Inner1)**: Ground Plane (GND)
- **Layer 3 (Inner2)**: Power Plane (3.3V, 1.8V, 5V)
- **Layer 4 (Bottom)**: Signal (Motor Drivers, Communication, Battery)

## 2. Schematic Components
### A. Power Management
- **U1**: TP4056 (Lithium Battery Charger, 5V -> 4.2V)
- **U2**: AMS1117-3.3 (3.3V LDO, 1A)
- **U3**: AMS1117-1.8 (1.8V LDO, 500mA for Camera/NPU)
- **C1-C10**: 0.1uF/10uF Ceramic Capacitors (Decoupling)
- **L1**: 10uH Inductor (Power Filtering)

### B. Core Controller
- **U4**: ESP32-S3-WROOM-1 (Dual-core 240MHz, Wi-Fi/BLE, Vector Instructions)
- **X1**: 40MHz Crystal Oscillator
- **R1-R5**: 10k Pull-up/Pull-down Resistors
- **SW1**: Reset Button

### C. Perception Layer
- **U5**: OV2640 Camera Module (2MP, DVP Interface, 8-bit parallel)
- **U6**: Digital Microphone (I2S Interface, INMP441)
- **U7**: DHT11 (Temperature/Humidity Sensor)
- **U8**: MPU6050 (6-Axis Accelerometer/Gyroscope)

### D. Storage
- **U9**: W25Q128JV (16MB SPI Flash, for AI Model & Firmware)
- **J1**: MicroSD Card Slot (Optional, for Data Logging)

### E. Execution Layer
- **U10**: TB6612FNG (Dual H-Bridge Motor Driver, 1.2A per channel)
- **M1-M2**: SG90 Servo Motors (PWM Control)
- **LED1-LED4**: RGB LEDs (Status Indicators)
- **SPK1**: 8 Ohm Speaker (I2S Output)

### F. Communication
- **ANT1**: PCB Trace Antenna (2.4GHz Wi-Fi/BLE)
- **J2**: USB-C Connector (5V Power & UART Debug)

## 3. Connection Logic (Key Nets)
- **MCU_GPIO[0-47]** <-> **Camera_DVP[0-7]**, **Camera_PCLK**, **Camera_VSYNC**, **Camera_HREF**
- **MCU_I2S** <-> **Mic_Data**, **Mic_CLK**, **Speaker_Data**, **Speaker_CLK**
- **MCU_SPI** <-> **SPI_Flash_CS**, **SPI_Flash_MOSI**, **SPI_Flash_MISO**, **SPI_Flash_CLK**
- **MCU_PWM** <-> **Motor_Driver_IN1/IN2**, **Motor_Driver_IN3/IN4**
- **MCU_I2C** <-> **DHT11_Data**, **MPU6050_SDA/SCL**
- **Power_Net** <-> **TP4056_OUT**, **LDO_3V3**, **LDO_1V8**, **5V_USB**

## 4. PCB Design Constraints
- **Trace Width**: 0.2mm (Signal), 0.5mm (Power), 1.0mm (Motor Current)
- **Clearance**: 0.15mm (Signal-Signal), 0.3mm (Power-Ground)
- **Impedance Control**: 50 Ohm for DVP Clock (24MHz)
- **Thermal Design**: Copper Pour on Bottom Layer (2oz), Thermal Vias under Motor Driver
- **EMC**: Ground Plane under Wi-Fi Antenna, Ferrite Beads on Power Lines

## 5. Bill of Materials (BOM) Summary
| Item | Part Number | Qty | Unit Price (RMB) | Total (RMB) |
|---|---|---|---|---|
| MCU | ESP32-S3-WROOM-1 | 1 | 35.00 | 35.00 |
| Camera | OV2640 | 1 | 15.00 | 15.00 |
| Flash | W25Q128JV | 1 | 3.00 | 3.00 |
| Motor Driver | TB6612FNG | 1 | 4.00 | 4.00 |
| Sensors | DHT11, MPU6050 | 2 | 8.00 | 16.00 |
| Audio | INMP441, Speaker | 2 | 10.00 | 20.00 |
| Power | TP4056, AMS1117 | 3 | 5.00 | 15.00 |
| PCB/Passives | 4-Layer Board, R/C/L | 1 | 50.00 | 50.00 |
| **TOTAL** | | | | **158.00** |

## 6. AI Model Deployment (Edge)
- **Model**: YOLOv8-nano (INT8 Quantized)
- **Framework**: ESP-NN (TensorFlow Lite Micro compatible)
- **Input**: 224x224 RGB (from OV2640 QVGA -> Resize)
- **Output**: Class ID (Dog, Cat, Bird, Human, Abnormal), Confidence Score
- **Latency**: ~150ms (ESP32-S3 Vector Instructions)
- **Memory**: 2MB Flash (Weights) + 512KB SRAM (Activations)

## 7. Prototype Development Steps
1. **Schematic Design**: Use KiCad/Altium (Refer to Connection Logic above)
2. **PCB Layout**: 4-Layer, 50mm x 50mm, USB-C Port, Antenna Clearance
3. **Gerber Generation**: Export for Fabrication (JLCPCB/PCBWay)
4. **Firmware Build**: ESP-IDF v5.x, Compile for ESP32-S3
5. **Flash Firmware**: Via USB-C (esptool.py)
6. **AI Model Flash**: Burn INT8 Model to SPI Flash
7. **Testing**: Camera Capture, Motor Rotation, Wi-Fi Connection, AI Inference

## 8. Risk & Mitigation
- **Risk**: ESP32-S3 NPU performance lower than expected.
  - **Mitigation**: Use Vector Instructions optimization, reduce model resolution.
- **Risk**: Camera DVP signal interference.
  - **Mitigation**: Short traces (< 50mm), Ground Shielding.
- **Risk**: Motor noise affecting sensors.
  - **Mitigation**: Separate Power Planes, Decoupling Capacitors.

## 9. Next Steps
- [ ] Complete KiCad Schematic (refer to Connection Logic)
- [ ] Generate Gerber Files (4-Layer)
- [ ] Order Prototype PCB (5 units)
- [ ] Write ESP-IDF Code (main.c provided)
- [ ] Train & Quantize AI Model (YOLOv8-nano)
- [ ] Assemble & Test Prototype
