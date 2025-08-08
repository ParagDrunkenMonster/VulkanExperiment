
/*#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
*/

#include "EngineMain.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() 
{
    VulkanTutorial::EngineMain Main;

    try
    {
        Main.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}