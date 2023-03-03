//returns the normal of a plane containing the triangle defined by the three vertices.
float3 GetNormalFromTriangle(float3 a, float3 b, float3 c)
{
    return cross(b-a, c-a);
}

//returns the centre of a triangle defined by three vertices.
float3 GetTriangleCentre(float3 a, float3 b, float3 c)
{
    return (a + b + c)/3.0;
}

float2 GetTriangleCentre(float2 a, float2 b, float2 c)
{
    return (a + b + c)/3.0;
}