#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <fstream>

static std::vector<char> readFile(std::string const& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

struct QueueFamilyIndices {
  int graphicsFamily = -1;
  int presentFamily = -1;

  bool isComplete() {
    return graphicsFamily >= 0 && presentFamily >= 0;
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

class HelloTriangleApplication {
 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

 private:
  static constexpr uint32_t WIDTH = 800;
  static constexpr uint32_t HEIGHT = 600;

#ifdef NDEBUG
  static constexpr bool enableValidationLayers = false;
#else
  static constexpr bool enableValidationLayers = true;
#endif

  const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};
  const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeCallback(window, HelloTriangleApplication::onWindowResized);
  }

  static void onWindowResized(GLFWwindow* window, int width, int height) {
    HelloTriangleApplication* app =
        reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
    app->recreateSwapChain();
  }

  void initVulkan() {
    createInstance();
    setupDebugCallback();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSemaphores();
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested, but not available!");
    }

    auto appInfo = vk::ApplicationInfo{}
                       .setPApplicationName("Hello Triangle")
                       .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                       .setPEngineName("No Engine")
                       .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                       .setApiVersion(VK_API_VERSION_1_0);

    auto exts = getRequiredExtensions();

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

    std::cout << "Required extensions: " << std::endl;
    for (auto ext : exts) {
      std::cout << "\t" << ext << std::endl;
    }

    std::cout << "Available extensions:" << std::endl;
    for (const auto& extension : extensions) {
      std::cout << "\t" << extension.extensionName << std::endl;
    }

    auto createInfo = vk::InstanceCreateInfo{}
                          .setPApplicationInfo(&appInfo)
                          .setPpEnabledExtensionNames(exts.data())
                          .setEnabledExtensionCount((uint32_t)exts.size());
    if (enableValidationLayers) {
      createInfo.setPpEnabledLayerNames(validationLayers.data());
    }

    try {
      vulkanInstance = vk::createInstanceUnique(createInfo);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  void setupDebugCallback() {
    if (!enableValidationLayers) {
      return;
    }

    vk::DebugReportCallbackCreateInfoEXT createInfo;
    createInfo.setPfnCallback(debugCallback);
    createInfo.setFlags(
        vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning |
        vk::DebugReportFlagBitsEXT::ePerformanceWarning);
    debugReportCallback = vulkanInstance->createDebugReportCallbackEXTUnique(createInfo, nullptr);
  }

  void createSurface() {
    VkSurfaceKHR localSurface;
    if (glfwCreateWindowSurface(vulkanInstance.get(), window, nullptr, &localSurface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
    surface = localSurface;
  }

  void pickPhysicalDevice() {
    vk::PhysicalDevice chosenDevice{};

    std::vector<vk::PhysicalDevice> devices = vulkanInstance->enumeratePhysicalDevices();

    if (devices.size() == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::multimap<int, vk::PhysicalDevice> candidates;
    for (const auto& candidateDevice : devices) {
      int score = rateDeviceSuitability(candidateDevice);
      candidates.insert(std::make_pair(score, candidateDevice));
    }

    if (candidates.rbegin()->first > 0) {
      physicalDevice = candidates.rbegin()->second;
    } else {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    vk::PhysicalDeviceFeatures features{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.setPEnabledFeatures(&features);

    createInfo.setEnabledExtensionCount(0);
    if (enableValidationLayers) {
      createInfo.setEnabledLayerCount(validationLayers.size());
      createInfo.setPpEnabledLayerNames(validationLayers.data());
    }

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    queueCreateInfos.reserve(uniqueQueueFamilies.size());

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies) {
      vk::DeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.setQueueFamilyIndex(queueFamily);
      queueCreateInfo.setQueueCount(1);
      queueCreateInfo.setPQueuePriorities(&queuePriority);
      queueCreateInfos.push_back(queueCreateInfo);
    }

    createInfo.setPQueueCreateInfos(queueCreateInfos.data());
    createInfo.setQueueCreateInfoCount(queueCreateInfos.size());

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    device = physicalDevice.createDeviceUnique(createInfo);
    if (!device) {
      throw std::runtime_error("failed to create logical device!");
    }

    graphicsQueue = device->getQueue(indices.graphicsFamily, 0);
    presentQueue = device->getQueue(indices.presentFamily, 0);
  }

  void cleanupSwapChain() {
    swapChainFramebuffers.clear();
    device->freeCommandBuffers(commandPool.get(), commandBuffers);
    commandBuffers.clear();

    graphicsPipeline.reset();
    pipelineLayout.reset();
    renderPass.reset();

    swapChainImageViews.clear();
    swapChain.reset();
  }

  void recreateSwapChain() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width == 0 || height == 0) {
      return;
    }

    device->waitIdle();

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandBuffers();
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily,
                                     (uint32_t)indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
      createInfo.queueFamilyIndexCount = 0; // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = nullptr;

    swapChain = device->createSwapchainKHRUnique(createInfo);

    swapChainImages = device->getSwapchainImagesKHR(*swapChain);
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
  }

  void createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
      vk::ImageViewCreateInfo createInfo;
      createInfo.image = swapChainImages[i];
      createInfo.format = swapChainImageFormat;

      createInfo.components.r = vk::ComponentSwizzle::eIdentity;
      createInfo.components.g = vk::ComponentSwizzle::eIdentity;
      createInfo.components.b = vk::ComponentSwizzle::eIdentity;
      createInfo.components.a = vk::ComponentSwizzle::eIdentity;

      createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      swapChainImageViews[i] = device->createImageViewUnique(createInfo);
    }
  }

  void createRenderPass() {
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;

    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits{};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask =
        vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    renderPass = device->createRenderPassUnique(renderPassInfo);
  }

  void createGraphicsPipeline() {
    auto vertShaderCode = readFile("shaders/shader.vert.spv");
    auto fragShaderCode = readFile("shaders/shader.frag.spv");

    vk::UniqueShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    vk::UniqueShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo shaderStages[2];
    vk::PipelineShaderStageCreateInfo& vertShaderStageInfo = shaderStages[0];
    vk::PipelineShaderStageCreateInfo& fragShaderStageInfo = shaderStages[1];

    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule.get();
    vertShaderStageInfo.pName = "main";

    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule.get();
    fragShaderStageInfo.pName = "main";

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(0.0f)
        .setWidth(swapChainExtent.width)
        .setHeight(swapChainExtent.height)
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D{0, 0};
    scissor.extent = swapChainExtent;

    vk::PipelineViewportStateCreateInfo viewportState;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    vk::PipelineMultisampleStateCreateInfo multisampling;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

    vk::PipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    vk::DynamicState dynamicStates[] = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eLineWidth,
    };
    vk::PipelineDynamicStateCreateInfo dynamicState;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = 0; // Optional
    pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo);

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional

    pipelineInfo.layout = pipelineLayout.get();
    pipelineInfo.renderPass = renderPass.get();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    graphicsPipeline = device->createGraphicsPipelineUnique(nullptr, pipelineInfo);
  }

  void createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
      vk::ImageView attachments[] = {swapChainImageViews[i].get()};

      vk::FramebufferCreateInfo framebufferInfo{};
      framebufferInfo.renderPass = renderPass.get();
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = swapChainExtent.width;
      framebufferInfo.height = swapChainExtent.height;
      framebufferInfo.layers = 1;

      swapChainFramebuffers[i] = device->createFramebufferUnique(framebufferInfo);
    }
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = {}; // Optional

    commandPool = device->createCommandPoolUnique(poolInfo);
  }

  void createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool.get();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    commandBuffers = device->allocateCommandBuffers(allocInfo);

    for (size_t i = 0; i < commandBuffers.size(); i++) {
      vk::CommandBufferBeginInfo beginInfo = {};
      beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
      beginInfo.pInheritanceInfo = nullptr; // Optional

      commandBuffers[i].begin(beginInfo);

      vk::RenderPassBeginInfo renderPassInfo = {};
      renderPassInfo.renderPass = renderPass.get();
      renderPassInfo.framebuffer = swapChainFramebuffers[i].get();

      renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
      renderPassInfo.renderArea.extent = swapChainExtent;

      vk::ClearValue clearColor{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

      commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());
      commandBuffers[i].draw(3, 1, 0, 0);
      commandBuffers[i].endRenderPass();

      commandBuffers[i].end();
    }
  }

  void createSemaphores() {
    vk::SemaphoreCreateInfo semaphoreInfo{};
    imageAvailableSemaphore = device->createSemaphoreUnique(semaphoreInfo);
    renderFinishedSemaphore = device->createSemaphoreUnique(semaphoreInfo);
  }

  void drawFrame() {
    uint32_t imageIndex = -1;
    vk::Result getImageResult = vk::Result::eSuccess;
    try {
      getImageResult = device->acquireNextImageKHR(
          swapChain.get(),
          std::numeric_limits<uint64_t>::max(),
          imageAvailableSemaphore.get(),
          nullptr,
          &imageIndex);
    } catch (vk::OutOfDateKHRError& e) {
      recreateSwapChain();
      return;
    }

    if (getImageResult != vk::Result::eSuccess) {
      if (getImageResult == vk::Result::eSuboptimalKHR || getImageResult == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return;
      }
      throw std::exception(std::to_string(static_cast<uint32_t>(getImageResult)).c_str());
    }

    if (imageIndex >= swapChainImages.size()) {
      return;
    }

    vk::SubmitInfo submitInfo{};

    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphore.get()};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphore.get()};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    graphicsQueue.submit(submitInfo, nullptr);

    vk::PresentInfoKHR presentInfo{};

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = {swapChain.get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    try {
      presentQueue.presentKHR(presentInfo);
    } catch (vk::OutOfDateKHRError& e) {
      recreateSwapChain();
    }

    presentQueue.waitIdle();
  }

  vk::UniqueShaderModule createShaderModule(std::vector<char> const& code) {
    vk::ShaderModuleCreateInfo createInfo;

    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    return device->createShaderModuleUnique(createInfo);
  }

  int rateDeviceSuitability(vk::PhysicalDevice const& deviceToRate) {
    vk::PhysicalDeviceProperties props = deviceToRate.getProperties();

    vk::PhysicalDeviceFeatures features = deviceToRate.getFeatures();

    if (!isDeviceSuitable(deviceToRate)) {
      return 0;
    }

    int score = 0;
    // Discrete GPUs have a significant performance advantage
    if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
      score += 1000;
    }
    // Maximum possible size of textures affects graphics quality
    score += props.limits.maxImageDimension2D;
    // Application can't function without geometry shaders
    if (!features.geometryShader) {
      return 0;
    }
    return score;
  }

  bool isDeviceSuitable(vk::PhysicalDevice const& deviceToCheck) {
    QueueFamilyIndices indices = findQueueFamilies(deviceToCheck);

    bool extensionsSupported = checkDeviceExtensionSupport(deviceToCheck);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(deviceToCheck);
      swapChainAdequate =
          !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
  }

  bool checkDeviceExtensionSupport(vk::PhysicalDevice const& deviceToCheck) {
    uint32_t extensionCount;
    auto availableExtensions = deviceToCheck.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice const& deviceToQuery) {
    QueueFamilyIndices indices;

    auto queueFamilies = deviceToQuery.getQueueFamilyProperties();

    int i = 0;
    for (auto const& family : queueFamilies) {
      if (family.queueCount > 0 && family.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.graphicsFamily = i;
      }

      auto presentSupport = deviceToQuery.getSurfaceSupportKHR(i, surface);
      if (family.queueCount > 0 && presentSupport) {
        indices.presentFamily = i;
      }
      if (indices.isComplete()) {
        break;
      }
      i++;
    }

    return indices;
  }

  SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice const& deviceToQuery) {
    SwapChainSupportDetails details;

    details.capabilities = deviceToQuery.getSurfaceCapabilitiesKHR(surface);

    details.formats = deviceToQuery.getSurfaceFormatsKHR(surface);

    details.presentModes = deviceToQuery.getSurfacePresentModesKHR(surface);

    return details;
  }

  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      std::vector<vk::SurfaceFormatKHR> const& availableFormats) {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
      return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
    }

    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
          availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  vk::PresentModeKHR chooseSwapPresentMode(
      std::vector<vk::PresentModeKHR> const& availablePresentModes) {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        return availablePresentMode;
      } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
        bestMode = availablePresentMode;
      }
    }

    return bestMode;
  }

  vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetWindowSize(window, &width, &height);

      VkExtent2D actualExtent = {width, height};

      actualExtent.width = std::max(
          capabilities.minImageExtent.width,
          std::min(capabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = std::max(
          capabilities.minImageExtent.height,
          std::min(capabilities.maxImageExtent.height, actualExtent.height));

      return actualExtent;
    }
  }

  bool checkValidationLayerSupport() {
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char*> getRequiredExtensions() {
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    extensions.reserve(glfwExtensionCount + 1);
    for (unsigned int i = 0; i < glfwExtensionCount; i++) {
      extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugReportFlagsEXT flags,
      VkDebugReportObjectTypeEXT objType,
      uint64_t obj,
      size_t location,
      int32_t code,
      const char* layerPrefix,
      const char* msg,
      void* userData) {
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }
  }

  void cleanup() {
    cleanupSwapChain();

    imageAvailableSemaphore.reset();
    renderFinishedSemaphore.reset();
    commandPool.reset();

    device.reset();
    if (surface) {
      vulkanInstance->destroySurfaceKHR(surface);
      surface = nullptr;
    }
    debugReportCallback.reset();
    vulkanInstance.reset();

    glfwDestroyWindow(window);

    glfwTerminate();
  }

  GLFWwindow* window = nullptr;

  vk::UniqueInstance vulkanInstance;
  vk::UniqueDebugReportCallbackEXT debugReportCallback;
  vk::SurfaceKHR surface;
  vk::UniqueDevice device;

  vk::PhysicalDevice physicalDevice;
  vk::Queue graphicsQueue;
  vk::Queue presentQueue;
  vk::UniqueSwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::Format swapChainImageFormat;
  vk::Extent2D swapChainExtent;

  std::vector<vk::UniqueImageView> swapChainImageViews;
  vk::UniqueRenderPass renderPass;
  vk::UniquePipelineLayout pipelineLayout;
  vk::UniquePipeline graphicsPipeline;

  std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

  vk::UniqueCommandPool commandPool;
  std::vector<vk::CommandBuffer> commandBuffers;

  vk::UniqueSemaphore imageAvailableSemaphore;
  vk::UniqueSemaphore renderFinishedSemaphore;
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
