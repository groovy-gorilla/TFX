#pragma once

#include <vulkan/vulkan.h>
#include "../../../Engine/Core/Window.h"
#include "../Resources/RenderTarget.h"

class VulkanSceneResources {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits msaaSamples, float ssaaScale, TextureFilter filterMode, VkRenderPass renderPass);
    void Destroy(VkDevice device, AntiAliasing aaMode);

    VkFramebuffer GetFramebuffer() const { return m_framebuffer; }
    VkExtent2D GetExtent() const { return m_extent; }

    RenderTarget& GetOutputColor();
    RenderTarget& GetOutputDepth();

    RenderTarget SceneColor{};
    RenderTarget SceneDepth{};
    RenderTarget MSAAColor{};
    RenderTarget ResolveColor{};
    RenderTarget ResolveDepth{};

private:
    VkExtent2D m_extent{};
    VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;
    AntiAliasing m_aaMode = AntiAliasing::None;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

};