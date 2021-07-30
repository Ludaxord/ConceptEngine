#pragma once
#include "../CEDefinitions.h"

#define D3D12_MAX_ROOT_PARAMETERS              (64)
#define D3D12_MAX_SHADER_PARAMETERS            (16)
#define D3D12_MAX_HIT_GROUPS                   (512)
#define D3D12_MAX_DESCRIPTOR_RANGES            (64)
#define D3D12_MAX_DESCRIPTOR_RANGE_SIZE        (256)
#define D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT (32)
#define D3D12_MAX_VERTEX_BUFFER_SLOTS          (8)
#define D3D12_MAX_RENDER_TARGET_COUNT          (8)
#define D3D12_MAX_ONLINE_DESCRIPTOR_COUNT      (2048) // See spec.
#define D3D12_MAX_OFFLINE_DESCRIPTOR_COUNT     (2048) // See spec.

#define D3D12_DEFAULT_QUERY_COUNT (64)

#define D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT (D3D12_MAX_ONLINE_DESCRIPTOR_COUNT)
#define D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT  (D3D12_MAX_ONLINE_DESCRIPTOR_COUNT)

#define D3D12_DEFAULT_DESCRIPTOR_TABLE_HANDLE_COUNT     (16)
#define D3D12_DEFAULT_CONSTANT_BUFFER_COUNT             (16)
#define D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT        (16)
#define D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT       (16)
#define D3D12_DEFAULT_SAMPLER_STATE_COUNT               (16)

#define D3D12_DEFAULT_LOCAL_CONSTANT_BUFFER_COUNT       (4)
#define D3D12_DEFAULT_LOCAL_SHADER_RESOURCE_VIEW_COUNT  (8)
#define D3D12_DEFAULT_LOCAL_UNORDERED_ACCESS_VIEW_COUNT (4)
#define D3D12_DEFAULT_LOCAL_SAMPLER_STATE_COUNT         (4)

// Space: | Usage:
// 0      | Standard
// 1      | Constants
// 2      | RT Local

#define D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS (1)
#define D3D12_SHADER_REGISTER_SPACE_RT_LOCAL        (2)