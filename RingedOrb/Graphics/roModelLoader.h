#pragma once
#include "..\Core\pch.h"
#include <DirectXMath.h>

class roObjModelLoader {

public:

	roObjModelLoader() = delete;
	roObjModelLoader(const roObjModelLoader& rhs) = delete;
	roObjModelLoader& operator= (const roObjModelLoader& rhs) = delete;

	struct VertexObj{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 UV;
	};

	struct VertexAttributeInfo {
		bool bHasNormal = false;
		bool bHasTex = false;
	};

	struct ObjGeometry {
		std::vector<VertexObj> Vertices;
		std::vector<UINT> Indices;
		VertexAttributeInfo Attrubutes;
	};

	struct VertexInfoObj {
		bool bTex;
		bool bNorm;
		int numVertPos;
		int numVertText;
		int numVertNorm;
		bool foundSpaceOnLF_f_line;
	};

public:
	static ObjGeometry LoadModelFromObjFile(std::string Externalfilename);

private:
	static bool OpenOBJFile(std::string filename, std::ifstream& fin);
	static VertexInfoObj AnalyseVertexDataFromFile(std::string filename);
	static void LoadOBJIndices(std::string filename, VertexInfoObj vertexInfoObj);
	static void ResolveIndices();
	static void ResolveVertices(VertexInfoObj vertexInfoObj);
	static void ResetTempStructures();

private:
	static std::vector<DirectX::XMFLOAT3> sm_temp_vert_position;
	static std::vector<DirectX::XMFLOAT3> sm_temp_vert_normal;
	static std::vector<DirectX::XMFLOAT2> sm_temp_vert_uv;

	static std::vector<int> sm_tempObjIndices;
	static std::vector<int> sm_tempObjIndicesVertexCountPerFace;

	static std::vector<VertexObj> sm_Vertices;
	static std::vector<UINT> sm_Indices;

};