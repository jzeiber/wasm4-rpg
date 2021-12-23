#include "map.h"
#include "palette.h"
#include "spriteterrain.h"
#include "wasmmath.h"
#include "lz4blitter.h"

#include "wasm4.h"

#define LAND_MASK_TOPLEFT		0b10000000
#define LAND_MASK_TOP			0b01000000
#define LAND_MASK_TOPRIGHT		0b00100000
#define LAND_MASK_LEFT			0b00010000
#define LAND_MASK_RIGHT			0b00001000
#define LAND_MASK_BOTTOMLEFT	0b00000100
#define LAND_MASK_BOTTOM		0b00000010
#define LAND_MASK_BOTTOMRIGHT	0b00000001

Map::Map():m_seed(0),m_worldsize(0),m_bufferstartx(~0x0ULL),m_bufferstarty(~0x0ULL),m_bufferdim(18)
{

}

Map::~Map()
{

}

Map &Map::Instance()
{
	static Map m;
	return m;
}
	
void Map::SetSeed(const uint64_t seed)
{
	m_seed=seed;
	m_perlin.Setup(seed);
	m_bufferstartx=~0x0ULL;
	m_bufferstarty=~0x0ULL;
}

void Map::SetSize(const uint64_t size)
{
	m_worldsize=size;
}

uint64_t Map::GetSize()
{
	return m_worldsize;
}

uint64_t Map::WrapCoordinate(int64_t coordinate) const
{
	int64_t coord=coordinate;
	while(coord<0 && m_worldsize>0)
	{
		coord+=m_worldsize;
	}
	while(coord>=m_worldsize)
	{
		coord=coord%m_worldsize;
	}
	
	return coord;
}

int64_t Map::DeltaCoordinate(int64_t sourcecoord, int64_t destcoord) const
{
	int64_t d1=destcoord-sourcecoord;
	int64_t d2=destcoord-(sourcecoord-m_worldsize);
	int64_t d3=(destcoord-m_worldsize)-sourcecoord;

	if(_abs(d1)<=_abs(d2) && _abs(d1)<=_abs(d3))
	{
		return d1;
	}
	else if(_abs(d2)<=_abs(d3))
	{
		return d2;
	}

	return d3;
}

bool Map::MoveBlocked(const uint64_t sourcex, const uint64_t sourcey, const uint64_t destx, const uint64_t desty)
{
	return false;
}

uint8_t Map::GetTerrainType(const uint64_t worldx, const uint64_t worldy, const bool usecached) const
{
	const int64_t wx=WrapCoordinate(worldx);
	const int64_t wy=WrapCoordinate(worldy);
	
	if(usecached==true)
	{
		int64_t wwx=wx;
		int64_t wwy=wy;
		if(wx<m_bufferstartx)
		{
			wwx+=m_worldsize;
		}
		if(wy<m_bufferstarty)
		{
			wwy+=m_worldsize;
		}
		if(wwx>=m_bufferstartx && wwx<m_bufferstartx+m_bufferdim && wwy>=m_bufferstarty && wwy<m_bufferstarty+m_bufferdim)
		{
			const int64_t bx=wwx-m_bufferstartx;
			const int64_t by=wwy-m_bufferstarty;
			const int pos=(by*m_bufferdim)+bx;
			return m_buffer[pos].GetTerrainType();
		}
	}
	
	const double h=m_perlin.Get(static_cast<double>(wx)*4.0/static_cast<double>(m_worldsize),static_cast<double>(wy)*4.0/static_cast<double>(m_worldsize),4,10);
	if(h<0.0)
	{
		return Tile::TERRAIN_WATER;
	}
	else
	{
		return Tile::TERRAIN_LAND;
	}
}

void Map::UpdateWorldPosition(const uint64_t worldx, const uint64_t worldy)
{
	int64_t sx=WrapCoordinate(static_cast<int64_t>(worldx)-(m_bufferdim/2));
	int64_t sy=WrapCoordinate(static_cast<int64_t>(worldy)-(m_bufferdim/2));
	if(sx!=m_bufferstartx || sy!=m_bufferstarty)
	{
		m_bufferstartx=sx;
		m_bufferstarty=sy;
		for(int64_t by=0; by<m_bufferdim; by++)
		{
			for(int64_t bx=0; bx<m_bufferdim; bx++)
			{
				m_buffer[(by*m_bufferdim)+bx]=ComputeTile(bx+m_bufferstartx,by+m_bufferstarty);
			}
		}
	}
}

