﻿#pragma once

constexpr double kPI = 3.14159265358979323846;
constexpr double kPI2 = 2.0 * 3.14159265358979323846;
constexpr double kINF = 1e128;
constexpr double kEPS = 1e-6;
constexpr uint32_t kDepth = 5;
constexpr uint32_t kDepthLimit = 64;

using Color = Vector3;

#define RESOURCE_DIR "../resource/"
#define MODEL_DIR RESOURCE_DIR##"model/"
#define OUTPUT_DIR "../output/"
