struct Interpolators
{
	float4 positionHCS : SV_POSITION;
	float4 colour : COLOR;
};
float4 frag(Interpolators i) : SV_TARGET
{
	return i.colour;
}