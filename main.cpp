#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "VulkanEnv.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

int main(int, char**) {
    std::cout << "=== SDL3 + VulkanEnv Test Program ===" << std::endl;
    
    try {
        // 1. Initialize SDL3
        std::cout << "\n[1] Initializing SDL3..." << std::endl;
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return -1;
        }
        std::cout << "✓ SDL3 initialized successfully!" << std::endl;
        
        // 2. Create SDL window with Vulkan support
        std::cout << "\n[2] Creating SDL3 window with Vulkan support..." << std::endl;
        SDL_Window* window = SDL_CreateWindow(
            "SDL3 + VulkanEnv Test",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
        );
        
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return -1;
        }
        std::cout << "✓ SDL3 window created successfully!" << std::endl;
        std::cout << "  - Size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
        std::cout << "  - Vulkan support enabled" << std::endl;
        
        // 3. Get required Vulkan extensions from SDL
        std::cout << "\n[3] Getting required Vulkan extensions from SDL3..." << std::endl;
        uint32_t extensionCount = 0;
        const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        
        if (!extensions) {
            std::cerr << "Failed to get Vulkan extensions: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }
        
        std::cout << "✓ Required Vulkan extensions (" << extensionCount << "):" << std::endl;
        for (uint32_t i = 0; i < extensionCount; ++i) {
            std::cout << "  - " << extensions[i] << std::endl;
        }
        
        // 4. Initialize VulkanEnv
        std::cout << "\n[4] Initializing VulkanEnv..." << std::endl;
        [[maybe_unused]] auto& vulkanEnv = VulkanEnv::Instance();
        std::cout << "✓ VulkanEnv initialized successfully!" << std::endl;
        
        // Print Vulkan API version
        auto apiVersion = GetCurVulkanAPI();
        std::cout << "  - Vulkan API: " << apiVersion.Major() << "." 
                  << apiVersion.Minor() << "." << apiVersion.Patch() << std::endl;
        
        // 5. Run main loop
        std::cout << "\n[5] Entering main loop..." << std::endl;
        std::cout << "  - Press ESC or close window to exit" << std::endl;
        std::cout << "  - Window is resizable" << std::endl;
        
        bool running = true;
        SDL_Event event;
        uint32_t frameCount = 0;
        
        while (running) {
            // Handle events
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_QUIT:
                        std::cout << "\n[Event] Window close requested" << std::endl;
                        running = false;
                        break;
                    
                    case SDL_EVENT_KEY_DOWN:
                        if (event.key.key == SDLK_ESCAPE) {
                            std::cout << "\n[Event] ESC key pressed" << std::endl;
                            running = false;
                        }
                        break;
                    
                    case SDL_EVENT_WINDOW_RESIZED:
                        std::cout << "[Event] Window resized to: " 
                                  << event.window.data1 << "x" << event.window.data2 << std::endl;
                        break;
                    
                    case SDL_EVENT_WINDOW_MINIMIZED:
                        std::cout << "[Event] Window minimized" << std::endl;
                        break;
                    
                    case SDL_EVENT_WINDOW_RESTORED:
                        std::cout << "[Event] Window restored" << std::endl;
                        break;
                }
            }
            
            // Simple frame counter display
            frameCount++;
            if (frameCount % 60 == 0) {
                std::cout << "[Frame] " << frameCount << " frames rendered" << std::endl;
            }
            
            // Cap frame rate to ~60 FPS
            SDL_Delay(16);
        }
        
        std::cout << "\n[6] Total frames rendered: " << frameCount << std::endl;
        
        // 6. Cleanup
        std::cout << "\n[7] Cleaning up..." << std::endl;
        SDL_DestroyWindow(window);
        std::cout << "✓ SDL3 window destroyed" << std::endl;
        
        SDL_Quit();
        std::cout << "✓ SDL3 quit successfully" << std::endl;
        
        std::cout << "\n=== Test Complete ===" << std::endl;
        std::cout << "All systems working correctly!" << std::endl;
        std::cout << "  - SDL3: ✓" << std::endl;
        std::cout << "  - Vulkan: ✓" << std::endl;
        std::cout << "  - VulkanEnv: ✓" << std::endl;
        
        return 0;
        
    } catch (const vk::SystemError& e) {
        std::cerr << "\n✗ Vulkan System Error: " << e.what() << std::endl;
        SDL_Quit();
        return -1;
        
    } catch (const std::runtime_error& e) {
        std::cerr << "\n✗ Runtime Error: " << e.what() << std::endl;
        SDL_Quit();
        return -2;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Unexpected Error: " << e.what() << std::endl;
        SDL_Quit();
        return -3;
        
    } catch (...) {
        std::cerr << "\n✗ Unknown error occurred!" << std::endl;
        SDL_Quit();
        return -4;
    }
}