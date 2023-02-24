cbuffer mycBuffer :register(b0)
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
	float4 colour : COLOR;

};
struct Interpolators
{
	float4 positionHCS : SV_POSITION;
	float4 colour : COLOR;
}

float4 vert(Attributes i : POSITION ) : SV_POSITION
{
	Interpolators o;
	float4x4 VP = mul(P, V);
	float4x4 MVP = mul(VP, M);

	o.positionHCS = mul(MVP, o.posiion);
	o.colour = i.colour;
	return o;
}