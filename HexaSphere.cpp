#include "HexaSphere.h"


std::array<double, 3> crossProd(std::array<float, 3> a, std::array<float, 3> b) {
	return {
		(a[1] * b[2])- (b[1] * a[2]),
		(a[2] * b[0])- (b[2] * a[0]),
		(a[0] * b[1])- (b[0] * a[1])
	};
}

bool getWindingOrder(std::array<float, 3> a, std::array<float, 3> b, std::array<float, 3> c, std::array<float, 3> source) {
	a[0] *= 4;
	a[1] *= 4;
	a[2] *= 4;
	std::array<double, 3> destination = {
		crossProd(
			{b[0] - a[0], b[1] - a[1], b[2] - a[2]} ,
			{c[0] - b[0], c[1] - b[1], c[2] - b[2]}
		)
	};

	double agreement = (source[0] * destination[0]) + (source[1] * destination[1]) + (source[2] * destination[2]);
	//td::cout << agreement << " _ ["  <<
	//	destination[0] << ", " << destination[1] << ", " << destination[2] << "] _ [" <<
	//	source[0] << ", " << source[1] << ", " << source[2] << "]"
	//	<< std::endl;

	return agreement < 0;
}


void convertRowAndTileIntoBary(int row, int tri, std::array<float, 2>& vec) {
	int rev = (row * 2) - tri;
	//int n3 = 3 * n;
	//bool notOddTri = !(tri & 1);

	// From when this actually did bary coords:
	//vec[0] = (1.f + int(3 * (n - row)) + notOddTri) / n3;
	//vec[1] = (tri + floor(tri / 2) - notOddTri) / n3;
	//vec[2] = (rev + floor(rev / 2) - !(rev & 1)) / n3;
	
	// Old uncompressed way of calculating l2s: 
	//float p[2] = { (1.f + int(3 * (n - row)) + notOddTri) / n3, (tri + floor(tri / 2) - notOddTri) / n3 };
	//vec[0] = p[1] / (p[0] + p[1]);

	vec[0] = (tri + floor(tri / 2) - !(tri & 1)) / (tri + 3 * (n - row) + floor(tri / 2) + 1); // Compressed l2s calculation
	vec[1] = (rev + floor(rev / 2) - !(rev & 1)) / (3*n);
}

inline int translate_data_into_vertIndex(int rowLim, int tri, int face, uint8_t vertexUsageID) {
	return (rowLim + tri - 1 ) * 4 + vertexUsageID + face;
}

std::array<double, 3> slerp(std::array<double, 3> p0, std::array<double, 3>p1, double t) {
	double ang0_cos = p0[0] * p1[0] + p0[1] * p1[1] + p0[2] * p1[2]; // 100ms angle between point after slerp and remainging corner
	double ang0_sin =  1 / sqrt(1 - ang0_cos * ang0_cos);  // 0 ms damn...
	double ang0 = acos(ang0_cos); // 250 ms acos(ancos) used to be arctan(ansin/ancos) which used to be atan2(ansin, ancos)
	double l0 = sin((1 - t) * ang0) * ang0_sin; // 100 ms
	double l1 = sin(t * ang0) * ang0_sin; // 100 ms

	return {	
		(l0 * p0[0] + l1 * p1[0]),
		(l0 * p0[1] + l1 * p1[1]),
		(l0 * p0[2] + l1 * p1[2]),
	};
}

std::array<double, 3> cheap_slerp(std::array<double, 3> p0, std::array<double, 3> p1, double t) {
	constexpr double ang0_sin = 1/0.89442719099991586;
	constexpr double ang0 = 1.1071487177940904;
	double l0 = sin((1 - t) * ang0)* ang0_sin; 
	double l1 = sin(t * ang0)* ang0_sin; 

	return {
		(l0 * p0[0] + l1 * p1[0]),
		(l0 * p0[1] + l1 * p1[1]),
		(l0 * p0[2] + l1 * p1[2]),
	};
}

