//
// Created by WhoLeb on 21-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <cstdint>

namespace Blainn
{
/// The maximum number of input buffer slots.
/// D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT == 32
#define BLAINN_MAX_BUFFER_SLOTS 32

/// The maximum number of simultaneous render targets.
#define BLAINN_MAX_RENDER_TARGETS 8

/// The maximum number of viewports.
#define BLAINN_MAX_VIEWPORTS 16

/// The maximum number of resource signatures that one pipeline can use
#define BLAINN_MAX_RESOURCE_SIGNATURES 8

/// The maximum number of queues in graphics adapter description.
#define BLAINN_MAX_ADAPTER_QUEUES 16

/// Special constant for the default adapter index.
#define BLAINN_DEFAULT_ADAPTER_ID 0xFFFFFFFFU

/// Special constant for the default queue index.
#define BLAINN_DEFAULT_QUEUE_ID 0xFF

/// The maximum number of shading rate modes.
#define BLAINN_MAX_SHADING_RATES 9

/// Bit shift for the the shading X-axis rate.
#define BLAINN_SHADING_RATE_X_SHIFT 2

/// The maximum number of 4-byte inline constants in a pipeline state.
#define BLAINN_MAX_INLINE_CONSTANTS 64

static constexpr uint32_t MAX_BUFFER_SLOTS        = BLAINN_MAX_BUFFER_SLOTS;
static constexpr uint32_t MAX_RENDER_TARGETS      = BLAINN_MAX_RENDER_TARGETS;
static constexpr uint32_t MAX_VIEWPORTS           = BLAINN_MAX_VIEWPORTS;
static constexpr uint32_t MAX_RESOURCE_SIGNATURES = BLAINN_MAX_RESOURCE_SIGNATURES;
static constexpr uint32_t MAX_ADAPTER_QUEUES      = BLAINN_MAX_ADAPTER_QUEUES;
static constexpr uint32_t DEFAULT_ADAPTER_ID      = BLAINN_DEFAULT_ADAPTER_ID;
static constexpr uint8_t  DEFAULT_QUEUE_ID        = BLAINN_DEFAULT_QUEUE_ID;
static constexpr uint32_t MAX_SHADING_RATES       = BLAINN_MAX_SHADING_RATES;
static constexpr uint32_t SHADING_RATE_X_SHIFT    = BLAINN_SHADING_RATE_X_SHIFT;
static constexpr uint32_t MAX_INLINE_CONSTANTS    = BLAINN_MAX_INLINE_CONSTANTS;


}