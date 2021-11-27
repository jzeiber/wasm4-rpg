#include "tile.h"
//#include "spriteterrain.h"
#include "wasm4.h"

Tile::Tile()
{
	m_flags=0;
	m_feature=FEATURE_NONE;
	//m_glyphidxx=0;
	//m_glyphidxy=0;
}

Tile::~Tile()
{

}

void Tile::SetTerrainType(const uint8_t terraintype)
{
	m_flags=(m_flags & 0x7f) | ((terraintype & 0x1) << 7);
}

void Tile::SetLocationType(const uint8_t locationtype)
{
	m_flags=(m_flags & 0xbf) | ((locationtype & 0x1) << 6);
}

void Tile::SetPaletteIndex(const uint8_t paletteindex)
{
	m_flags=(m_flags & 0xcf) | ((paletteindex & 0x3) << 4);
}

void Tile::SetFlipX(const bool flipx)
{
	if(flipx==true)
	{
		m_flags=m_flags | (0x1 << 3);
	}
	else
	{
		m_flags=m_flags & ~(0x1 << 3);
	}
}

void Tile::SetFlipY(const bool flipy)
{
	if(flipy==true)
	{
		m_flags=m_flags | (0x1 << 2);
	}
	else
	{
		m_flags=m_flags & ~(0x1 << 2);
	}
}

void Tile::SetRotate(const bool rotate)
{
	if(rotate==true)
	{
		m_flags=m_flags | (0x1 << 1);
	}
	else
	{
		m_flags=m_flags & ~(0x1 << 1);
	}
}

void Tile::SetMovementBlocked(const bool blocked)
{
	if(blocked==true)
	{
		m_flags=m_flags | 0x1;
	}
	else
	{
		m_flags=m_flags & ~(0x1);
	}
}

/*
void Tile::SetGlyphByIndex(const uint8_t indexx, const uint8_t indexy)
{
	m_glyphidxx=indexx;
	m_glyphidxy=indexy;
}

void Tile::SetGlyphByType(const uint16_t glyphtype)
{
	// TODO - 
	switch(glyphtype)
	{
	case GLYPH_WATER:
		m_flags=0;
		SetTerrainType(TERRAIN_WATER);
		SetPaletteIndex(3);
		SetGlyphByIndex(8,5);
		break;
	case GLYPH_WATER_LANDCORNER:
		m_flags=0;
		SetTerrainType(TERRAIN_WATER);
		SetPaletteIndex(3);
		SetGlyphByIndex(11,5);
		break;
	case GLYPH_LAND:
		m_flags=0;
		SetTerrainType(TERRAIN_LAND);
		SetPaletteIndex(2);
		SetGlyphByIndex(8,5);
		break;
	default:
		m_flags=0;
		SetPaletteIndex(1);
		SetGlyphByIndex(0,0);
		break;
	}
}
*/

void Tile::SetFeature(const uint8_t feature)
{
	m_feature=feature;
}
	
uint8_t Tile::GetTerrainType() const
{
	return (m_flags >> 7) & 0x1;
}

uint8_t Tile::GetLocationType()
{
	return (m_flags >> 6) & 0x1;
}

uint8_t Tile::GetPaletteIndex()
{
	return (m_flags >> 4) & 0x3;
}

bool Tile::GetFlipX()
{
	return ((m_flags >> 3) & 0x1)==0x1 ? true : false;
}

bool Tile::GetFlipY()
{
	return ((m_flags >> 2) & 0x1)==0x1 ? true : false;
}

bool Tile::GetRotate()
{
	return ((m_flags >> 1) & 0x1)==0x1 ? true : false;
}

bool Tile::GetMovementBlocked()
{
	return ((m_flags & 0x1)==0x1) ? true : false;
}

/*
char *Tile::GetGlyph()
{
	return (char *)spritesheet+(m_glyphidxy*16*spritesheet_Width)+(m_glyphidxx*16);
}
*/

uint8_t Tile::GetFeature() const
{
	return m_feature;
}

struct Tile::DrawData Tile::GetDrawData() const
{
	if(m_feature>=0)
	{
		return m_drawdata[m_feature];
	}
	return m_drawdata[0];
}

/*
void Tile::Draw(const int x, const int y)
{

	*DRAW_COLORS=GetPaletteIndex();
	uint8_t blitflags=BLIT_1BPP;
	if(GetFlipX()==true)
	{
		blitflags |= BLIT_FLIP_X;
	}
	if(GetFlipY()==true)
	{
		blitflags |= BLIT_FLIP_Y;
	}
	if(GetRotate()==true)
	{
		blitflags |= BLIT_ROTATE;
	}
	blitSub((char *)spritesheet,x,y,16,16,m_glyphidxx*16,m_glyphidxy*16,spritesheet_Width,blitflags);

}
*/