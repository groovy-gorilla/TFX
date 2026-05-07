#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class VulkanSync {
public:

    void Create(VkDevice device, uint32_t maxFramesInFlight);
    void Destroy(VkDevice device);
    void Wait(VkDevice device);
    void Reset(VkDevice device);

    VkSemaphore GetImageAvailableSemaphore() const;
    VkSemaphore GetRenderFinishedSemaphore() const;
    VkFence GetFence() const;

    uint32_t GetCurrentFrame() const;

    void NextFrame(uint32_t maxFrames);

private:
    std::vector<VkFence> m_renderFences{};
    std::vector<VkSemaphore> m_imageAvailableSemaphores{};
    std::vector<VkSemaphore> m_renderFinishedSemaphores{};

    uint32_t m_currentFrame = 0;

};