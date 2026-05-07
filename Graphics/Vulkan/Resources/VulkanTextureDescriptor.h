#pragma once

#include <vulkan/vulkan.h>

class RenderTarget;

class VulkanTextureDescriptor {
public:

    void Create(VkDevice device, RenderTarget& colorTarget, RenderTarget& depthTarget);
    void Destroy(VkDevice device);

    VkDescriptorSetLayout GetLayout() const { return m_layout; }
    VkDescriptorSet GetSet() const { return m_set; }

private:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    VkDescriptorSet m_set = VK_NULL_HANDLE;

};