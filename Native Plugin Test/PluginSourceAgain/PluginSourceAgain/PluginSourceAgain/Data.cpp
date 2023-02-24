struct ConstantBufferData
{
    float xOffset;
    float yOffset;
    float zOffset;
    float M[16];
    float V[16];
    float P[16];
};
struct MeshVertex
{
    float pos[3];
    float colour[4];
};