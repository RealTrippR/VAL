#include <FML/lib/system/renderPassInfo.hpp>

namespace fml {
    void renderPassInfo::createSubpassDependencies(std::vector<VkSubpassDependency>& dependencies) {
        size_t subpassCount = subpasses.size();

        // Iterate through subpasses to define dependencies
        for (size_t srcSubpass = 0; srcSubpass < subpassCount; ++srcSubpass) {
            for (size_t dstSubpass = srcSubpass + 1; dstSubpass < subpassCount; ++dstSubpass) {
                // Check if there are shared attachments between subpasses
                for (size_t attachmentIndex = 0; attachmentIndex < attachments.size(); ++attachmentIndex) {
                    const VkAttachmentReference* srcAttachment = nullptr;
                    const VkAttachmentReference* dstAttachment = nullptr;

                    // Check if this attachment is used in the source subpass
                    for (uint32_t i = 0; i < subpasses[srcSubpass].colorAttachmentCount; ++i) {
                        if (subpasses[srcSubpass].pColorAttachments[i].attachment == attachmentIndex) {
                            srcAttachment = &subpasses[srcSubpass].pColorAttachments[i];
                            break;
                        }
                    }

                    // Check if this attachment is used in the destination subpass
                    for (uint32_t i = 0; i < subpasses[dstSubpass].inputAttachmentCount; ++i) {
                        if (subpasses[dstSubpass].pInputAttachments[i].attachment == attachmentIndex) {
                            dstAttachment = &subpasses[dstSubpass].pInputAttachments[i];
                            break;
                        }
                    }

                    // If both subpasses use the same attachment, create a dependency
                    if (srcAttachment && dstAttachment) {
                        VkSubpassDependency dependency{};
                        dependency.srcSubpass = srcSubpass;
                        dependency.dstSubpass = dstSubpass;
                        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                        dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                        dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // Optimize for overlapping regions

                        dependencies.push_back(dependency);
                    }
                }
            }
        }

        // Add external dependencies (optional, depends on application needs)
        // Example: Ensure attachment is in correct layout before first subpass
        VkSubpassDependency preRenderDependency{};
        preRenderDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        preRenderDependency.dstSubpass = 0;
        preRenderDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        preRenderDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        preRenderDependency.srcAccessMask = 0;
        preRenderDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        preRenderDependency.dependencyFlags = 0;

        dependencies.push_back(preRenderDependency);

        // Example: Ensure layout transition after last subpass
        VkSubpassDependency postRenderDependency{};
        postRenderDependency.srcSubpass = subpassCount - 1;
        postRenderDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
        postRenderDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        postRenderDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        postRenderDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        postRenderDependency.dstAccessMask = 0;
        postRenderDependency.dependencyFlags = 0;

        dependencies.push_back(postRenderDependency);
    }
}