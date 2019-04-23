#include "roGraphicsManager.h"
#include "roModelLoader.h"
#include "roVertex.h"
#include "roGraphicsResources.h"
#include <DirectXMath.h>
#include "roRenderer.h"


roGraphicsManager::roGraphicsManager(roEventQueue* eventqueue) : roEventNode(eventqueue) {
	SubscribeForEvent(roEVENT_ID::E_ID_MODEL);
	SubscribeForEvent(roEVENT_ID::E_ID_RENTITY);
	Initialize();
}

void roGraphicsManager::Initialize() {
	roMaterial mat;
	roGraphicsResourceLoader::LoadMaterial(mat, "default");

	auto gridVertices = GeometryGen::GridLines(10.0f, 10.0f, 12, 12);
	roMesh gridLineMesh;
	gridLineMesh.UploadGeometryData(&gridVertices[0], sizeof(RingedOrb::Vertex) * gridVertices.size(), sizeof(RingedOrb::Vertex));

	roMeshInfo meshInfoModel;
	meshInfoModel.BaseVertexLocation = 0;
	meshInfoModel.IndexCount = 0;
	meshInfoModel.StartIndexLocation = 0;
	meshInfoModel.VertexCount = gridVertices.size();

	gridLineMesh.MeshInfo.push_back(meshInfoModel);

	roGraphicsResourceLoader::LoadMesh(gridLineMesh, "gridline");
		;
	roRenderableEntity GridREntity;

	GridREntity.ID = m_IDGenerator.GetID();
	GridREntity.Mesh = roGraphicsResourceGet::Mesh("gridline");
	GridREntity.PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	DirectX::XMStoreFloat4x4(&GridREntity.WorldTransform, DirectX::XMMatrixIdentity());

	AddREntityOpaque(GridREntity, RET_OPAQUE_LINES);

	roGraphicsRenderer::CopyRenderableEntitiesOpaque(m_OpaqueREntities);
	roGraphicsRenderer::CopyRenderableEntities(m_REntitiesPool);
}

void roGraphicsManager::OnEvent(roEvent* evt) {
	if (evt->EventID == E_ID_MODEL) {
		if (((roModelEvent*)evt)->Type == roModelEvent::roEVENT_MODEL_TYPE::CREATE) {
			std::wstring path = ((roModelEvent*)evt)->ModelPath;
			LoadModel(path);
		}
 	}
	else if (evt->EventID == E_ID_RENTITY) {
		if (((roREntityEvent*)evt)->Type == roREntityEvent::CREATE) {
			std::string meshname;
			meshname = ((roREntityEvent*)evt)->MeshRefName;
			AddREntity(meshname);
		}
	}
}