std::array<float, 3> convert_barycentric_into_absolute(std::array<float, 2> p, std::array<double, 3> s1, std::array<double, 3> s2, std::array<double, 3> s3) {

	//double l2s = p[1] / (p[0] + p[1]); // Can be optimized away, this is a value calculated by only the bary coords.
	// So the bary coords doesn't need to be 3 bary coords it can be the L2s value and the third bary coord.

	//std::array<double, 3> p12 = slerp(s1, s2, p[0]);
	std::array<double, 3> p12 = cheap_slerp(s1, s2, p[0]);  // Should be faster, but the compiler probably knows more and has made it obsolete.

	std::array<double, 3> result = slerp(p12, s3, p[1]);

	return { float(result[0]), float(result[1]), float(result[2]) };
};


void HexaSphere::GetInBoundVertIndices(int& I) 
{
	constexpr int lim = HEX_COUNT(n); // Limit of the total triangle.
	// constexpr int nSqr = n * n; // The amount of tris in a face. aka n^2.
	constexpr int nSqr4 = 4*nSqr; // The amount of tris in a face when including vertex usage redudancy. aka 4n^2.
	int R = 2; // Index of row (row indices start at 1)
	int T = 0; // 2*Tile index in row
	// int I = 0 // The starting index within the hexList
	int RLim = 1; // Limit of the current row

	for (int i = 0; i < lim; i++) {
		int F = 0;

		for (int f = 0; f < 20; f ++) {
			hexList[F + I]     = translate_data_into_vertIndex(RLim,     1 + T, f*nSqr4, T!=0);
			hexList[F + I + 1] = translate_data_into_vertIndex(RLim,     2 + T, f*nSqr4, 0);
			hexList[F + I + 2] = translate_data_into_vertIndex(RLim,     3 + T, f*nSqr4, 0);
			hexList[F + I + 3] = translate_data_into_vertIndex(RLim + (2*R-1), 2 + T, f*nSqr4, R==n-1? 0 : T==0? 1 : 2);
			hexList[F + I + 4] = translate_data_into_vertIndex(RLim + (2*R-1), 3 + T, f*nSqr4, R==n-1? 0 : 2);
			hexList[F + I + 5] = translate_data_into_vertIndex(RLim + (2*R-1), 4 + T, f*nSqr4, 1);
			F += lim*6;		

		}

		I += 6;
		if (4+T < 2*R) {
			T+= 2;
		}
		else {
			RLim += 2*R - 1;
			R++;
			T = 0;
		}

	}
	I = lim*20*6;
};

