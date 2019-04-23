#pragma once
#include "..\Core\pch.h"
#include "roViewProjection.h"
#include "roRenderableEntity.h"

class roGPUContext12;
class roConstantRingBuffer;

struct roGraphicsRenderer {

	struct RenderableEntities {
		std::vector<roRenderableEntity*> rEntities;
		std::vector<roRenderableEntity*> rOpaqueEntities[RET_OPAQUE_COUNT];
	};

public:
	static void Initialize(roConstantRingBuffer* constantRingBuffer);

	static void CopyRenderableEntities(std::vector<roRenderableEntity*>& enties);
	static void CopyRenderableEntitiesOpaque(std::vector<roRenderableEntity*> entities[RET_OPAQUE_COUNT]);
	static void SetWireframeState(bool state);
	static bool GetWireframeState();
	static void UpdateMainPassConstant();
 	static void UpdateRenderableEntities();
	static void Draw(roGPUContext12* mainGpuContext);
	static void Shutdown();
private:
	static void DrawOpaque(roGPUContext12* gpuContext);
	static void DrawOpaqueVertColor(roGPUContext12* gpuContext);
	static void DrawOpaqueLightMat(roGPUContext12* gpuContext);
	static void DrawOpaqueLines(roGPUContext12* gpuContext);

	static void DrawWireframe(roGPUContext12* gpuContext);
private:
	static bool sm_IsCreated;

	static bool sm_WireframeState;
	static roConstantRingBuffer* sm_ConstantRingBuffer;
	static std::vector<roViewProjection> sm_Cameras;
	static UINT64 sm_MainPassRingOffset;

	static RenderableEntities sm_RenderableEntities;
};
