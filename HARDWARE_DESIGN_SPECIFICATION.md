# AI Smart Pet Prototype - Hardware Description
# Role: AI Hardware Architect (Mode C: Hardware Circuit Design)

## 1. PCB Layer Stackup (4-Layer, Detailed)
- **Layer 1 (Top)**: Signal (MCU, Camera, Sensors, LEDs) - **Thickness: 35um**
  - **Key Components**: ESP32-S3-WROOM-1 (Center), OV2640 (Top-Left), TB6612FNG (Bottom-Right), TP4056 (Bottom-Left), AMS1117-3.3/1.8 (Top-Right)
  - **Trace Width**: 0.2mm (Signal), 0.5mm (Power), 1.0mm (Motor Current)
  - **Clearance**: 0.15mm (Signal-Signal), 0.3mm (Power-Ground)
  - **Impedance Control**: 50 Ohm for DVP Clock (24MHz)
  - **Thermal Design**: Copper Pour on Top Layer (1oz) under Motor Driver
- **Layer 2 (Inner1)**: Ground Plane (GND) - **Thickness: 35um** (Complete Ground Plane, Shielding)
- **Layer 3 (Inner2)**: Power Plane (3.3V, 1.8V, 5V) - **Thickness: 35um** (Split Power Regions)
- **Layer 4 (Bottom)**: Signal (Motor Drivers, Communication, Battery) - **Thickness: 35um**
  - **Key Components**: USB-C Connector (Left Edge), Battery Connector (Bottom Edge), Motor Driver Heat Sink (Bottom-Right)
  - **Thermal Design**: Copper Pour on Bottom Layer (2oz), Vias connecting to Ground Plane (10+)
  - **EMC**: Ground Plane under Wi-Fi Antenna (Top-Right), Ferrite Beads on Power Lines

## 2. Schematic Components (Detailed Logic)
### A. Power Management (U1-U3)
- **U1**: TP4056 (Lithium Battery Charger, 5V -> 4.2V)
  - **Pins**: VIN (USB-C 5V), BAT (3.7V Li-ion), GND, PROG (1.2k Resistor), STAT (LED)
  - **Decoupling**: C1 (0.1uF), C2 (10uF) near VIN/BAT
- **U2**: AMS1117-3.3 (3.3V LDO, 1A)
  - **Pins**: VIN (3.7V_OUT), VOUT (3.3V), GND
  - **Decoupling**: C3 (0.1uF), C4 (10uF) near VIN/VOUT
- **U3**: AMS1117-1.8 (1.8V LDO, 500mA for Camera/NPU)
  - **Pins**: VIN (3.3V), VOUT (1.8V), GND
  - **Decoupling**: C5 (0.1uF), C6 (10uF) near VIN/VOUT

### B. Core Controller (U4: ESP32-S3-WROOM-1)
- **Pins**: GPIO0-GPIO47 (Refer to Detailed Logic below), EN (Reset), 3.3V/GND
- **Crystal**: X1 (40MHz Crystal Oscillator)
- **Resistors**: R1-R5 (10k Pull-up/Pull-down)
- **Switch**: SW1 (Reset Button)
- **Decoupling**: C7-C10 (0.1uF) near each power pin

### C. Perception Layer (U5-U8)
- **U5**: OV2640 Camera Module (2MP, DVP Interface, 8-bit parallel)
  - **Pins**: D0-D7 (GPIO26-GPIO39), PCLK (GPIO21), VSYNC (GPIO22), HREF (GPIO23), SCCB_SDA (GPIO18), SCCB_SCL (GPIO19), PWDN (GPIO32), RESET (GPIO33), 1.8V (U3), 3.3V (U2), GND
  - **Decoupling**: C11 (0.1uF), C12 (10uF) near 1.8V/3.3V
- **U6**: Digital Microphone (INMP441, I2S Interface)
  - **Pins**: DIN (GPIO34), BCLK (GPIO35), WS (GPIO36), VDD (3.3V), GND
- **U7**: DHT11 (Temperature/Humidity Sensor)
  - **Pins**: DATA (GPIO25), VCC (3.3V), GND
- **U8**: MPU6050 (6-Axis Accelerometer/Gyroscope)
  - **Pins**: SDA (GPIO21), SCL (GPIO22), VCC (3.3V), GND

### D. Storage (U9)
- **U9**: W25Q128JV (16MB SPI Flash, for AI Model & Firmware)
  - **Pins**: CS (GPIO5), MOSI (GPIO23), MISO (GPIO19), CLK (GPIO18), VCC (3.3V), GND
  - **Decoupling**: C13 (0.1uF)

