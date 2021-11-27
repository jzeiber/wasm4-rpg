#pragma once

#include <stdint.h>

#include "palette.h"
#include "wasm4.h"

// tile - base terrin type (land/water)
// feature - grass, tree, town, etc.

/*

	terrain type - water/land (1 bit flag)
	loc flag - indoor/outdoor (1 bit flag)
	color - 2 bits
	glyph - 6 bits x, 6 bits y

	GLYPH_VOID
	GLYPH_GRASS1
	GLYPH_STONEWALK1
	GLYPH_STONEWALK2
	GLYPH_STONEWALK3
	GLYPH_GRASS2
	GLYPH_GRASS3
	GLYPH_GRASS4
	GLYPH_EVERGREEN1
	GLYPH_EVERGREEN2
	GLYPH_DECIDUOUS1
	GLYPH_DECIDUOUS2
	GLYPH_DECIDUOUS3
	GLYPH_DECIDUOUS4
	GLYPH_CACTUS1
	GLYPH_CACTUS2
	GLYPH_WEED1
	GLYPH_EVERGREEN3
	GLYPH_DECIDUOUS5
	GLYPH_STONE1
	GLYPH_DEADTREE
	GLYPH_TROCIALTREE

*/

class Tile
{
public:
	Tile();
	~Tile();
	
	enum TerrainType
	{
		TERRAIN_WATER=0,
		TERRAIN_LAND=1
	};
	
	enum LocationType
	{
		LOCATION_OUTDOOR=0,
		LOCATION_INDOOR=1
	};
	
	enum PaletteIndex
	{
		PALETTEINDEX_1=0,
		PALETTEINDEX_2=1,
		PALETTEINDEX_3=2,
		PALETTEINDEX_4=3
	};
	
	/*
	enum Glyph
	{
		GLYPH_VOID=0,
		GLYPH_WATER,
		GLYPH_WATER_LANDCORNER,
		GLYPH_WATER_LAND1SIDE,
		GLYPH_WATER_LAND2SIDE,
		GLYPH_WATER_LAND3SIDE,
		GLYPH_WATER_LAND4SIDE,
		GLYPH_LAND,
		GLYPH_GRASS1,
		GLYPH_GRASS2,
		GLYPH_GRASS3,
		GLYPH_GRASS4,
		GLYPH_TOWN1,
		GLYPH_TOWN2
	};
	*/
	
	enum Feature
	{
		FEATURE_NONE=0,
		FEATURE_GRASS1,
		FEATURE_GRASS2,
		FEATURE_GRASS3,
		FEATURE_GRASS4,
		FEATURE_TOWN1,
		FEATURE_TOWN2
	};
	
	struct DrawData
	{
		uint8_t spriteidxx;
		uint8_t spriteidxy;
		uint32_t blitcolors;
		uint8_t blitflags;
	};
	
	void SetTerrainType(const uint8_t terraintype);
	void SetLocationType(const uint8_t locationtype);
	void SetPaletteIndex(const uint8_t paletteindex);
	void SetFlipX(const bool flipx);
	void SetFlipY(const bool flipy);
	void SetRotate(const bool rotate);
	void SetMovementBlocked(const bool blocked);
	//void SetGlyphByIndex(const uint8_t indexx, const uint8_t indexy);
	//void SetGlyphByType(const uint16_t glyphtype);
	void SetFeature(const uint8_t feature);
	
	uint8_t GetTerrainType() const;
	uint8_t GetLocationType();
	uint8_t GetPaletteIndex();
	bool GetFlipX();
	bool GetFlipY();
	bool GetRotate();
	bool GetMovementBlocked();
	//char *GetGlyph();
	uint8_t GetFeature() const;
	struct Tile::DrawData GetDrawData() const;
	
	//void Draw(const int x, const int y);
	
private:
	uint8_t m_flags;		// terrain type, location, palette index, flipx, flipy, rotate
	uint8_t m_feature;
	
	static constexpr DrawData m_drawdata[]=
	{	/*uint8_t spriteidxx, uint8_t spriteidxy, uint32_t blitcolors, uint8_t blitflags */
		{0,0,0,0},
		{1 ,0 ,PALETTE_WHITE << 4 ,BLIT_1BPP},	// FEATURE_GRASS1
		{5 ,0 ,PALETTE_WHITE << 4 ,BLIT_1BPP},	// FEATURE_GRASS2
		{6 ,0 ,PALETTE_WHITE << 4 ,BLIT_1BPP},	// FEATURE_GRASS3
		{7 ,0 ,PALETTE_WHITE << 4 ,BLIT_1BPP},	// FEATURE_GRASS4
		{5 ,3 ,PALETTE_BROWN << 4 ,BLIT_1BPP},	// FEATURE_TOWN1
		{6 ,3 ,PALETTE_BROWN << 4 ,BLIT_1BPP}	// FEATURE_TOWN2
	};
	
	//uint8_t m_glyphidxy;
	//uint8_t m_glyphidxx;
};
