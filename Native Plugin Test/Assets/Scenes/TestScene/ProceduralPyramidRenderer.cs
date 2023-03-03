using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Security;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Rendering;

public class ProceduralPyramidRenderer : MonoBehaviour
{
    [Tooltip("A mesh to extrude the pyramids from")]
    [SerializeField] private Mesh sourceMesh = default;
    [Tooltip("The compute shader that creates the pyramid geometry")]
    [SerializeField] private ComputeShader computeShader = default;
    [Tooltip("The material to render the pyramid mesh")]
    [SerializeField] private Material material = default;
    [Tooltip("Pyramid height")]
    [SerializeField] private float pyramidHeight = 1.0f;
    [Tooltip("Animation frequency")]
    [SerializeField] private float animationFrequency = 1.0f;

    //Here we have our struct that will fill up our SourceVertex buffer. It should be identical to the 
    //SourceVertex defined in hlsl, but using vectors instead of floats. We also ensure that the data is laid 
    //out sequentially with our header

    [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
    struct SourceVertex 
    {
        public Vector3 positionOS;
        public Vector2 uv;

        public SourceVertex(Vector3 positionOS, Vector2 uv) 
        {
            this.positionOS = positionOS;
            this.uv = uv;
        }
    }

    //A state variable to keep track of whether the compute buffers have been successfully initialised
    private bool initialised;
    //A compute buffer to hold vertex data of the source mesh - this and the following 2 variables are really just pointers to their gpu equivalents.
    private ComputeBuffer _SourceVertices;
    //A compute buffer to hold index data of the source mesh.
    private ComputeBuffer _SourceTriangles;
    //A compute buffer to hold vertex data of the generated mesh.
    private ComputeBuffer _DrawTriangles;
    //indirect arguments buffer. Contains 4 values.
    private ComputeBuffer argsBuffer;
    //The id of the kernel in the compute shader.
    private int kernelId;
    //the size of the thread group
    private int dispatchSize;
    //The bounding volume within which the mesh may be drawn
    private Bounds bounds;
    

    //The size of one entry into the various compute buffers
    private const int _SourceVertStride = sizeof(float) * (3 + 2);
    private const int _SourceTriStride = sizeof(int);
    private const int _DrawTriangleStride = sizeof(float) * (3 + (3 + 2) * 3);
    private const int _IndirectArgsStride = sizeof(int) * 4;

    private void OnEnable()
    {
        if (initialised) 
        {
            OnDisable(); //clean things up
        }
        initialised = true;
        //grab the data from the source mesh
        Vector3[] positions = sourceMesh.vertices;
        Vector2[] uvs = sourceMesh.uv;
        int[] tris = sourceMesh.triangles;

        bounds = sourceMesh.bounds;
        bounds.Expand(pyramidHeight);

        //Create the data to upload to the source vertex buffer
        SourceVertex[] sourceVertices = new SourceVertex[positions.Length];
        for (int i = 0; i < sourceVertices.Length; i++) 
        {
            sourceVertices[i] = new SourceVertex(positions[i], uvs[i]);
        }
        //number of triangles is a third of the triangle array size
        int numTriangles = tris.Length / 3;

        //Create the compute buffers
        _SourceVertices = new ComputeBuffer(sourceVertices.Length, _SourceVertStride, ComputeBufferType.Structured, ComputeBufferMode.Immutable);
        _SourceVertices.SetData(sourceVertices);
        _SourceTriangles = new ComputeBuffer(tris.Length, _SourceTriStride, ComputeBufferType.Structured, ComputeBufferMode.Immutable);
        _SourceTriangles.SetData(tris);
        //We split each triangle into three new triangles, which is why we mulitply numTriangles by 3.
        _DrawTriangles = new ComputeBuffer(numTriangles * 3, _DrawTriangleStride, ComputeBufferType.Append);
        //Setting the ComputeBufferType to append gives the buffer a counter value, as it now behaves like a stack
        _DrawTriangles.SetCounterValue(0);
        argsBuffer = new ComputeBuffer(1, _IndirectArgsStride, ComputeBufferType.IndirectArguments);
        argsBuffer.SetData(new int[] { 0, 1, 0, 0 });

        //We now need to bind these Compute Buffers to their equivalents on the GPU.

        kernelId = computeShader.FindKernel("CSMain");
        computeShader.SetBuffer(kernelId, "_SourceVertices", _SourceVertices);
        computeShader.SetBuffer(kernelId, "_SourceTriangles", _SourceTriangles);
        computeShader.SetBuffer(kernelId, "_DrawTriangles", _DrawTriangles);
        computeShader.SetInt("_NumSourceTriangles", numTriangles);
        material.SetBuffer("_DrawTriangles", _DrawTriangles);

        //Now we calculate the number of thread groups to dispatch. We get the size of a 
        //single thread group from the kernel, then divide the number of triangles by that 
        //size and round up. Since we only have more than one thread in the x direction
        //we only worry about the x size
        computeShader.GetKernelThreadGroupSizes(kernelId, out uint x, out _, out _);
        dispatchSize = Mathf.CeilToInt((float)numTriangles / x);
    }
    
    private void OnDisable()
    {
        if (initialised) 
        {
            _SourceVertices.Release();
            _SourceTriangles.Release();
            _DrawTriangles.Release();
            argsBuffer.Release();
        }
        initialised = false;
    }

    private void LateUpdate()
    {
        _DrawTriangles.SetCounterValue(0);
        computeShader.SetMatrix("_LocalToWorld", transform.localToWorldMatrix);
        computeShader.SetFloat("_PyramidHeight", pyramidHeight * Mathf.Sin(animationFrequency * Time.timeSinceLevelLoad)); //animate the triangles
        computeShader.Dispatch(kernelId, dispatchSize, 1, 1);
        ComputeBuffer.CopyCount(_DrawTriangles, argsBuffer, 0);

        //DrawProceduralIndirect queues up a draw call for our generated mesh. It will receive a shodw casting pass, like normal
        //Material
        //Culling bounds - area in WS the mesh could appear in
        //mesh topology - triangles
        //computerBuffer buffer with args - small compute buffer that tells the system the number of verts in the mesh
        Graphics.DrawProceduralIndirect(material, bounds, MeshTopology.Triangles, argsBuffer, 0, null, null, ShadowCastingMode.On, true, gameObject.layer);
    }

}
