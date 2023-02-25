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
};

Interpolators main(Attributes i)
{
	Interpolators o;
	float4x4 VP = mul(P, V);
	float4x4 MVP = mul(VP, M);

	o.positionHCS = mul(MVP, i.position);
	o.colour = i.colour;
	return o;
}