cbuffer mycBuffer
{
	float xOffset;
	float yOffset;
	float zOffset;
	float4x4 M;
	float4x4 V;
	float4x4 P;
}

struct Attributes
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float4 colour : COLOR;
};
struct Interpolators
{
	float4 positionHCS : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 colour : COLOR;
}

float4 main(Attributes i : POSITION ) : SV_POSITION
{
	Interpolators o;
	float4x4 VP = mul(P, V);
	float4x4 MVP = mul(VP, M);

	o.positionHCS = mul(MVP, o.posiion);
	o.uv = i.uv;
	o.colour = i.colour;
	
}