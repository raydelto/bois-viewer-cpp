#pragma once
#include <stormancer.h>

class ShipCreatedDto : Stormancer::ISerializable
{
public:
	ShipCreatedDto();

	ShipCreatedDto(bytestream* stream);

	~ShipCreatedDto();

public:
	void serialize(bytestream* stream);

	void deserialize(bytestream* stream);

public:
	Stormancer::uint16 id;
	float x;
	float y;
	float rot;
};
