#include "roModelLoader.h"

bool roObjModelLoader::OpenOBJFile(std::string filename, std::ifstream& fin) {
	fin.open(filename);
	if (fin.fail())
		return false;
	else
		return true;
}

roObjModelLoader::VertexInfoObj roObjModelLoader::AnalyseVertexDataFromFile(std::string filename) {

	std::ifstream fin;

	if (!OpenOBJFile(filename, fin))
		THROW(L"file open error");

	int vPosCount = 0;
	int vTexCount = 0;
	int vNormCount = 0;

	bool tInvoke = false;
	bool nInvoke = false;

	bool foundSpaceBeforeLineFeed_f_line;

	bool foundLF = false;

	char input;
	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			if (input == ' ') {
				vPosCount++;
			}
			if (input == 't') {
				vTexCount++;
				if (!tInvoke)
					tInvoke = true;
			}
			if (input == 'n') {
				vNormCount++;
				if (!nInvoke)
					nInvoke = true;
			}
		}

		if (!foundLF) {
			if (input == 'f') {
				fin.get(input);
				if (input == ' ') {
					while (!foundLF) {
						fin.get(input);
						if (input == '\n') {
							foundLF = true;
							int tellg = fin.tellg();
							fin.seekg(tellg - 2, std::ios_base::beg);
							char peekNext = fin.peek();
							if (peekNext == '\n')
								foundSpaceBeforeLineFeed_f_line = true;
							else
								foundSpaceBeforeLineFeed_f_line = false;
						}
					}
				}
			}
		}

		while (input != '\n') {
			fin.get(input);
		}

		fin.get(input);
	}

	VertexInfoObj vertexInfoObj;
	vertexInfoObj.bTex = tInvoke;
	vertexInfoObj.bNorm = nInvoke;
	vertexInfoObj.numVertPos = vPosCount;
	vertexInfoObj.numVertText = vTexCount;
	vertexInfoObj.numVertNorm = vNormCount;
	vertexInfoObj.foundSpaceOnLF_f_line = foundSpaceBeforeLineFeed_f_line;

	fin.close();
	return vertexInfoObj;
}

void roObjModelLoader::ResetTempStructures() {

	sm_temp_vert_position.clear();
	sm_temp_vert_normal.clear();
	sm_temp_vert_uv.clear();
	sm_tempObjIndices.clear();
	sm_tempObjIndicesVertexCountPerFace.clear();
}

