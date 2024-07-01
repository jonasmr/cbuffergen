#pragma once
#include "helpers/descriptorhandle.h"


struct funk
{
	float shininessxx;
	float3 hest[2][ASDF][FOO];
	float lala;
	float bar;
};


struct appearanceStruct
{
	float shininess;
	uint3 fisk;
	float3 hest[2];
	float2x2 matrix_type;
	float2x2 matrix_array[2][3][3][4][4];
	float lala;

	float4 ambient;
	float4 diffuse;
	float4 specular;
	float opacity[ASDF][BADF];
	float specLevel;
	int debrisColorStyle;


	PalDescriptorHandle baseTexture;
	PalDescriptorHandle specTexture;
	PalDescriptorHandle normalTexture;
	
};

struct primitiveContainerStruct
{
	float3 aabbMin;
	float3 aabbMax;
	float3 boundingSphereCenter;
	float  boundingSphereRadius;
	uint primitiveOffset;
	uint primitiveCount;
};


struct primitiveStruct
{
	uint4 IndexBufferVA;

	uint appearanceId;

	PalDescriptorHandle posDescriptor;
	PalDescriptorHandle normDescriptor;
	PalDescriptorHandle tc0Descriptor;
};

struct drawStruct
{
	 uint objectId;
	 uint primitiveId;
};
