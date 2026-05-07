#pragma once

#include <vulkan/vulkan.h>

enum class AntiAliasing {
    None,
    MSAA,
    SSAA,
    MSAA_TAA,
    SSAA_TAA
};

enum class TextureFilter {
    Nearest,
    Linear
};

struct ApplicationDesc {

    const char* TITLE                   = "Indigo Engine";

    uint32_t WIDTH                      = 800;
    uint32_t HEIGHT                     = 600;

    float SCALING                       = 1.0f;

    bool FULLSCREEN                     = false;
    bool ASPECT_RATIO                   = false;
    bool VSYNC                          = true;

    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    TextureFilter FILTER                = TextureFilter::Nearest;

    // AntiAliasing
    AntiAliasing AA_MODE                = AntiAliasing::None;

    // MSAA
    VkSampleCountFlagBits MSAA_SAMPLES  = VK_SAMPLE_COUNT_1_BIT;

    // SSAA
    float SSAA_SCALE                    = 1.0f;     // scale choice: 1.0 - 1.25 - 1.5 - 2.0

};