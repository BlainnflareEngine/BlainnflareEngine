#pragma once

#include "Render/CommandQueue.h"

 Blainn::CommandQueue::CommandQueue(eastl::shared_ptr<Device> device, D3D12_COMMAND_LIST_TYPE commandType)
 {
     // Describe and create the command queue.
     D3D12_COMMAND_QUEUE_DESC queueDesc = {};
     // the most often used are direct, compute and copy
     queueDesc.Type = commandType;
     queueDesc.Priority;
     queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
     queueDesc.NodeMask;
     // If we have multiple command queues, we can write a resource only from one queue at the same time.
     // Before it can be accessed by another queue, it must transition to read or common state.
     // In a read state resource can be read from multiple command queues simultaneously, including across processes,
     // based on its read state.
     ThrowIfFailed(device->GetDevicePtr()->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

     switch (commandType)
     {
     case (D3D12_COMMAND_LIST_TYPE_DIRECT):
         m_commandQueue->SetName(L"Direct Command Queue");
         break;
     }
 }

 Blainn::CommandQueue::~CommandQueue()
 {

 }

 ID3D12CommandQueue *Blainn::CommandQueue::Get() const
 {
     return m_commandQueue.Get();
 }