### E. Execution Layer (U10 & Motors/LEDs)
- **U10**: TB6612FNG (Dual H-Bridge Motor Driver, 1.2A per channel)
  - **Pins**: IN1 (GPIO1), IN2 (GPIO2), IN3 (GPIO3), IN4 (GPIO4), PWM_A (GPIO18), PWM_B (GPIO19), VM (5V USB), VCC (3.3V), GND
  - **Decoupling**: C14 (0.1uF), C15 (10uF) near VM/VCC
  - **Heat Sink**: Copper Pour (2oz) under U10, Vias connecting to Bottom Ground Plane (10+)
- **M1-M2**: SG90 Servo Motors (PWM Control)
  - **Pins**: Signal (PWM_A/PWM_B), VCC (5V), GND
- **LED1-LED4**: RGB LEDs (Status Indicators)
  - **Pins**: R/G/B (GPIO6/GPIO7/GPIO8), VCC (3.3V), GND

### F. Communication & Interface (J1-J3)
- **J1**: USB-C Connector (5V Power & UART Debug)
  - **Pins**: VBUS (5V), GND, D+/D- (UART_TX/RX), CC1/CC2 (Resistors)
- **J2**: MicroSD Card Slot (Optional, for Data Logging)
  - **Pins**: CS (GPIO12), MOSI (GPIO13), MISO (GPIO14), CLK (GPIO15), VCC (3.3V), GND
- **J3**: Battery Connector (3.7V Li-ion)
  - **Pins**: BAT+, BAT-, GND

## 3. Connection Logic (Key Nets - Detailed)
- **MCU_GPIO[0-47]** <-> **Camera_DVP[0-7]**, **Camera_PCLK**, **Camera_VSYNC**, **Camera_HREF** (DVP 8-bit parallel, 24MHz clock, length matching < 50mm, impedance 50Ω)
- **MCU_I2S** <-> **Mic_Data**, **Mic_CLK**, **Speaker_Data**, **Speaker_CLK** (I2S Digital Audio, differential routing, shielding)
- **MCU_SPI** <-> **SPI_Flash_CS**, **SPI_Flash_MOSI**, **SPI_Flash_MISO**, **SPI_Flash_CLK** (SPI 16MB Flash, short traces < 30mm)
- **MCU_PWM** <-> **Motor_Driver_IN1/IN2**, **Motor_Driver_IN3/IN4**, **Motor_Driver_PWM_A/PWM_B** (PWM Control, 1kHz frequency, decoupling capacitors 10uF)
- **MCU_I2C** <-> **DHT11_Data**, **MPU6050_SDA/SCL** (I2C Sensors, pull-up resistors 4.7k)
- **Power_Net** <-> **TP4056_OUT**, **LDO_3V3**, **LDO_1V8**, **5V_USB** (Power Distribution, ferrite beads on power lines, decoupling capacitors 0.1uF/10uF)

## 4. PCB Design Constraints (Detailed)
- **Trace Width**: 0.2mm (Signal), 0.5mm (Power), 1.0mm (Motor Current)
- **Clearance**: 0.15mm (Signal-Signal), 0.3mm (Power-Ground)
- **Impedance Control**: 50 Ohm for DVP Clock (24MHz)
- **Thermal Design**: Copper Pour on Top Layer (1oz) and Bottom Layer (2oz), Thermal Vias under Motor Driver (10+ vias, 0.3mm diameter)
- **EMC**: Ground Plane (Inner1) complete, Power Plane (Inner2) split regions, Ferrite Beads on Power Lines (FB1-FB5), Antenna Clearance (16mm x 16mm, no signal lines)
- **Layout Planning**:
  - **Center (0,0)**: ESP32-S3-WROOM-1 (Core Compute Layer)
  - **Top-Left (-20, 20)**: OV2640 Camera Interface (Perception Input Layer)
  - **Bottom-Right (20, -20)**: TB6612FNG Motor Driver (Execution Control Layer)
  - **Bottom-Left (-20, -20)**: TP4056 & AMS1117 (Power Management Layer)
  - **Top-Right (20, 20)**: W25Q128JV & Sensors (Storage/Perception Layer)
  - **Antenna Area (Top-Right)**: 16mm x 16mm clearance, no signal lines

