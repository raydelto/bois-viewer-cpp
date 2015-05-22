#include "ShipCreatedDto.h"

ShipCreatedDto::ShipCreatedDto()
{
}

ShipCreatedDto::ShipCreatedDto(bytestream* stream)
{
	deserialize(stream);
}

ShipCreatedDto::~ShipCreatedDto()
{
}

void ShipCreatedDto::serialize(bytestream* stream)
{
}

void ShipCreatedDto::deserialize(bytestream* stream)
{
	MsgPack::Deserializer deserializer(stream->rdbuf());
	unique_ptr<MsgPack::Element> element;
	deserializer >> element;

	id = uint16FromMsgPackMap(element, L"id");
	x = floatFromMsgPackMap(element, L"x");
	y = floatFromMsgPackMap(element, L"y");
	rot = floatFromMsgPackMap(element, L"rot");
}
