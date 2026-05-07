#include "VulkanSceneResources.h"

#include <iostream>
#include <ostream>

#include "../../../Engine/Core/Error/ErrorDialog.h"
#include "../Utils/VulkanUtils.h"


void VulkanSceneResources::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits msaaSamples, float ssaaScale, TextureFilter filterMode, VkRenderPass renderPass) {

    m_extent = extent;
    m_samples = msaaSamples;
    m_aaMode = aaMode;

    // EXTENT SWITCH
    switch (aaMode) {

        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:
             m_extent.width = static_cast<uint32_t>(m_extent.width * ssaaScale);
             m_extent.height = static_cast<uint32_t>(m_extent.height * ssaaScale);
             break;

        default:
             break;

    }

    // COLOR SWITCH
    switch (aaMode) {

        case AntiAliasing::None:
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:

             // Color
             CreateImage(physicalDevice, device, m_extent.width, m_extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_SAMPLE_COUNT_1_BIT,SceneColor.Image, SceneColor.Memory);
             SceneColor.View = CreateImageView(device, SceneColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
             SceneColor.CreateSampler(device, filterMode);
             break;

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:

             // MSAA Color
             CreateImage(physicalDevice, device, m_extent.width, m_extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_samples, MSAAColor.Image, MSAAColor.Memory);
             MSAAColor.View = CreateImageView(device, MSAAColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

             // Resolve Color
             CreateImage(physicalDevice, device, m_extent.width, m_extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, ResolveColor.Image, ResolveColor.Memory);
             ResolveColor.View = CreateImageView(device, ResolveColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
             ResolveColor.CreateSampler(device, filterMode);
            break;
    }

    // DEPTH SWITCH
    switch (aaMode) {

        case AntiAliasing::None:
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:

            // Scene Depth
            CreateImage(physicalDevice, device, m_extent.width, m_extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, SceneDepth.Image, SceneDepth.Memory);
            SceneDepth.View = CreateImageView(device, SceneDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
            SceneDepth.CreateSampler(device, filterMode);
            break;

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:

            // MSAA Depth
            CreateImage(physicalDevice, device, m_extent.width, m_extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_samples, SceneDepth.Image, SceneDepth.Memory);
            SceneDepth.View = CreateImageView(device, SceneDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

            // Resolve Depth
            CreateImage(physicalDevice, device, m_extent.width, m_extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, ResolveDepth.Image, ResolveDepth.Memory);
            ResolveDepth.View = CreateImageView(device, ResolveDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
            ResolveDepth.CreateSampler(device, filterMode);
            break;

    }


    // FRAMEBUFFER
    std::vector<VkImageView> attachments;

    switch (aaMode) {

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
             attachments = {
                MSAAColor.View,
                SceneDepth.View,
                ResolveColor.View,
                ResolveDepth.View
             };
             break;

        default:
             attachments = {
                SceneColor.View,
                SceneDepth.View
             };

    }

    VkFramebufferCreateInfo fb{};
    fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb.renderPass = renderPass;
    fb.attachmentCount = static_cast<uint32_t>(attachments.size());
    fb.pAttachments = attachments.data();
    fb.width = m_extent.width;
    fb.height = m_extent.height;
    fb.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &fb, nullptr, &m_framebuffer));

    std::cout << "[Vulkan] Scene-resources created" << std::endl;

}

void VulkanSceneResources::Destroy(VkDevice device, AntiAliasing aaMode) {

    vkDestroyFramebuffer(device, m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;

    SceneColor.Destroy(device);
    SceneDepth.Destroy(device);
    MSAAColor.Destroy(device);
    ResolveColor.Destroy(device);
    ResolveDepth.Destroy(device);

    std::cout << "[Vulkan] Scene-resources destroyed" << std::endl;

}

RenderTarget& VulkanSceneResources::GetOutputColor(){

    switch (m_aaMode) {
        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
            return ResolveColor;

        default:
            return SceneColor;
    }

}

RenderTarget& VulkanSceneResources::GetOutputDepth() {

    switch (m_aaMode) {
        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
            return ResolveDepth;

        default:
            return SceneDepth;
    }

}





