#pragma once

class CNetworkedQuantizedFloat
{
public:
	std::float_t m_Value;
	uint16 m_nEncoder;
	bool m_bUnflattened;
	MEM_PAD(0x1);
};

class CNetworkVelocityVector
{
public:
	Vector m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkViewOffsetVector
{
public:
	Vector m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkOriginQuantizedVector
{
public:
	Vector m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkOriginCellCoordQuantizedVector
{
public:
	Vector m_vecValue;					// 0x0000
	uint16 m_cellX;						// 0x0010
	uint16 m_cellY;						// 0x0012
	uint16 m_cellZ;						// 0x0014
	uint16 m_nOutsideWorld;				// 0x0016
	CNetworkedQuantizedFloat m_vecX;	// 0x0018
	CNetworkedQuantizedFloat m_vecY;	// 0x0020
	CNetworkedQuantizedFloat m_vecZ;	// 0x0028
};