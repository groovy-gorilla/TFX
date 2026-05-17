#version 450

layout(set = 0, binding = 0)
uniform sampler2D sceneTexture;

layout(set = 0, binding = 1)
uniform sampler2D depthTexture;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float exposure;
} pc;

void main() {

    // HDR
    vec3 hdr = texture(sceneTexture, inUV).rgb;

    hdr *= pc.exposure;

    vec3 mapped = hdr / (1.0 + hdr);

    //mapped = pow(mapped, vec3(1.0 / 2.2));

    outColor = vec4(mapped, 1.0);

}

// Z poniższej funkcji wyiera format dla swapchain.
// Jeśli jest SRGB to nie włączaj pow()
// Jeśli jest UNORM to włącz.
/*
VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

// 1. Najlepszy wybór (sRGB)
for (const auto& f : formats) {
if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
return f;
}
}

// 2. Drugi wybór (UNORM + sRGB colorspace)
for (const auto& f : formats) {
if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
return f;
}
}

// 3. Ostateczność
return formats[0];

}*/