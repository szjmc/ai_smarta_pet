// AI Smart Pet Prototype Firmware
// Based on ESP32-S3 (ESP-IDF)
// Role: AI Hardware Architect - System Integration

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2s.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_nn.h" // AI Runtime
#include "camera.h" // OV2640 DVP

static const char *TAG = "AI_SMART_PET";

// Hardware Pin Definitions (Prototype PCB)
#define GPIO_MOTOR_1    18
#define GPIO_MOTOR_2    19
#define GPIO_LED        21
#define GPIO_MIC_DATA   34
#define GPIO_MIC_CLK    35
#define GPIO_CAM_PWDN   32
#define GPIO_CAM_RESET  33

// AI Model Parameters (YOLOv8-nano INT8)
#define MODEL_PATH      "/spiflash/models/pet_detect_v1.tflite"
#define INPUT_SIZE      224
#define NUM_CLASSES     5 // Dog, Cat, Bird, Human, Abnormal

// Task Handles
TaskHandle_t ai_task_handle = NULL;
TaskHandle_t motor_task_handle = NULL;
TaskHandle_t comm_task_handle = NULL;

// --- AI Inference Task ---
void ai_inference_task(void *pvParameters) {
    ESP_LOGI(TAG, "Starting AI Inference Task...");
    
    // Initialize Camera (OV2640)
    camera_config_t config = {
        .pin_pwdn = GPIO_CAM_PWDN,
        .pin_reset = GPIO_CAM_RESET,
        .pin_xclk = 20,
        .pin_d0 = 26, .pin_d1 = 27, .pin_d2 = 28, .pin_d3 = 29,
        .pin_d4 = 30, .pin_d5 = 31, .pin_d6 = 36, .pin_d7 = 39,
        .pin_pclk = 21,
        .pin_vsync = 22,
        .pin_href = 23,
        .pin_sscb_sda = 18,
        .pin_sscb_scl = 19,
        .pixel_format = PIXFORMAT_RGB565,
        .frame_size = FRAMESIZE_QVGA,
        .jpeg_quality = 12,
        .fb_count = 2
    };
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        vTaskDelete(NULL);
    }

    // Load AI Model (ESP-NN)
    esp_nn_model_t *model = esp_nn_load_model(MODEL_PATH);
    if (!model) {
        ESP_LOGE(TAG, "Failed to load AI model");
        vTaskDelete(NULL);
    }

    while (1) {
        // Capture Image
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            continue;
        }

        // Pre-process and Inference (INT8)
        // Note: ESP-NN API simplified for prototype
        int class_id = esp_nn_infer(model, fb->buf, INPUT_SIZE, INPUT_SIZE);
        
        if (class_id >= 0) {
            ESP_LOGI(TAG, "Detected Class: %d", class_id);
            // Trigger Motor/LED based on detection
            if (class_id == 0) { // Dog detected
                gpio_set_level(GPIO_MOTOR_1, 1);
                gpio_set_level(GPIO_LED, 1);
                vTaskDelay(pdMS_TO_TICKS(2000));
                gpio_set_level(GPIO_MOTOR_1, 0);
                gpio_set_level(GPIO_LED, 0);
            }
        }

        esp_camera_fb_return(fb);
        vTaskDelay(pdMS_TO_TICKS(100)); // 10 FPS
    }
}

// --- Motor Control Task ---
void motor_control_task(void *pvParameters) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_MOTOR_1) | (1ULL << GPIO_MOTOR_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        // Listen for MQTT commands or AI triggers
        // Placeholder for motor PWM control
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// --- Main Entry ---
void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi (Communication Layer)
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_start();

    // Initialize GPIOs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_LED),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(GPIO_LED, 0);

    // Create Tasks
    xTaskCreatePinnedToCore(ai_inference_task, "ai_task", 4096, NULL, 5, &ai_task_handle, 0);
    xTaskCreatePinnedToCore(motor_control_task, "motor_task", 2048, NULL, 3, &motor_task_handle, 1);

    ESP_LOGI(TAG, "AI Smart Pet Prototype Initialized");
}