## 5. Bill of Materials (BOM) Summary (Updated)
| Item | Part Number | Qty | Unit Price (RMB) | Total (RMB) | Notes |
|---|---|---|---|---|---|
| MCU | ESP32-S3-WROOM-1 | 1 | 35.00 | 35.00 | Dual-core 240MHz, Wi-Fi/BLE, Vector Instructions |
| Camera | OV2640 | 1 | 15.00 | 15.00 | 2MP, DVP Interface, 8-bit parallel |
| Flash | W25Q128JV | 1 | 3.00 | 3.00 | 16MB SPI Flash, for AI Model & Firmware |
| Motor Driver | TB6612FNG | 1 | 4.00 | 4.00 | Dual H-Bridge, 1.2A per channel |
| Sensors | DHT11, MPU6050 | 2 | 8.00 | 16.00 | Temp/Humidity, 6-Axis Accelerometer/Gyroscope |
| Audio | INMP441, Speaker | 2 | 10.00 | 20.00 | I2S Digital Mic, 8 Ohm Speaker |
| Power | TP4056, AMS1117-3.3, AMS1117-1.8 | 3 | 5.00 | 15.00 | Charger, 3.3V LDO, 1.8V LDO |
| PCB/Passives | 4-Layer Board, R/C/L/Ferrite Beads | 1 | 50.00 | 50.00 | 50mm x 50mm, 35um thickness, 2oz copper |
| **TOTAL** | | | | **158.00** | **Estimate (Prototype)** |

## 6. AI Model Deployment (Edge) - Detailed
- **Model**: YOLOv8-nano (INT8 Quantized)
- **Framework**: ESP-NN (TensorFlow Lite Micro compatible)
- **Input**: 224x224 RGB (from OV2640 QVGA -> Resize)
- **Output**: Class ID (Dog, Cat, Bird, Human, Abnormal), Confidence Score
- **Latency**: ~150ms (ESP32-S3 Vector Instructions)
- **Memory**: 2MB Flash (Weights) + 512KB SRAM (Activations)
- **Power Consumption**: ~50mW (Inference), ~200mW (Peak)

## 7. Prototype Development Steps (Detailed)
1. **Schematic Design**: Use KiCad/Altium (Refer to Connection Logic above)
   - Import Netlist Logic (see schematic/ai_smarta_pet_proto_detailed.kicad_sch)
   - Place Components (U1-U10, J1-J3, R/C/L)
   - Connect Nets (DVP, I2S, SPI, I2C, PWM, Power)
2. **PCB Layout**: 4-Layer, 50mm x 50mm, USB-C Port, Antenna Clearance
   - Layer Stackup (Top/Inner1/Inner2/Bottom)
   - Physical Layout (Center/Top-Left/Bottom-Right/Bottom-Left/Top-Right)
   - Signal Constraints (DVP 50Ω, I2S Differential, PWM 1kHz)
   - Thermal Design (Copper Pour, Vias)
   - EMC (Ground Plane, Ferrite Beads, Antenna Clearance)
3. **Gerber Generation**: Export for Fabrication (JLCPCB/PCBWay)
   - Top/Inner1/Inner2/Bottom Layers
   - Solder Mask, Silkscreen, Drill Files
4. **Firmware Build**: ESP-IDF v5.x, Compile for ESP32-S3 (refer to firmware/main.c)
5. **Flash Firmware**: Via USB-C (esptool.py)
6. **AI Model Flash**: Burn INT8 Model to SPI Flash (2MB)
7. **Testing**: Camera Capture, Motor Rotation, Wi-Fi Connection, AI Inference

## 8. Risk & Mitigation (Updated)
- **Risk**: ESP32-S3 NPU performance lower than expected.
  - **Mitigation**: Use Vector Instructions optimization, reduce model resolution (224x224 -> 160x160), or replace with Kendryte K210 module.
- **Risk**: Camera DVP signal interference (Critical).
  - **Mitigation**: Short traces (< 50mm), Ground Shielding (Inner1), Impedance Control (50Ω), Differential Routing (I2S).
- **Risk**: Motor noise affecting sensors (Critical).
  - **Mitigation**: Separate Power Planes (Inner2), Decoupling Capacitors (0.1uF/10uF), Thermal Vias (10+), Copper Pour (2oz).
- **Risk**: Wi-Fi Antenna interference (Critical).
  - **Mitigation**: Antenna Clearance (16mm x 16mm), No signal lines in clearance area, Ground Plane shielding.

## 9. Next Steps (Detailed)
- [ ] Complete KiCad Schematic (refer to schematic/ai_smarta_pet_proto_detailed.kicad_sch)
- [ ] Generate Gerber Files (4-Layer, Top/Inner1/Inner2/Bottom)
- [ ] Order Prototype PCB (5 units, JLCPCB/PCBWay)
- [ ] Write ESP-IDF Code (refer to firmware/main.c)
- [ ] Train & Quantize AI Model (YOLOv8-nano, INT8)
- [ ] Assemble & Test Prototype (Camera, Motor, Wi-Fi, AI Inference)
- [ ] Verify EMC/ESD (3kV Static Discharge, 50MHz Interference)

## 10. KiCad Schematic Reference (Detailed Logic)
(See schematic/ai_smarta_pet_proto_detailed.kicad_sch for full netlist and component definitions)
