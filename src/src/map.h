#pragma once

#include <stdint.h>

#include "tile.h"
#include "randommt.h"
#include "perlinnoise.h"

class Map
{
public:
	Map();
	~Map();
	
	static Map &Instance();
	
	void SetSeed(const uint64_t seed);
	void SetSize(const uint64_t size);
	
	uint64_t GetSize();
	
	void UpdateWorldPosition(const uint64_t worldx, const uint64_t worldy);
	//void UpdateLocationPosition(const uint64_t worldx, const uint64_t worldy, const uint64_t locationx, const uint64_t locationy);
	
	Tile GetTile(const uint64_t worldx, const uint64_t worldy);
	void DrawTile(const uint64_t worldx, const uint64_t worldy, const int64_t screenx, const int64_t screeny);
	//char *GetGlyph(const uint64_t worldx, const uint64_t worldy);
	// GetColor
	
	uint64_t WrapCoordinate(int64_t coordinate) const;
	
	uint8_t GetTerrainType(const uint64_t worldx, const uint64_t worldy, const bool usecached) const;	// land or water

	Tile ComputeTile(const uint64_t worldx, const uint64_t worldy);

	bool MoveBlocked(const uint64_t sourcex, const uint64_t sourcey, const uint64_t destx, const uint64_t desty);

	float ComputeAngle(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const;
	float ComputeDistance(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const;
	float ComputeDistanceSq(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const;
	
private:

	uint8_t GetLandMask(const uint64_t worldx, const uint64_t worldy) const;
	bool LandMaskSet(const uint8_t landmask, const uint8_t checkmask) const;

	// unwraps coordinates (as needed) so distance can be calculated between them
	void UnwrapCoordinates(const int64_t coord1, const int64_t coord2, int64_t &unwrappedcoord1, int64_t &unwrappedcoord2) const;

	uint64_t m_seed;
	uint16_t m_worldsize;
	RandomMT m_rand;
	PerlinNoise m_perlin;
	bool m_worldbuffer;
	Tile m_buffer[324];	// 18x18
	int32_t m_bufferdim;
	uint64_t m_bufferstartx;
	uint64_t m_bufferstarty;
};
