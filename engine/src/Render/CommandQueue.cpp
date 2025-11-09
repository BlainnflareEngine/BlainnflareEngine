#pragma once

#include "Render/CommandQueue.h"
#include "Render/Device.h"
#include "Render/DXHelpers.h"

 void Blainn::CommandQueue::Create(
 	ID3D12Device *device, 
 	ECommandQueueType commandType, 
 	const char *name /*= nullptr*/)
 {
	
 }

 void Blainn::CommandQueue::Destroy()
 {
    SAFE_RELEASE(m_commandQueue);
 }