#include "HexaSphere.h"
#include <chrono>

int main(){
	std::cout << "----------<RUN>----------\n" << std::endl;

	// Construct the hexasphere object, reserving most memory.
	auto t = std::chrono::high_resolution_clock::now();
	HexaSphere hs;
	std::cout << "Constructed" << " n=" << n << ": " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;

	// Perform the generation steps:
	
	// Prepare vertex indices:
	t = std::chrono::high_resolution_clock::now();
	hs.GetAllVertIndices();
	std::cout << "GetAllVertIndices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	// Generate vertex array:
	t = std::chrono::high_resolution_clock::now();
	hs.GetVertices();
	std::cout << "GetVertices: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	// Generate triangle array:
	t = std::chrono::high_resolution_clock::now();
	hs.GetTriangles();
	std::cout << "GetTriangles: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;
	// Generate neighbour map:
	t = std::chrono::high_resolution_clock::now();
	hs.GetNeighbourMap();
	std::cout << "GetNeighbourMap: " << (std::chrono::high_resolution_clock::now() - t).count() / 1000000.0 << "ms" << std::endl;

	/*
	
	Still to do: figure out what assumptions can be made to populate the neighbour map.

	Code steps:
	1. Get the vertexIndices for all in bound coordinates and populate its parts of the VU list.
	2. Get the vertexIndices for all edge hex coordinates and populate its parts of the VU list.
	3. Get the vertexIndices for all pentagon coordinates and populate its parts of the VU list.

	4. Create a list of all vertices in barycoordinates.
	5. Copy paste the bary list for each face but translated into vector coordinates.
	
	4. Create the triangles list using the vertexIndices and removing the modulus of 4.
	
	4. Create the neighbour map using the VU list?
	
	Same number steps can be done simultaniously (probably).
	*/

	// Debug: print out the triangle array:
	for (auto i : hs.triangles) {
		std::cout << i << ", ";
	}
	std::cin >> new char; // Await input

	// Debug: print out the vertex array:
	for (auto i : hs.vertices) {
		std::cout << "{" << i[0] << ", " << i[1] << ", " << i[2] << "}, ";
	}
	std::cin >> new char; // Await input

	std::cout << "\n----------<END>----------" << std::endl;
	return 0;
}