void HexaSphere::GetBorderVertIndices(int& I) 
{
	constexpr int lim = n-1; // The amount of tiles on an edge.
	// constexpr int nSqr = n*n; // The amount of tris in a face. aka n^2.
	constexpr int nSqr4 = 4*nSqr; // The amount of tris in a face when including vertex usage redudancy. aka 4n^2.
	constexpr int n_1sqr = (n-1)*(n-1); // The amount of tris in a face excluding the final row.
	constexpr int n_1sqr4 = 4*n_1sqr; // The amount of tris in a face excluding the final row when including vertex usage redundancy.

	for (int e = 0; e < 30; e++) { // For each edge
		uint8_t faces[2] = { edgeOrderFaces[e][0], edgeOrderFaces[e][1] };
		uint8_t face_edgeIDs[2] = { edgeOrderEdgeIDs[e][0], edgeOrderEdgeIDs[e][1] };
		bool face_inverseWinds[2] = { 0, edgeOrderWinding[e]};

		// Known faults:
		// Maybe (non proven) the Top vert vertex usage ID needs to check with capital T rather than t, but this will only show on full scale tests.

		for (int t = 0; t < lim; t++) { // For each tile on the edge

			for (int f = 0; f < 2; f++) {
				// Do the vert calc for face 0
				int Foffset = faces[f] * nSqr4;  // The offset within the vertex list dictated by the face index.

				if (face_edgeIDs[f] == 0) {
					int T = face_inverseWinds[f]? n-t-2 : t; // Index of the tile along the edge, accounting for winding order in the face.
					int Tsqr = T * T;
					int T_1sqr = (T+1) * (T+1);
					hexList[f?I+3:I+0] = Tsqr          * 4 + Foffset + 2;		             // Left
					hexList[f?I+4:I+1] = (T_1sqr + 1)  * 4 + Foffset + (T == 0 ? 1 : 2);     // Top
					hexList[f?I+5:I+2] = T_1sqr        * 4 + Foffset + 1;                    // Right
				}
				else if (face_edgeIDs[f] == 1) {

					int T = face_inverseWinds[f]? n-t-2 : t; // Index of the tile along the edge, accounting for winding order in the face.
					hexList[f?I+3:I+0] = (n_1sqr+2*T)     * 4 + Foffset + 2;				 // Left
					hexList[f?I+4:I+1] = (n_1sqr+2*T + 1) * 4 + Foffset + (T == 0? 1 : 2);   // Top
					hexList[f?I+5:I+2] = (n_1sqr+2*T + 2) * 4 + Foffset + 1;                 // Right
				}
				else {
					int T = !face_inverseWinds[f]? n-t-2 : t; // Index of the tile along the edge, accounting for winding order in the face.
					// The calculation for this side is much simpler if it is assumed to be inverted winding order.
					// That is why the value of T is based on inverted winding order (!).
					// That also means that the left-right order is inverted here:
					int T_1sqr = (T + 1) * (T + 1);
					int T_2sqr = (T + 2) * (T + 2);
					hexList[f?I+5:I+2] = (T_1sqr-1)   * 4 + Foffset + 1;                      // Right
					hexList[f?I+4:I+1] = (T_2sqr-2)   * 4 + Foffset + (T==lim-1? 0: 2);       // Top
					hexList[f?I+3:I+0] = (T_2sqr-1)   * 4 + Foffset + 2;		              // Left

				}
			}
			I += 6;
		}
	}
};

void HexaSphere::GetPentVertIndices(int& I)
{
	constexpr int nSqr4 = nSqr * 4;
	constexpr int n8_4 = n * 8 - 4;
	for (int pd = 0; pd < 12; pd++) {
		for (int vd = 0; vd < 5;vd++) {
			uint8_t face   = pentagonFaceOrder[pd][vd][0];
			uint8_t corner = pentagonFaceOrder[pd][vd][1];

			hexList[I] = corner == 0 ? face * nSqr4 : corner == 1 ? ((face + 1) * nSqr4) - n8_4: ((face + 1) * nSqr4) - 4;
			I++;
		}
	}
};

void HexaSphere::GetVertices() 
{
	// Gather the bary centric vertex coordinates on one face.
	// This has been optimized down to calculating some values using the bcv and outputting those instead.
	// It now returns a value called l2s and the third bary coord instead as these directly correlate to the t values in the two slerps.
	std::vector<std::array<float, 2>> baryCentricVertices = std::vector<std::array<float, 2>>(nSqr);
	int i = 0;
	for (int ro = 1; ro < 1 + n; ro++) {
		for (int tr = 1; tr < 2 * ro; tr++) {
			convertRowAndTileIntoBary(ro, tr, baryCentricVertices[i]);
			i++;
		}
	}

	// Copy and paste that for all 20 faces 4 times and convert into vector coordinates to get all used vertices.
	int offset = 0;
	for (int f = 0; f < 20; f++) {
		std::array<double, 3> c[3] = {
			{ pentagonCoordinates[triangleVertices[f][0]][0], pentagonCoordinates[triangleVertices[f][0]][1], pentagonCoordinates[triangleVertices[f][0]][2] },
			{ pentagonCoordinates[triangleVertices[f][1]][0], pentagonCoordinates[triangleVertices[f][1]][1], pentagonCoordinates[triangleVertices[f][1]][2] },
			{ pentagonCoordinates[triangleVertices[f][2]][0], pentagonCoordinates[triangleVertices[f][2]][1], pentagonCoordinates[triangleVertices[f][2]][2] } 
		};
		
		for (int v = 0; v < nSqr; v++) {
			// This is a very slow calculation, I've already optimized it (almost) as far as I can.
			// It is in an acceptable range of runtime now.
			// If it needs to be further optimized, the angles of the slerps are consisten between faces and could be calculated in the "baryCentricVertices" loop.
			auto result = convert_barycentric_into_absolute(baryCentricVertices[v], c[0], c[1], c[2]);
			vertices[offset] = result;
			vertices[offset+1] = result;
			vertices[offset+2] = result;
			vertices[offset+3] = result;
			offset += 4;
		}
	}
};

