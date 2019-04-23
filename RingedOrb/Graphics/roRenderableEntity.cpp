#include "roRenderableEntity.h"

void roMesh::UploadGeometryData(void* vertices, size_t vert_arraySize, size_t vert_elementSize, void* indices, size_t ind_arraySize,
	size_t ind_elementSize, bool is32bit) {
	Destroy();
	EnableIndices = true;

	VertexBuffer = new roVertexBuffer((UINT)vert_arraySize, (UINT)vert_elementSize, true);
	VertexBuffer->UploadData(0, 0, vertices);
	IndexBuffer = new roIndexBuffer(ind_arraySize, is32bit, true);
	IndexBuffer->UploadData(0, 0, indices);
}

void roMesh::UploadGeometryData(void* vertices, size_t vert_arraySize, size_t vert_elementSize) {
	Destroy();
	EnableIndices = false;
	VertexBuffer = new roVertexBuffer((UINT)vert_arraySize, (UINT)vert_elementSize, true);
	VertexBuffer->UploadData(0, 0, vertices);

}

void roMesh::UploadGeometryData(void* indices, size_t ind_arraySize, size_t ind_elementSize, bool is32bit) {
	Destroy();
	EnableIndices = true;
	IndexBuffer = new roIndexBuffer((UINT)ind_arraySize, is32bit, true);
	IndexBuffer->UploadData(0, 0, indices);

}

void roMesh::SetVertexAtrributes(bool norm, bool UV) {
	bHasNormal = norm; bHasTexCoord = UV;
}

bool roMesh::GetNormalAtrributeState() {
	return bHasNormal;
}

bool roMesh::GetUVAttributeState() {
	return bHasTexCoord;
}

D3D12_VERTEX_BUFFER_VIEW roMesh::GetVertexBufferView() {
	return VertexBuffer->GetView(0);
}

D3D12_INDEX_BUFFER_VIEW roMesh::GetIndexBufferView() {
	if (!EnableIndices) {
		THROW(L"Indices  not enabled");
	}

	return IndexBuffer->GetView(0);
}

bool roMesh::IndicesState() {
	return EnableIndices;
}

void roMesh::Destroy() {

	if (VertexBuffer) {
		VertexBuffer->Destroy();
		SAFE_DELETE(VertexBuffer);
	}
	if (IndexBuffer) {
		IndexBuffer->Destroy();
		SAFE_DELETE(IndexBuffer);
	}
}