Tile Map::GetTile(const uint64_t worldx, const uint64_t worldy)
{
	if(worldx>=m_bufferstartx && worldx<m_bufferstartx+m_bufferdim && worldy>=m_bufferstarty && worldy<m_bufferstarty+m_bufferdim)
	{
		return m_buffer[((worldy-m_bufferstarty)*m_bufferdim)+(worldx-m_bufferstartx)];
	}
	else
	{
		return ComputeTile(worldx,worldy);
	}
}

Tile Map::ComputeTile(const uint64_t worldx, const uint64_t worldy)
{
	Tile t;
	const int64_t wx=WrapCoordinate(worldx);
	const int64_t wy=WrapCoordinate(worldy);
	const double h=m_perlin.Get(static_cast<double>(wx)*4.0/static_cast<double>(m_worldsize),static_cast<double>(wy)*4.0/static_cast<double>(m_worldsize),4,10);
	
	m_rand.Seed(m_seed ^ ((wx << 32) | wy));

	if(h<0)
	{
		t.SetTerrainType(Tile::TERRAIN_WATER);
		t.SetLocationType(Tile::LOCATION_OUTDOOR);

		if(m_rand.NextDouble()<0.4)
		{
			if(m_rand.NextDouble()<0.5)
			{
				t.SetFeature(Tile::FEATURE_WATER1);
			}
			else
			{
				t.SetFeature(Tile::FEATURE_WATER2);
			}
		}
	}
	else
	{
		t.SetTerrainType(Tile::TERRAIN_LAND);
		t.SetLocationType(Tile::LOCATION_OUTDOOR);
		
		bool placedfeature=false;
		uint64_t s=m_rand.Next();
		m_rand.Seed(s);
		// chance of town between certain heights
		if(h>0.01 && h<0.4)
		{
			if(m_rand.NextDouble()<0.001)
			{
				if(m_rand.NextDouble()<0.5)
				{
					t.SetFeature(Tile::FEATURE_TOWN1);
				}
				else
				{
					t.SetFeature(Tile::FEATURE_TOWN2);
				}
				placedfeature=true;
			}
		}
		if(placedfeature==false && h>=0.001 && h<0.5)
		{
			const double v=m_rand.NextDouble();
			if(v<0.025)
			{
				t.SetFeature(Tile::FEATURE_GRASS1);
			}
			else if(v<0.05)
			{
				t.SetFeature(Tile::FEATURE_GRASS2);
			}
			else if(v<0.075)
			{
				t.SetFeature(Tile::FEATURE_GRASS3);
			}
			else if(v<0.1)
			{
				t.SetFeature(Tile::FEATURE_GRASS4);
			}
		}
		
		//t.SetGlyphByType(Tile::GLYPH_LAND);
		//trace("land");
	}
	
	return t;
}

uint8_t Map::GetLandMask(const uint64_t worldx, const uint64_t worldy) const
{
	uint8_t landmask=0;	// bitmask for surrounding 8 positions, bit=1 if there is land 0 if there is water
	uint8_t bitshift=7;
	for(int64_t by=((int64_t)worldy)-1; by<((int64_t)worldy)+2; by++)
	{
		for(int64_t bx=((int64_t)worldx)-1; bx<((int64_t)worldx)+2; bx++)
		{
			const uint64_t wbx=WrapCoordinate(bx);
			const uint64_t wby=WrapCoordinate(by);
			if(wby!=worldy || wbx!=worldx)
			{
				if(GetTerrainType(wbx,wby,true)==Tile::TERRAIN_LAND)
				{
					landmask|=(0x1 << bitshift);
				}
				bitshift--;
			}
		}
	}
	return landmask;
}

bool Map::LandMaskSet(const uint8_t landmask, const uint8_t checkmask) const
{
	return (landmask & checkmask) == checkmask;
}

