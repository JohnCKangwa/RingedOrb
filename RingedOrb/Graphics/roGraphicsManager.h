#pragma once
#include "..\Core\roEventSystem.h"
#include "roRenderableEntity.h"
#include "roGeometryGen.h"


class roGraphicsManager : public roEventNode {
public:
	roGraphicsManager(roEventQueue* eventqueue);
	void Initialize();
	void OnEvent(roEvent*) override;
	void LoadModel(std::wstring);
	void AddREntity(std::string meshName);
	void AddREntityOpaque(roRenderableEntity& entity, R_ENTITY_TYPE_OPAQUE reOpaqueType);
	void AddMaterial();
	void DeleteREntities();
	void DeleteREntity();
private:
	std::vector<roRenderableEntity*> m_REntitiesPool;
	std::vector<roRenderableEntity*> m_OpaqueREntities[RET_OPAQUE_COUNT];

	std::mutex m_REManagerMutex;
	roID_Generator m_IDGenerator;
};