void HexaSphere::GetTriangles() 
{
	// Time for triangles.
	// Only afterwards can we test if the first getters work...
	/*
	id starts at 0 and is incremented for each hex/pentagon.
	For each pentagon:
		If index is 0, 6, 1 or 7:
				Add the main face tris:
					triangle_list.append([vertex[2], vertex[1], vertex[0]])
					triangle_list.append([vertex[0], vertex[3], vertex[2]])
					triangle_list.append([vertex[4], vertex[3], vertex[0]])

				Add the side wall tris, I think this could be moved out of the if statement...
				for j in range(0, 5):
					decide_and_add_triangle_to_list([i[j], i[j], i[(j+1)%5]], [id, -1, -1])
					decide_and_add_triangle_to_list([i[j], i[(j+1)%5], i[(j+1)%5]], [id, -1, id])
		For all other pentagons:
				Add the main face tris:
					triangle_list.append([vertex[0], vertex[1], vertex[2]])
					triangle_list.append([vertex[2], vertex[3], vertex[0]])
					triangle_list.append([vertex[0], vertex[3], vertex[4]])

				Add the side wall tris, I think this could be moved out of the if statement...
				for j in range(0, 5):
					decide_and_add_triangle_to_list([i[(j+1)%5], i[j], i[j]], [-1, -1, id])
					decide_and_add_triangle_to_list([i[(j+1)%5], i[(j+1)%5], i[j]], [id, -1, id])
			

		for each hex (in bound and edge):
			if the the face index of the first vertex is < 10:
				Add the main face tris:
					triangle_list.append([giif(i[4], id), giif(i[2], id), giif(i[0], id)])
					triangle_list.append([giif(i[0], id), giif(i[3], id), giif(i[4], id)])
					triangle_list.append([giif(i[2], id), giif(i[1], id), giif(i[0], id)])
					triangle_list.append([giif(i[2], id), giif(i[4], id), giif(i[5], id)])

				Prepare the proper vertex ordering. [constexpr]
				b = [0, 1, 2, 5, 4, 3]

				Generate the per-side normal by subtracting the base points in funky orders, this is used for winding order src dir later:
				//pts = [
				//	full_list[giif(i[0], -1)],
				//	full_list[giif(i[1], -1)],
				//	full_list[giif(i[2], -1)],
				//	full_list[giif(i[5], -1)],
				//	full_list[giif(i[4], -1)],
				//	full_list[giif(i[3], -1)],
				//]
				//sides = [
				//	[pts[0][0]-pts[1][0], pts[0][1]-pts[1][1], pts[0][2]-pts[1][2]],
				//	[pts[1][0]-pts[2][0], pts[1][1]-pts[2][1], pts[1][2]-pts[2][2]],
				//	[pts[2][0]-pts[3][0], pts[2][1]-pts[3][1], pts[2][2]-pts[3][2]],
				//	[pts[3][0]-pts[4][0], pts[3][1]-pts[4][1], pts[3][2]-pts[4][2]],
				//	[pts[4][0]-pts[5][0], pts[4][1]-pts[5][1], pts[4][2]-pts[5][2]],
				//	[pts[5][0]-pts[0][0], pts[5][1]-pts[0][1], pts[5][2]-pts[0][2]]
				//]

				Add the wall tris, originally this calced the wall normal and winding order, but it should be stable identically to the main face.
				for j in range(0, 6):  
					The src direction shouldn't be needed as the winding order is stable.
					//src = [sides[j][0] - sides[j-3][0], sides[j][1] - sides[j-3][1], sides[j][2] - sides[j-3][2]] if j & 1 else [sides[j-3][0] - sides[j][0], sides[j-3][1] - sides[j][1], sides[j-3][2] - sides[j][2]]

					k0 = b[j]
					k1 = b[(j+1)%6]
					decide_and_add_triangle_to_list([i[k1], i[k0], i[k0]], [id, -1, -1], src) // This ordering should always be correct.
					decide_and_add_triangle_to_list([i[k1], i[k1], i[k0]], [id, -1, id], src)


					// Nevermind, the order is consistent PER FACE, each face is identical in order, but the order depends different for different n.
					// That means the first face should calculate a LUT to use in all others.
			For all other faced hexes:
				Add the main face tris:
					triangle_list.append([giif(i[0], id), giif(i[2], id), giif(i[4], id)])
					triangle_list.append([giif(i[4], id), giif(i[3], id), giif(i[0], id)])
					triangle_list.append([giif(i[0], id), giif(i[1], id), giif(i[2], id)])
					triangle_list.append([giif(i[5], id), giif(i[4], id), giif(i[2], id)])

				Prepare the proper vertex ordering. [constepr]
				b = [0, 1, 2, 5, 4, 3]

				Generate the per-side normal by subtracting the base points in funky orders, this is used for winding order src dir later:
				//pts = [
				//	full_list[giif(i[0], -1)],
				//	full_list[giif(i[1], -1)],
				//	full_list[giif(i[2], -1)],
				//	full_list[giif(i[5], -1)],
				//	full_list[giif(i[4], -1)],
				//	full_list[giif(i[3], -1)],
				//]
				//sides = [
				//	[pts[0][0]-pts[1][0], pts[0][1]-pts[1][1], pts[0][2]-pts[1][2]],
				//	[pts[1][0]-pts[2][0], pts[1][1]-pts[2][1], pts[1][2]-pts[2][2]],
				//	[pts[2][0]-pts[3][0], pts[2][1]-pts[3][1], pts[2][2]-pts[3][2]],
				//	[pts[3][0]-pts[4][0], pts[3][1]-pts[4][1], pts[3][2]-pts[4][2]],
				//	[pts[4][0]-pts[5][0], pts[4][1]-pts[5][1], pts[4][2]-pts[5][2]],
				//	[pts[5][0]-pts[0][0], pts[5][1]-pts[0][1], pts[5][2]-pts[0][2]]
				//]

				Add the wall tris, originally this calced the wall normal and winding order, but it should be stable identically to the main face.
				for j in range(0, 6): 
					The src direction shouldn't be needed as the winding order is stable.
					//src = [sides[j][0] - sides[j-3][0], sides[j][1] - sides[j-3][1], sides[j][2] - sides[j-3][2]] if j & 1 else [sides[j-3][0] - sides[j][0], sides[j-3][1] - sides[j][1], sides[j-3][2] - sides[j][2]]

					k0 = b[j]
					k1 = b[(j+1)%6]
					decide_and_add_triangle_to_list([i[k0], i[k0], i[k1]], [id, -1, -1], src) // This ordering should always be correct.
					decide_and_add_triangle_to_list([i[k0], i[k1], i[k1]], [id, -1, id], src)
	
	*/


	// First, a loop for all hexes on a face to precalculate the wall directions.
	std::vector<bool> inversionMap = std::vector<bool>(HEX_COUNT(n)*6);
	for (int i = 0; i < 6*HEX_COUNT(n); i+=6) {
		std::array<std::array<float, 3>, 6> pts = { // Reordered!
			vertices[hexList[i + 0]],
			vertices[hexList[i + 1]],
			vertices[hexList[i + 2]],
			vertices[hexList[i + 5]],
			vertices[hexList[i + 4]],
			vertices[hexList[i + 3]],
		};

		std::array<std::array<float, 3>, 6> sides = {
			std::array<float, 3>{pts[0][0] - pts[1][0], pts[0][1] - pts[1][1], pts[0][2] - pts[1][2]},
			std::array<float, 3>{pts[1][0] - pts[2][0], pts[1][1] - pts[2][1], pts[1][2] - pts[2][2]},
			std::array<float, 3>{pts[2][0] - pts[3][0], pts[2][1] - pts[3][1], pts[2][2] - pts[3][2]},
			std::array<float, 3>{pts[3][0] - pts[4][0], pts[3][1] - pts[4][1], pts[3][2] - pts[4][2]},
			std::array<float, 3>{pts[4][0] - pts[5][0], pts[4][1] - pts[5][1], pts[4][2] - pts[5][2]},
			std::array<float, 3>{pts[5][0] - pts[0][0], pts[5][1] - pts[0][1], pts[5][2] - pts[0][2]}
		};

		constexpr int b[9] = { 0, 1, 2, 3, 4, 5, 0, 1, 2}; // The dumbest LUT in existance but its faster than the % operator.
		// B might not be the best name for it though.

		for (int j = 0; j < 6; j++) {
			std::array<float, 3> src = (j & 1) ?
				std::array<float, 3>{
					src[0] = sides[j][0] - sides[b[j + 3]][0],
					src[1] = sides[j][1] - sides[b[j + 3]][1],
					src[2] = sides[j][2] - sides[b[j + 3]][2],
				} : 
				std::array<float, 3>{
					src[0] = sides[b[j + 3]][0] - sides[j][0],
					src[1] = sides[b[j + 3]][1] - sides[j][1],
					src[2] = sides[b[j + 3]][2] - sides[j][2],
				} ;
			inversionMap[i+j] = getWindingOrder(pts[j], pts[j], pts[b[j + 1]], src);
		}
	}


	// Loop over all in-bound hexagons to add their walls to the list.
	// TO-DO: this currently does not take into account top-half inversion.
	int I = 0;
	for (int i = 0; i < 6 * HEX_COUNT(n) * 20; i += 6) { // For hexagon in the hexlist, get its start index
		// Add the four face triangles:
		{
			triangles[I + 0] = hexList[i + 0];
			triangles[I + 1] = hexList[i + 2];
			triangles[I + 2] = hexList[i + 4];

			triangles[I + 3] = hexList[i + 4];
			triangles[I + 4] = hexList[i + 3];
			triangles[I + 5] = hexList[i + 0];

			triangles[I + 6] = hexList[i + 0];
			triangles[I + 7] = hexList[i + 1];
			triangles[I + 8] = hexList[i + 2];

			triangles[I + 9] = hexList[i + 5];
			triangles[I + 10] = hexList[i + 4];
			triangles[I + 11] = hexList[i + 2];

			I += 12;
		}

		// Add the 6 walls:
		{
			std::array<int, 6> pts{
				hexList[i + 0],
				hexList[i + 1],
				hexList[i + 2],
				hexList[i + 5],
				hexList[i + 4],
				hexList[i + 3]
			};
			constexpr int b[9] = { 0, 1, 2, 3, 4, 5, 0, 1, 2 }; // The dumbest LUT in existance but its faster than the % operator.

			// TO-DO: This currently does not take account the inversion map!
			for (int j = 0; j < 6; j++) {
				triangles[I+0] = pts[j]; 
				triangles[I+1] = (pts[j] | 3);   // used to use |4 instead of &~4
				triangles[I+2] = (pts[b[j+1]] | 3);  // used to use |4 instead of &~4
				triangles[I+3] = pts[j]; 
				triangles[I+4] = (pts[b[j+1]] | 3);   // used to use |4 instead of &~4
				triangles[I+5] = pts[b[j+1]];
				I += 6;
			}
		}
	};
};

void HexaSphere::GetNeighbourMap() 
{

};