void Map::DrawTile(const uint64_t worldx, const uint64_t worldy, const int64_t screenx, const int64_t screeny)
{
	LZ4Blitter::Instance().SetSheet((uint8_t **)spriteterrain,spriteterrainWidth,spriteterrainRowHeight);
	int64_t wx=WrapCoordinate(worldx);
	int64_t wy=WrapCoordinate(worldy);
	// make world x and y inside buffer pos if possible (buffer might be near wrapping position and wrapped coords of buffer are "outside" map size
	if(wx<=m_bufferstartx)
	{
		wx+=m_worldsize;
	}
	if(wy<=m_bufferstarty)
	{
		wy+=m_worldsize;
	}
	if(wx>=m_bufferstartx && wx<m_bufferstartx+m_bufferdim && wy>=m_bufferstarty && wy<m_bufferstarty+m_bufferdim)
	{
		const int64_t mx=wx-m_bufferstartx;
		const int64_t my=wy-m_bufferstarty;
		const int pos=(my*m_bufferdim)+mx;
		bool drawfeature=true;
		
		if(m_buffer[pos].GetTerrainType()==Tile::TERRAIN_WATER)
		{
			*DRAW_COLORS=PALETTE_BLUE;
			rect(screenx,screeny,16,16);
			
			const uint8_t landmask=GetLandMask(worldx,worldy);
			const uint8_t masktlbr=landmask & (LAND_MASK_TOP | LAND_MASK_RIGHT | LAND_MASK_BOTTOM | LAND_MASK_LEFT);
			*DRAW_COLORS=PALETTE_GREEN;
			uint32_t drawflags=spriteterrainFlags;
			int16_t idxx=-1;
			int16_t idxy=11;

			// land to the left
			if(masktlbr==LAND_MASK_LEFT)
			{
				idxx=1;
			}
			// land below
			else if(masktlbr==LAND_MASK_BOTTOM)
			{
				idxx=1;
				drawflags=spriteterrainFlags|BLIT_ROTATE;
			}
			// land right
			else if(masktlbr==LAND_MASK_RIGHT)
			{
				idxx=1;
				drawflags=spriteterrainFlags|BLIT_FLIP_X;
			}
			// land above
			else if(masktlbr==LAND_MASK_TOP)
			{
				idxx=1;
				drawflags=spriteterrainFlags|BLIT_FLIP_X|BLIT_ROTATE;
			}
			// land left and above
			else if(masktlbr==(LAND_MASK_LEFT | LAND_MASK_TOP))
			{
				idxx=2;
			}
			// land right and above
			else if(masktlbr==(LAND_MASK_TOP | LAND_MASK_RIGHT))
			{
				idxx=2;
				drawflags=spriteterrainFlags|BLIT_FLIP_X;
			}
			// land below and left
			else if(masktlbr==(LAND_MASK_BOTTOM | LAND_MASK_LEFT))
			{
				idxx=2;
				drawflags=spriteterrainFlags|BLIT_ROTATE;
			}
			// land below and right
			else if(masktlbr==(LAND_MASK_RIGHT | LAND_MASK_BOTTOM))
			{
				idxx=2;
				drawflags=spriteterrainFlags|BLIT_FLIP_Y|BLIT_ROTATE;
			}
			// land below,left,and top
			else if(masktlbr==(LAND_MASK_BOTTOM | LAND_MASK_LEFT | LAND_MASK_TOP))
			{
				idxx=4;
				idxy=10;
				drawflags=spriteterrainFlags|BLIT_ROTATE;
			}
			// land left,top,and right
			else if(masktlbr==(LAND_MASK_LEFT | LAND_MASK_TOP | LAND_MASK_RIGHT))
			{
				idxx=4;
				idxy=10;
			}
			// land top,right, and bottom
			else if(masktlbr==(LAND_MASK_TOP | LAND_MASK_RIGHT | LAND_MASK_BOTTOM))
			{
				idxx=4;
				idxy=10;
				drawflags=spriteterrainFlags|BLIT_FLIP_Y|BLIT_ROTATE;
			}
			// land right,bottom,and left
			else if(masktlbr==(LAND_MASK_RIGHT | LAND_MASK_BOTTOM | LAND_MASK_LEFT))
			{
				idxx=4;
				idxy=10;
				drawflags=spriteterrainFlags|BLIT_FLIP_Y;
			}

			if(idxx>=0)
			{
				LZ4Blitter::Instance().Blit(screenx,screeny,16,16,idxx,idxy,drawflags);
			}

			if(landmask!=0)
			{
				drawfeature=false;
			}
		}
		else if(m_buffer[pos].GetTerrainType()==Tile::TERRAIN_LAND)
		{
			*DRAW_COLORS=PALETTE_GREEN;
			rect(screenx,screeny,16,16);
		}
		
		if(drawfeature==true && m_buffer[pos].GetFeature()!=Tile::FEATURE_NONE)
		{
			const struct Tile::DrawData dd=m_buffer[pos].GetDrawData();
			*DRAW_COLORS=dd.blitcolors;
			//blitSub(spriteterrain,screenx,screeny,16,16,(dd.spriteidxx*16),(dd.spriteidxy*16),spriteterrainWidth,dd.blitflags);
			LZ4Blitter::Instance().Blit(screenx,screeny,16,16,dd.spriteidxx,dd.spriteidxy,dd.blitflags);
		}
	}
	else
	{
		*DRAW_COLORS=PALETTE_WHITE;
		rect(screenx,screeny,4,4);
	}
}

