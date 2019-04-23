#pragma once
#include <DirectXMath.h>

namespace RingedOrb {

	struct Vertex {
		DirectX::XMFLOAT3 position;
	};

	struct Vertex_N {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	struct Vertex_NT {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 textcoord;
	};

}
