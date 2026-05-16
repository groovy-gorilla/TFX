#include "pch.h"
#include "VulkanTextureDescriptor.h"
#include "RenderTarget.h"
#include "Debug/ErrorDialog.h"
#include "Core/ApplicationDesc.h"

void VulkanTextureDescriptor::Create(VkDevice device, uint32_t maxFramesInFlight, RenderTarget& colorTarget, RenderTarget& depthTarget, TextureFilter filter) {

    CreateDescriptorResources(device, maxFramesInFlight);

    for (uint32_t i = 0; i < maxFramesInFlight; i++) {
        UpdateColor(device, i, colorTarget, filter);
        UpdateDepth(device, i, depthTarget, filter);
    }

}

void VulkanTextureDescriptor::Create(VkDevice device, uint32_t maxFramesInFlight, TextureFilter filter) {

    CreateDescriptorResources(device, maxFramesInFlight);

}

void VulkanTextureDescriptor::Destroy(VkDevice device) {

    if (m_pool) {
        vkDestroyDescriptorPool(device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }

    if (m_layout) {
        vkDestroyDescriptorSetLayout(device, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }

}

void VulkanTextureDescriptor::CreateDescriptorResources(VkDevice device, uint32_t maxFramesInFlight) {

    // DESCRIPTOR SET LAYOUT
    VkDescriptorSetLayoutBinding bindings[2]{};

    // COLOR
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // DEPTH
    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo,nullptr, &m_layout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 2 * maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SET
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType =VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;

    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, m_layout);

    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(maxFramesInFlight);

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, m_sets.data()));

}

void VulkanTextureDescriptor::UpdateColor(VkDevice device, uint32_t frameIndex, RenderTarget& color, TextureFilter filter) {

    VkDescriptorImageInfo colorInfo{};
    colorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    colorInfo.imageView = color.View;
    colorInfo.sampler = (filter == TextureFilter::Nearest) ? color.NearestSampler : color.LinearSampler;

    VkWriteDescriptorSet colorWrite{};
    colorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    colorWrite.dstSet = m_sets[frameIndex];
    colorWrite.dstBinding = 0;
    colorWrite.dstArrayElement = 0;
    colorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorWrite.descriptorCount = 1;
    colorWrite.pImageInfo = &colorInfo;

    vkUpdateDescriptorSets(device, 1, &colorWrite, 0, nullptr);

}

void VulkanTextureDescriptor::UpdateDepth(VkDevice device, uint32_t frameIndex, RenderTarget& depth, TextureFilter filter) {

    VkDescriptorImageInfo depthInfo{};
    depthInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    depthInfo.imageView = depth.View;
    depthInfo.sampler = (filter == TextureFilter::Nearest) ? depth.NearestSampler : depth.LinearSampler;

    VkWriteDescriptorSet depthWrite{};
    depthWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    depthWrite.dstSet = m_sets[frameIndex];
    depthWrite.dstBinding = 1;
    depthWrite.dstArrayElement = 0;
    depthWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthWrite.descriptorCount = 1;
    depthWrite.pImageInfo = &depthInfo;

    vkUpdateDescriptorSets(device, 1, &depthWrite, 0, nullptr);

}
