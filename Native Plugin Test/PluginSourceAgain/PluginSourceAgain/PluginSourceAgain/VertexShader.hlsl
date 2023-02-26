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
	float3 posOffset = float3(xOffset, yOffset, zOffset);
	float4x4 VP = mul(P, V);
	float4x4 MVP = mul(VP, M);
	float3 posWorldSpace = mul(M, i.position);
	posWorldSpace += posOffset;

	o.positionHCS = mul(VP, posWorldSpace);
	o.colour = i.colour;
	return o;
}