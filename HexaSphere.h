#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <chrono>


constexpr int HEX_COUNT(const int x) {
	return ((x - 2) * (x - 1)) / 2;
}

constexpr int n = 15; // Limit: 5982. because the size of the hexList would be just barely below 2^31-1 (2_147_483_647).
// Actually, 5181. because the size of the vertices array is 33% larger and reaches the limit sooner.

constexpr int nSqr =  n * n;

constexpr double pentagonCoordinates[12][3] = {
	{0, 0, 1}, {0.8944271909999159, 0.0, 0.4472135954999579}, {0.27639320225002106, 0.8506508083520399, 0.4472135954999579},
	{-0.7236067977499788, 0.5257311121191337, 0.4472135954999579}, {-0.7236067977499789, -0.5257311121191335, 0.4472135954999579},
	{0.27639320225002084, -0.85065080835204, 0.4472135954999579}, {0, 0, -1}, {-0.8944271909999159, 0.0, -0.4472135954999579},
	{-0.27639320225002106, 0.8506508083520399, -0.4472135954999579}, {0.7236067977499788, 0.5257311121191337, -0.4472135954999579},
	{0.7236067977499789, -0.5257311121191335, -0.4472135954999579}, {-0.27639320225002084, -0.85065080835204, -0.4472135954999579}
};

constexpr uint8_t triangleVertices[20][3] = {
	{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5}, {0, 5, 1},
	{6, 7, 8}, {6, 8, 9}, {6, 9, 10}, {6, 10, 11}, {6, 11, 7},
	{1, 2, 9}, {2, 3, 8}, {3, 4, 7}, {4, 5, 11}, {5, 1, 10},
	{1, 9, 10}, {2, 8, 9}, {3, 7, 8}, {4, 11, 7}, {5, 10, 11}
};

constexpr uint8_t pentagonFaceOrder[12][5][2] = { // Dictates the face and corner index for each vertex in each pentagon.
	{{0, 0}, {1, 0}, {2 , 0}, {3 , 0}, {4 , 0}},
	{{0, 1}, {4, 2}, {14, 1}, {15, 0}, {10, 0}},
	{{0, 2}, {1, 1}, {11, 0}, {16, 0}, {10, 1}},
	{{1, 2}, {2, 1}, {12, 0}, {17, 0}, {11, 1}},
	{{2, 2}, {3, 1}, {13, 0}, {18, 0}, {12, 1}},
	{{3, 2}, {4, 1}, {14, 0}, {19, 0}, {13, 1}},
	{{5, 0}, {6, 0}, {7 , 0}, {8 , 0}, {9 , 0}},
	{{5, 1}, {9, 2}, {18, 2}, {12, 2}, {17, 1}},
	{{5, 2}, {6, 1}, {16, 1}, {11, 2}, {17, 2}},
	{{6, 2}, {7, 1}, {15, 1}, {10, 2}, {16, 2}},
	{{7, 2}, {8, 1}, {19, 1}, {14, 2}, {15, 2}},
	{{8, 2}, {9, 1}, {18, 1}, {13, 2}, {19, 2}}
};

constexpr uint8_t edgeOrderFaces[30][2] = {
	{0, 4},
	{0, 1},
	{0, 10},
	{1, 2},
	{1, 11},
	{2, 3},
	{2, 12},
	{3, 4},
	{3, 13},
	{4, 14},
	{5, 9},
	{5, 6},
	{5, 17},
	{6, 7},
	{6, 16},
	{7, 8},
	{7, 15},
	{8, 9},
	{8, 19},
	{9, 18},
	{10, 15},
	{10, 16},
	{11, 16},
	{11, 17},
	{12, 17},
	{12, 18},
	{13, 18},
	{13, 19},
	{14, 19},
	{14, 15}
};

constexpr uint8_t edgeOrderEdgeIDs[30][2] = {
	{0, 2},
	{2, 0},
	{1, 0},
	{2, 0},
	{1, 0},
	{2, 0},
	{1, 0},
	{2, 0},
	{1, 0},
	{1, 0},
	{0, 2},
	{2, 0},
	{1, 1},
	{2, 0},
	{1, 1},
	{2, 0},
	{1, 1},
	{2, 0},
	{1, 1},
	{1, 1},
	{2, 0},
	{1, 2},
	{2, 0},
	{1, 2},
	{2, 0},
	{1, 2},
	{2, 0},
	{1, 2},
	{2, 0},
	{1, 2}
};

constexpr bool edgeOrderWinding[30] = {
	true,
	true,
	false,
	true,
	false,
	true,
	false,
	true,
	false,
	false,
	true,
	true,
	false,
	true,
	false,
	true,
	false,
	true,
	false,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true
};


class HexaSphere {
public:

	HexaSphere() {};

	// The list of vertex indices (final).
	std::vector<unsigned int> hexList = std::vector<unsigned int>(HEX_COUNT(n) * 120 + 180 * (n - 1) + 60);
	// The list of vertices in coordinates (final).
	std::vector<std::array<float, 3>> vertices = std::vector<std::array<float, 3>>(20*4*nSqr);
	// The list of vertex indices ordered as triangles.
	std::vector<unsigned int> triangles = std::vector<unsigned int>(HEX_COUNT(n)*48*20);

	void GetAllVertIndices() {
		int I = 0;
		auto t = std::chrono::high_resolution_clock::now();
		GetInBoundVertIndices(I);
		std::cout << "GetInBoundVertIndices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
		t = std::chrono::high_resolution_clock::now();
		GetBorderVertIndices(I);
		std::cout << "GetBorderVertIndices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
		t = std::chrono::high_resolution_clock::now();
		GetPentVertIndices(I);
		std::cout << "GetPertagonVertIndices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	};

	void GetInBoundVertIndices(int& I);
	void GetBorderVertIndices(int& I);
	void GetPentVertIndices(int& I);

	void GetVertices();

	void GetTriangles();

	void GetNeighbourMap();

};