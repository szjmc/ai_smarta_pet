# AI Smart Pet Prototype - Software Stack
# Role: AI Hardware Architect

## 1. Directory Structure
```
/firmware
  ├── main.c                # Main Entry & Task Scheduling (FreeRTOS)
  ├── camera.c              # OV2640 DVP Driver (ESP-IDF)
  ├── motor.c               # TB6612FNG PWM Control
  ├── audio.c               # I2S Mic/Speaker Driver
  ├── sensors.c             # DHT11/MPU6050 I2C Driver
  ├── ai_model.tflite       # INT8 Quantized YOLOv8-nano (2MB)
  ├── esp_nn_wrapper.c      # ESP-NN Inference Wrapper
  ├── wifi_comm.c           # MQTT/HTTP Client
  └── CMakeLists.txt        # Build Configuration
```

## 2. Build Configuration (CMakeLists.txt)
```cmake
cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME "ai_smarta_pet")
project(${PROJECT_NAME})

# ESP-IDF Components
set(EXTRA_COMPONENT_DIRS "components/esp-nn" "components/camera")

idf_component_register(SRCS "main.c" "camera.c" "motor.c" "audio.c" "sensors.c" "esp_nn_wrapper.c" "wifi_comm.c"
                       INCLUDE_DIRS "."
                       REQUIREMENTS "esp_wifi" "esp_event" "freertos" "driver" "esp_nn"
                       EMBED_FILES "ai_model.tflite")
```

## 3. Key Code Snippets (Refer to main.c)
### A. Task Scheduling (FreeRTOS)
- **AI Task** (Priority 5, Core 0): Camera Capture -> ESP-NN Inference -> Action Trigger
- **Motor Task** (Priority 3, Core 1): PWM Control -> Feedback Loop
- **Comm Task** (Priority 2, Core 1): MQTT Publish -> Cloud Sync
- **Sensor Task** (Priority 1, Core 0): DHT11/MPU6050 Read -> Local Log

### B. AI Inference Wrapper (esp_nn_wrapper.c)
```c
int esp_nn_infer(esp_nn_model_t *model, uint8_t *image_buf, int width, int height) {
    // Pre-process: Resize QVGA (320x240) -> 224x224, Normalize
    // Inference: esp_nn_run(model, input_tensor, output_tensor)
    // Post-process: Argmax(output_tensor) -> Class ID
    // Return Class ID (0: Dog, 1: Cat, 2: Bird, 3: Human, 4: Abnormal)
}
```

### C. Motor Control (motor.c)
```c
void motor_set_speed(int motor_id, int speed) {
    // PWM Duty Cycle: 0-255
    // GPIO_MOTOR_1/2 controlled by TB6612FNG IN1/IN2
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
```

## 4. AI Model Training & Quantization (External)
- **Source Model**: YOLOv8-nano (PyTorch)
- **Quantization**: INT8 (Post-Training Quantization)
- **Conversion**: TFLite -> ESP-NN Format (esp-nn-converter)
- **Input Size**: 224x224 RGB
- **Output**: 5 Classes (Dog, Cat, Bird, Human, Abnormal)
- **Accuracy**: > 90% (Prototype Target)

## 5. Deployment Instructions
1. **Install ESP-IDF**: v5.x (Ubuntu/Windows/macOS)
2. **Clone Repository**: `git clone https://github.com/szjmc/ai_smarta_pet.git`
3. **Build Firmware**: `idf.py build -DIDF_TARGET=esp32s3`
4. **Flash Firmware**: `idf.py flash -p /dev/ttyUSB0`
5. **Monitor Logs**: `idf.py monitor`
6. **Connect Wi-Fi**: Use App or Serial Console to configure SSID/Password
7. **Test AI**: Point Camera at Pet -> Observe Motor/LED Response

## 6. API Reference (MQTT)
- **Topic**: `ai_smarta_pet/status`
  - **Payload**: `{"pet_type": "dog", "confidence": 0.95, "temp": 25.5, "humidity": 60}`
- **Topic**: `ai_smarta_pet/control`
  - **Payload**: `{"action": "feed", "amount": 50}` -> Trigger Motor
- **Topic**: `ai_smarta_pet/config`
  - **Payload**: `{"wifi_ssid": "...", "wifi_pass": "..."}` -> Update Config

## 7. Troubleshooting
- **Camera Not Found**: Check DVP Pin Mapping, 1.8V Power, Reset Pin
- **AI Inference Slow**: Check Vector Instructions Enabled, Model Quantization
- **Motor Not Moving**: Check TB6612FNG Enable Pin, PWM Frequency (1kHz)
- **Wi-Fi Connection Failed**: Check SSID/Password, Antenna Clearance
