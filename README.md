# HexaSphereGeneratorTest
---
A generator for vertex and triangle arrays of hexagon-tiled spheres.  
Given a number N, generates a sphere of `10*(N^2-1)` hexagonal tiles and 12 pentagonal tiles.   
Returns as output:   
- an array of vertex positions on the unit sphere (array size: `20*N^2`, `*4` in case of walls),  
- an array of triangles, made from 3 vertex indices each (array size: `to be determined`),
- an optional neighbour map, made from the general indices 6 (or 5 in case of the pentagons) neighbours.
- Optionally includes extra vertices and faces for sidewalls per tile.

>  ---
> ### Currently work in progress
> Triangle array and neighbour map are in development, vertex array is (mostly) functional.
> 
>  ---

## Known issues:
- Top and bottom half triangles have an inverted winding order,
- Pentagons and hexagons on the edges of dodecahedral faces aren't included in triangle array,
- Optional neighbour map is missing,
- Neighbour map and wall vertices and faces aren't optional,
- Value of N is hardcoded,

## Tile indexing:
Here will follow a small explanation on how tiles are indexed in the vertex array.    
Indexing vertices per tile can be used to manipulate tile height or get positions of tiles for other purpouses.  
The neighbour map will be made from tile indices.  

*This explanation will likely be added once the neighnour map generation is funcitonal.*
