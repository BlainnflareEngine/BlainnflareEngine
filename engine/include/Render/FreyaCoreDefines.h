#pragma once

/*
 * Render Engine CPP wrappers
 */

#ifndef FORCEINLINE
	#define FORCEINLINE __forceinline
#endif

#ifndef VVOID
	#define VVOID virtual void
#endif

/*
 * Memory conversions
 */

#define BYTE_TO_MB(x) ((x) / (1024 * 1024))
#define BYTE_TO_KB(x) (x / 1024)

 /*
  * GPU memory specific
  */

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

/*
 * Debug 
 */

#if defined(DEBUG) || defined(_DEBUG)
	#define SCALD_NAME_D3D12_OBJECT(obj, name)					\
				obj->SetName(name);								\
				OutputDebugString(L"::D3D12 Object Created: "); \
				OutputDebugString(name);						\
				OutputDebugString(L"\n");
#else
	#define SCALD_NAME_D3D12_OBJECT(obj, name)
#endif

/*
 * Textures
 */

//#define TextureMapsMaxCount 512

/*
 * Shader resource binding
 */

#define REGISTER_SPACE_0 0
#define REGISTER_SPACE_1 1

#define SHADER_REGISTER(x) (x)