void Map::UnwrapCoordinates(const int64_t coord1, const int64_t coord2, int64_t &unwrappedcoord1, int64_t &unwrappedcoord2) const
{
	if(coord1==coord2)
	{
		unwrappedcoord1=coord1;
		unwrappedcoord2=coord2;
	}
	else if(coord1<coord2)
	{
		const int64_t delta1=coord2-coord1;
		const int64_t delta2=(coord1+m_worldsize)-coord2;
		if(delta1<=delta2)
		{
			unwrappedcoord1=coord1;
			unwrappedcoord2=coord2;
		}
		else
		{
			unwrappedcoord1=coord1;
			unwrappedcoord2=coord2-m_worldsize;
		}
	}
	else
	{
		const int64_t delta1=coord1-coord2;
		const int64_t delta2=(coord2+m_worldsize)-coord1;
		if(delta1<=delta2)
		{
			unwrappedcoord1=coord1;
			unwrappedcoord2=coord2;
		}
		else
		{
			unwrappedcoord1=coord1-m_worldsize;
			unwrappedcoord2=coord2;
		}
	}
}

float Map::ComputeAngle(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const
{
	int64_t sx=sourcex;
	int64_t sy=sourcey;
	int64_t dx=destx;
	int64_t dy=desty;
	UnwrapCoordinates(sx,dx,sx,dx);
	UnwrapCoordinates(sy,dy,sy,dy);

	return _atan2(dy-sy,dx-sx);

}

float Map::ComputeDistance(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const
{
	return _sqrt(ComputeDistanceSq(sourcex,sourcey,destx,desty));
}

float Map::ComputeDistanceSq(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const
{
	int64_t sx=sourcex;
	int64_t sy=sourcey;
	int64_t dx=destx;
	int64_t dy=desty;
	UnwrapCoordinates(sx,dx,sx,dx);
	UnwrapCoordinates(sy,dy,sy,dy);

	return ((sx-dx)*(sx-dx))+((sy-dy)*(sy-dy));
}

int64_t Map::ComputeDistanceCoord(const int64_t source, const int64_t dest) const
{
	int64_t s=source;
	int64_t d=dest;
	UnwrapCoordinates(s,d,s,d);

	return (s<d ? d-s : s-d);
}

int64_t Map::ComputeDistanceManhattan(const int64_t sourcex, const int64_t sourcey, const int64_t destx, const int64_t desty) const
{
	int64_t sx=sourcex;
	int64_t sy=sourcey;
	int64_t dx=destx;
	int64_t dy=desty;
	UnwrapCoordinates(sx,dx,sx,dx);
	UnwrapCoordinates(sy,dy,sy,dy);

	return (sx<dx ? dx-sx : sx-dx)+(sy<dy ? dy-sy : sy-dy);
}
