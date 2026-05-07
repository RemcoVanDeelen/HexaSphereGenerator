#include "HexaSphere.h"
#include <chrono>

int main(){
	std::cout << "----------<RUN>----------\n" << std::endl;

	auto t = std::chrono::high_resolution_clock::now();
	HexaSphere hs;
	std::cout << "Constructed" << " n=" << n << ": " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;

	t = std::chrono::high_resolution_clock::now();
	hs.GetAllVertIndices();
	std::cout << "GetAllVertIndices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	t = std::chrono::high_resolution_clock::now();
	hs.GetVertices();
	std::cout << "GetVertices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	t = std::chrono::high_resolution_clock::now();
	hs.GetTriangles();
	std::cout << "GetTriangles: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	t = std::chrono::high_resolution_clock::now();
	hs.GetNeighbourMap();
	std::cout << "GetNeighbourMap: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;

	/*
	
	Step two: figure out what assumptions can be made to populate the neighbour map.
	step tre: code.

	Code:
	2. Get the vertexIndices for all in bound coordinates and populate its parts of the VU list.
	3. Get the vertexIndices for all edge hex coordinates and populate its parts of the VU list.
	1. Get the vertexIndices for all pentagon coordinates and populate its parts of the VU list.

	4. Create a list of all vertices in barycoordinates.
	5. Copy paste the bary list for each face but translated into vector coordinates.
	
	4. Create the triangles list using the vertexIndices and removing the modulus of 4.
	
	4. Create the neighbour map using the VU list?
	
	*/

	for (auto i : hs.triangles) {
		std::cout << i << ", ";
	}
	std::cin >> new char;

	for (auto i : hs.vertices) {
		std::cout << "{" << i[0] << ", " << i[1] << ", " << i[2] << "}, ";
	}
	std::cin >> new char;

	std::cout << "\n----------<END>----------" << std::endl;
	return 0;
}