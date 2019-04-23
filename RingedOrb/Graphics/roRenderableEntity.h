#pragma once
#include "..\Core\pch.h"
#include "roGeometryBuffer.h"
#include <DirectXMath.h>
#include "..\Core\roID_Generator.h"
#include "roMaterial.h"

struct roMeshInfo {
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	UINT BaseVertexLocation = 0;
	UINT VertexCount = 0;
};
struct roMesh {		

	void UploadGeometryData(void* vertices, size_t vert_arraySize, size_t vert_elementSize, void* indices, size_t ind_arraySize,
		size_t ind_elementSize, bool is32bit);
	void UploadGeometryData(void* vertices, size_t vert_arraySize, size_t vert_elementSize);
	void UploadGeometryData(void* indices, size_t ind_arraySize, size_t ind_elementSize, bool is32bit);
	void SetVertexAtrributes(bool norm, bool UV);
	bool GetNormalAtrributeState();
	bool GetUVAttributeState();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	bool IndicesState();

	void Destroy();
	std::vector<roMeshInfo> MeshInfo;

private:
	roVertexBuffer* VertexBuffer = nullptr;
	roIndexBuffer*  IndexBuffer = nullptr;
	bool EnableIndices			= false;
	bool bHasNormal = false;
	bool bHasTexCoord = false;
};

enum R_ENTITY_TYPE_OPAQUE {
	RET_OPAQUE_MAT = 0,
	RET_OPAQUE_VERT_COLOR = 1,
	RET_OPAQUE_LIGHT_MAT = 2,
	RET_OPAQUE_LIGHT_MAT_TEX = 3,
	RET_OPAQUE_LINES = 4,
	RET_OPAQUE_COUNT = 5
};

struct  roRenderableEntity {

	UINT64 ID;
	roMesh* Mesh;

 	roMaterial* Material = nullptr;
	
	DirectX::XMFLOAT4X4 WorldTransform;
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;

	UINT64 ObjectConstantBufferRingOffset;
	UINT64 MaterialConstantBufferRingOffset;
};