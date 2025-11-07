#pragma once

#include "FreyaCoreTypes.h"

class Direct3DQueue;
class Device;

namespace Blainn
{
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Init();
    
    private:
        Device* m_device = nullptr;
        Direct3DQueue* m_commandQueue = nullptr;
    };  
}