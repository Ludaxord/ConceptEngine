#pragma once
#include "MeshFactory.h"

#include "../../Core/Containers/Array.h"

#include "../../RenderLayer/Resources.h"
#include "../../RenderLayer/CommandList.h"

#include "../../Scene/AABB.h"
#include "Core/Containers/CEUUID.h"

struct MeshData;

class Mesh
{
public:
    Mesh()  = default;
    ~Mesh() = default;

    bool Init(const MeshData& Data);
    
    bool BuildAccelerationStructure(CommandList& CmdList);

    static TSharedPtr<Mesh> Make(const MeshData& Data);

public:
    void CreateBoundingBox(const MeshData& Data);

    CERef<VertexBuffer>       VertexBuffer;
    CERef<ShaderResourceView> VertexBufferSRV;
    CERef<IndexBuffer>        IndexBuffer;
    CERef<ShaderResourceView> IndexBufferSRV;
    CERef<RayTracingGeometry> RTGeometry;
    
    uint32 VertexCount = 0;
    uint32 IndexCount  = 0;

    float ShadowOffset = 0.0f;

    AABB BoundingBox;
 
    CEUUID ID = 0; 
};
