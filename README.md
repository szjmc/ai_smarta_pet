# AI Smart Pet Prototype - Project Overview
# Role: AI Hardware Architect

## 1. Project Summary
本仓库 `ai_smarta_pet` 包含了 AI 智能宠物硬件原型的完整设计方案，涵盖了：
- **硬件架构设计**（AI_HARDWARE_ARCHITECTURE_DESIGN.md）
- **硬件设计规范**（HARDWARE_DESIGN_SPECIFICATION.md，包含 PCB 层叠、原理图逻辑、BOM 表）
- **软件栈说明**（SOFTWARE_STACK.md，包含目录结构、构建配置、API 参考）
- **原型原理图参考**（schematic/ai_smarta_pet_proto.kicad_sch）
- **固件代码**（firmware/main.c，基于 ESP32-S3 的 FreeRTOS 任务调度与 AI 推理）

## 2. 核心技术指标
- **主控**：ESP32-S3-WROOM-1 (双核 240MHz, Wi-Fi/BLE, 向量指令集)
- **AI 算力**：边缘端 NPU (INT8 量化 YOLOv8-nano, ~150ms 延迟)
- **感知**：OV2640 摄像头 (2MP), 数字麦克风 (I2S), DHT11/MPU6050 传感器
- **执行**：TB6612FNG 电机驱动 (双路), SG90 舵机, RGB LED
- **电源**：USB-C 5V 输入, TP4056 充电管理, 3.3V/1.8V LDO
- **通信**：Wi-Fi 2.4GHz, BLE 5.0, MQTT/HTTP 协议
- **成本**：原型估算约 158 RMB (BOM)

## 3. 开发环境要求
- **硬件**：ESP32-S3 开发板（用于调试），PCB 原型板（4 层，50mm x 50mm）
- **软件**：ESP-IDF v5.x, KiCad 8.x (原理图/PCB), Python 3.10+ (AI 模型转换)
- **工具**：esptool.py (烧录), mqtt_client (调试), oscilloscope (信号验证)

## 4. 研发路线图
1. **阶段一：需求分析与架构设计**（已完成，见 AI_HARDWARE_ARCHITECTURE_DESIGN.md）
2. **阶段二：原理图与 PCB 设计**（已完成，见 HARDWARE_DESIGN_SPECIFICATION.md & schematic/）
3. **阶段三：固件开发与 AI 模型部署**（已完成，见 firmware/main.c & SOFTWARE_STACK.md）
4. **阶段四：原型样机制作与测试**（待执行，需 PCB 打样与元器件采购）
5. **阶段五：功能验证与性能优化**（待执行，AI 延迟优化、功耗测试）
6. **阶段六：量产准备与供应链管理**（待执行）

## 5. 关键文件列表
- `AI_HARDWARE_ARCHITECTURE_DESIGN.md`：系统架构设计方案（13 章节标准输出）
- `HARDWARE_DESIGN_SPECIFICATION.md`：PCB 设计规范、BOM 表、连接逻辑
- `software/SOFTWARE_STACK.md`：软件栈、构建配置、API 参考
- `firmware/main.c`：核心固件代码（AI 推理、电机控制、通信）
- `schematic/ai_smarta_pet_proto.kicad_sch`：KiCad 原理图参考（文本格式）
- `README.md`：项目概览

## 6. 风险与改进建议
- **风险**：ESP32-S3 内部 NPU 算力有限，复杂模型可能无法实时。
  - **改进**：使用向量指令优化，降低模型分辨率，或更换 K210 模组。
- **风险**：摄像头 DVP 信号干扰。
  - **改进**：短走线（< 50mm），地屏蔽。
- **风险**：电机噪音影响传感器。
  - **改进**：独立电源平面，去耦电容。

## 7. 下一步工作
- [ ] 完成 KiCad 原理图绘制（参考 schematic/ai_smarta_pet_proto.kicad_sch）
- [ ] 生成 Gerber 文件（4 层板）
- [ ] 订购原型 PCB（5 块）
- [ ] 编写 ESP-IDF 代码（参考 firmware/main.c）
- [ ] 训练与量化 AI 模型（YOLOv8-nano）
- [ ] 组装与测试原型