void roObjModelLoader::LoadOBJIndices(std::string filename, VertexInfoObj vertexInfoObj) {
	std::ifstream fin;
	if (!OpenOBJFile(filename, fin));

	char input;

	sm_tempObjIndices.reserve(1000000);

	fin.get(input);
	while (!fin.eof()) {

		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {

				int posIdx, normIdx, texIdx; char skip;

				bool IsNewLine = false;
				int vertCount = 0;
				char NextChar;


				while (!IsNewLine) {

					if (vertexInfoObj.bNorm && vertexInfoObj.bTex) {
						fin >> posIdx >> skip >> texIdx >> skip >> normIdx;

						sm_tempObjIndices.emplace_back(posIdx);
						sm_tempObjIndices.emplace_back(texIdx);
						sm_tempObjIndices.emplace_back(normIdx);

						if (vertexInfoObj.foundSpaceOnLF_f_line) {
							fin.get(input);
							NextChar = fin.peek();
						}
						else {
							NextChar = fin.peek();
						}
					}
					else if (!vertexInfoObj.bNorm && vertexInfoObj.bTex) {
						fin >> posIdx >> skip >> texIdx;

						sm_tempObjIndices.push_back(posIdx);
						sm_tempObjIndices.push_back(texIdx);

						if (vertexInfoObj.foundSpaceOnLF_f_line) {
							fin.get(input);
							NextChar = fin.peek();
						}
						else {
							NextChar = fin.peek();
						}
					}
					else if (vertexInfoObj.bNorm && !vertexInfoObj.bTex) {
						fin >> posIdx >> skip >> skip >> normIdx;

						sm_tempObjIndices.push_back(posIdx);
						sm_tempObjIndices.push_back(normIdx);

						if (vertexInfoObj.foundSpaceOnLF_f_line) {
							fin.get(input);
							NextChar = fin.peek();
						}
						else {
							NextChar = fin.peek();
						}
					}
					else if (!vertexInfoObj.bNorm && !vertexInfoObj.bTex) {
						fin >> posIdx ;

						sm_tempObjIndices.push_back(posIdx);

						if (vertexInfoObj.foundSpaceOnLF_f_line) {
							fin.get(input);
							NextChar = fin.peek();
						}
						else {
							NextChar = fin.peek();
						}
					}

					vertCount++;

					if (NextChar == '\n') {
						IsNewLine = true;
					}
				}

				sm_tempObjIndicesVertexCountPerFace.push_back(vertCount);
			}

		}

		while (input != '\n') {
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

}

void roObjModelLoader::ResolveIndices() {
	UINT indexCurrPos = 0;

	for (int faceIndex = 0; faceIndex < sm_tempObjIndicesVertexCountPerFace.size(); faceIndex++) {

		int vertexCount = sm_tempObjIndicesVertexCountPerFace[faceIndex];

		int indexCount = (vertexCount - 3) * 3 + 3;
		std::vector<UINT> indices;

		UINT stepRate = 0;
		UINT indexCurrPosCache = indexCurrPos;

		bool IsNotEndFaceIndex = true;

		while (IsNotEndFaceIndex) {
			indices.push_back(indexCurrPos);
			indexCurrPos = indexCurrPos + stepRate + 1;
			indices.push_back(indexCurrPos);
			indexCurrPos = indexCurrPos + 1;
			indices.push_back(indexCurrPos);

			if ((stepRate * 3 + 3) == indexCount) {
				IsNotEndFaceIndex = false;
			}

			indexCurrPos = indexCurrPosCache;
			stepRate += 1;
		}

		indexCurrPos += vertexCount;

		int index_temp_size_count = indices.size();
		for (int i = 0; i < index_temp_size_count; i++) {
			auto index = indices.back();
			indices.pop_back();
			sm_Indices.push_back(index);
		}
	}
}

void roObjModelLoader::ResolveVertices(VertexInfoObj vertexInfoObj) {

	int ObjIndicesIdx = 0;

	for (int faceIndex = 0; faceIndex < sm_tempObjIndicesVertexCountPerFace.size(); faceIndex++) {

		int currFace = sm_tempObjIndicesVertexCountPerFace[faceIndex];

		for (int vertIdx = 0; vertIdx < currFace; vertIdx++) {

			if (vertexInfoObj.bNorm && vertexInfoObj.bTex) {

				int pos_curr = sm_tempObjIndices[ObjIndicesIdx];
				int uv_curr = sm_tempObjIndices[++ObjIndicesIdx];
				int norm_curr = sm_tempObjIndices[++ObjIndicesIdx];

				VertexObj vert;
				vert.position = sm_temp_vert_position[pos_curr - 1];
				vert.UV = sm_temp_vert_uv[uv_curr - 1];
				vert.normal = sm_temp_vert_normal[norm_curr - 1];

				sm_Vertices.push_back(vert);

				ObjIndicesIdx++;
			}
			else if (!vertexInfoObj.bNorm && vertexInfoObj.bTex) {

				int pos_curr = sm_tempObjIndices[ObjIndicesIdx];
				int uv_curr = sm_tempObjIndices[++ObjIndicesIdx];

				VertexObj vert;
				vert.position = sm_temp_vert_position[pos_curr - 1];
				vert.UV = sm_temp_vert_uv[uv_curr - 1];

				sm_Vertices.push_back(vert);

				ObjIndicesIdx++;

			}
			else if (vertexInfoObj.bNorm && !vertexInfoObj.bTex) {

				int pos_curr = sm_tempObjIndices[ObjIndicesIdx];
				int norm_curr = sm_tempObjIndices[++ObjIndicesIdx];

				VertexObj vert;

				vert.position = sm_temp_vert_position[pos_curr - 1];
				vert.normal = sm_temp_vert_normal[norm_curr - 1];

				sm_Vertices.push_back(vert);

				ObjIndicesIdx++;
			}
			else if (!vertexInfoObj.bNorm && !vertexInfoObj.bTex) {

				int pos_curr = sm_tempObjIndices[ObjIndicesIdx];

				VertexObj vert;

				vert.position = sm_temp_vert_position[pos_curr - 1];

				sm_Vertices.push_back(vert);

				ObjIndicesIdx++;
			}
		}
	}
}

roObjModelLoader::ObjGeometry roObjModelLoader::LoadModelFromObjFile(std::string externalFilename) {

	std::string filename = externalFilename;

	ResetTempStructures();

	std::string filenameIndices = filename;
	filenameIndices.erase(filenameIndices.size() - 4, filenameIndices.size());
	filenameIndices += "Temp.idx";

	CopyFile(ConvertToMultiByteString(filename).c_str(), ConvertToMultiByteString(filenameIndices).c_str(), false);

	VertexInfoObj vertexInfoObj = AnalyseVertexDataFromFile(filename);

	auto indicesLoaderFunc = [&]()->void {LoadOBJIndices(filenameIndices, vertexInfoObj); };
	std::thread indicesThread(indicesLoaderFunc);

	sm_Vertices.clear();
	sm_Indices.clear();
	sm_Vertices.reserve(500000);
	sm_Indices.reserve(500000);

	sm_temp_vert_position.reserve(vertexInfoObj.numVertPos);
	sm_temp_vert_normal.reserve(vertexInfoObj.numVertNorm);
	sm_temp_vert_uv.reserve(vertexInfoObj.numVertText);

	std::ifstream fin;
	char input;

	if (!OpenOBJFile(filename, fin))
		THROW(L"file open error");

	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') {
				
				DirectX::XMFLOAT3 pos;
				fin >> pos.x >> pos.y >> pos.z;

				DirectX::XMFLOAT3 position = pos;
				position.y = pos.z;
				position.z = pos.y;

				sm_temp_vert_position.emplace_back(position);
			}
			if (input == 't') {
				DirectX::XMFLOAT2 tex;
				fin >> tex.x >> tex.y;
				tex.y = 1.0f - tex.y;

				sm_temp_vert_uv.emplace_back(tex);
			}
			if (input == 'n') {
				DirectX::XMFLOAT3 normal;
				fin >> normal.x >> normal.y >> normal.z;

				DirectX::XMFLOAT3 normal_;
				normal_ = normal;

				normal.z = normal_.y;
				normal.y = normal_.z;
 
				sm_temp_vert_normal.emplace_back(normal);
			}
		}

		while (input != '\n') {
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	indicesThread.join();
	DeleteFile(ConvertToMultiByteString(filenameIndices).c_str());

	auto ResolveIndicesFunc = [&]()->void {ResolveIndices(); };
	std::thread indicesResolveThread(ResolveIndicesFunc);
	ResolveVertices(vertexInfoObj);

	indicesResolveThread.join();

	VertexAttributeInfo vertAttribInfo{ };

	if (!sm_temp_vert_normal.empty())
		vertAttribInfo.bHasNormal = true;
	if (!sm_temp_vert_uv.empty())
		vertAttribInfo.bHasTex = true;

	ResetTempStructures();
	
	ObjGeometry geometry;
	geometry.Vertices = sm_Vertices;
	geometry.Indices = sm_Indices;
	geometry.Attrubutes = vertAttribInfo;
	
	return geometry;
}

std::vector<DirectX::XMFLOAT3> roObjModelLoader::sm_temp_vert_position;
std::vector<DirectX::XMFLOAT3> roObjModelLoader::sm_temp_vert_normal;
std::vector<DirectX::XMFLOAT2> roObjModelLoader::sm_temp_vert_uv;

std::vector<int> roObjModelLoader::sm_tempObjIndices;
std::vector<int> roObjModelLoader::sm_tempObjIndicesVertexCountPerFace;

std::vector<roObjModelLoader::VertexObj> roObjModelLoader::sm_Vertices;
std::vector<UINT> roObjModelLoader::sm_Indices;