void roGraphicsManager::LoadModel(std::wstring modelpath) {

	std::string resolveName;

	resolveName = ConvertToByteString(modelpath);
	int backslashIdx = resolveName.size();
	for (int i = resolveName.size(); i > 0; --i) {
		backslashIdx = i;
		if (resolveName[i] == '\\')
			break;
	}	
	resolveName.erase(resolveName.begin(), resolveName.end() - ((resolveName.size() - backslashIdx) - 1));
	resolveName.erase(resolveName.size() - 4);

	auto geometry = roObjModelLoader::LoadModelFromObjFile(ConvertToByteString(modelpath));

 	roMesh objMesh;
	objMesh.SetVertexAtrributes(geometry.Attrubutes.bHasNormal, geometry.Attrubutes.bHasTex);

	if (!geometry.Attrubutes.bHasNormal && !geometry.Attrubutes.bHasTex) {

		std::vector<RingedOrb::Vertex> modelVetices;

		for (auto& iter : geometry.Vertices) {
			RingedOrb::Vertex vert;
			vert.position = iter.position;
			modelVetices.push_back(vert);
		}

		objMesh.UploadGeometryData(&modelVetices[0], sizeof(RingedOrb::Vertex) * modelVetices.size(),
			sizeof(RingedOrb::Vertex), &geometry.Indices[0], sizeof(UINT) * geometry.Indices.size(), sizeof(UINT), true);
	}
	else if (geometry.Attrubutes.bHasNormal && !geometry.Attrubutes.bHasTex) {

		std::vector<RingedOrb::Vertex_N> modelVetices;

		for (auto& iter : geometry.Vertices) {
			RingedOrb::Vertex_N vert;
			vert.position = iter.position;
			vert.normal = iter.normal;
			modelVetices.push_back(vert);
		}

		objMesh.UploadGeometryData(&modelVetices[0], sizeof(RingedOrb::Vertex_N) * modelVetices.size(),
			sizeof(RingedOrb::Vertex_N), &geometry.Indices[0], sizeof(UINT) * geometry.Indices.size(), sizeof(UINT), true);
	}
	else if (geometry.Attrubutes.bHasNormal && geometry.Attrubutes.bHasTex) {

		std::vector<RingedOrb::Vertex_NT> modelVetices;

		for (auto& iter : geometry.Vertices) {
			RingedOrb::Vertex_NT vert;
			vert.position = iter.position;
			vert.normal = iter.normal;
			vert.textcoord = iter.UV;
			modelVetices.push_back(vert);
		}

		objMesh.UploadGeometryData(&modelVetices[0], sizeof(RingedOrb::Vertex_NT) * modelVetices.size(),
			sizeof(RingedOrb::Vertex_NT), &geometry.Indices[0], sizeof(UINT) * geometry.Indices.size(), sizeof(UINT), true);
	}


	roMeshInfo meshInfoModel;
	meshInfoModel.BaseVertexLocation = 0;
	meshInfoModel.IndexCount = geometry.Indices.size();
	meshInfoModel.StartIndexLocation = 0;
	meshInfoModel.VertexCount = geometry.Vertices.size();

	objMesh.MeshInfo.push_back(meshInfoModel);

	roGraphicsResourceLoader::LoadMesh(objMesh, resolveName);

	roModelEvent modelEvt;
	modelEvt.Name = resolveName;
	modelEvt.Type = roModelEvent::STORED;

	roEventNode::PushEvent(modelEvt);
}

void roGraphicsManager::AddREntity(std::string meshName) {

	roRenderableEntity rEntityModel;

	rEntityModel.Material = roGraphicsResourceGet::Material("default");
	rEntityModel.Mesh = roGraphicsResourceGet::Mesh(meshName);

	bool hasNormal    = rEntityModel.Mesh->GetNormalAtrributeState();
	bool hasTexCoord  = rEntityModel.Mesh->GetUVAttributeState();

	rEntityModel.PrimitiveTopologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DirectX::XMStoreFloat4x4(&rEntityModel.WorldTransform, DirectX::XMMatrixIdentity());

	if (!hasNormal && !hasTexCoord)
		AddREntityOpaque(rEntityModel, RET_OPAQUE_MAT);
	else if (hasNormal && !hasTexCoord)
		AddREntityOpaque(rEntityModel, RET_OPAQUE_LIGHT_MAT);
	else if (hasNormal && hasTexCoord)
		AddREntityOpaque(rEntityModel, RET_OPAQUE_LIGHT_MAT_TEX);

	roGraphicsRenderer::CopyRenderableEntitiesOpaque(m_OpaqueREntities);
	roGraphicsRenderer::CopyRenderableEntities(m_REntitiesPool);
}

void roGraphicsManager::AddREntityOpaque(roRenderableEntity& entity, R_ENTITY_TYPE_OPAQUE reOpaqueType) {

	roRenderableEntity* pEntity = new roRenderableEntity();
	*pEntity = entity;
	pEntity->ID = m_IDGenerator.GetID();
	m_REntitiesPool.push_back(pEntity);

	m_OpaqueREntities[reOpaqueType].push_back(pEntity);
}

void roGraphicsManager::DeleteREntities() {
	for (auto& enity : m_REntitiesPool) {
		SAFE_DELETE(enity);
	}
}

void roGraphicsManager::DeleteREntity() {

}