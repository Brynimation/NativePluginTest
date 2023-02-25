using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using UnityEngine.Rendering;
using System;
using Unity.VisualScripting;

public struct ConstantBufferData 
{
    float xOffset;
    float yOffset;
    float zOffset;

    float[] M;
    float[] V;
    float[] P;

    public ConstantBufferData(float xOffset, float yOffset, float zOffset,
                              float[] M, float[] V, float[] P)
    {
        this.xOffset = xOffset;
        this.yOffset = yOffset;
        this.zOffset = zOffset;
        this.M = M;
        this.V = V;
        this.P = P;
    }
}

public class NativePluginCallerAgain : MonoBehaviour
{
    [DllImport("NativePlugin")]
    private static extern void SetTimeFromUnity(float time);
    [DllImport("NativePlugin")]
    private static extern void SetMeshBuffersFromUnity(
        IntPtr vertexBuffer,int vertexCount,IntPtr sourceVerts,
        IntPtr sourceNorms, IntPtr sourceUVs
    );

    [DllImport("NativePlugin")]
    private static extern void SetShaderUniformsFromUnity(ConstantBufferData cbd);

    [DllImport("NativePlugin")]
    private static extern IntPtr GetRenderEventFunc();
    private void SendMVPMatricesToPlugin() 
    {
        float[] M = MatrixToArray(transform.localToWorldMatrix);
        float[] V = MatrixToArray(Camera.main.worldToCameraMatrix);
        float[] P = MatrixToArray(Camera.main.projectionMatrix);

        /*GCHandle gcM = GCHandle.Alloc(M, GCHandleType.Pinned);
        GCHandle gcV = GCHandle.Alloc(V, GCHandleType.Pinned);
        GCHandle gcP = GCHandle.Alloc(P, GCHandleType.Pinned);*/
        ConstantBufferData cbd = new ConstantBufferData(0, 0, 0, M, V, P);
        SetShaderUniformsFromUnity(cbd);
    }
    private static float[] MatrixToArray(Matrix4x4 i) 
    {
        float[] o = new float[16];
        for (int row = 0; row < 4; row++) 
        {
            for (int col = 0; col < 4; col++) 
            {
                o[row * 4 + col] = i[row,col];
            }
        }
        return o;
    }
    private void SendMeshDataToPlugin() 
    {
        MeshFilter mf = GetComponent<MeshFilter>();
        Mesh mesh = mf.mesh;
        

        mesh.MarkDynamic(); //makes vertex buffer cpu writable (not readable)
        Color[] colours = mesh.colors;
        Vector3[] verts = mesh.vertices;
        Vector2[] uvs = mesh.uv;
        Vector3[] norms = mesh.normals;

        //GCHandle allows us to access managed objects (in our c# code) from
        //unmanaged memory (our c++ code).
        GCHandle gcVerts = GCHandle.Alloc(verts, GCHandleType.Pinned);
        GCHandle gcUVs = GCHandle.Alloc(uvs, GCHandleType.Pinned);
        GCHandle gcNorms = GCHandle.Alloc(norms, GCHandleType.Pinned);

        SetMeshBuffersFromUnity( //calling external script from plugin
            mesh.GetNativeVertexBufferPtr(0), //returns an IntPtr to the graphic API vertex buffer (of type ID3D11Buffer*).
            mesh.vertexCount,
            gcVerts.AddrOfPinnedObject(),
            gcNorms.AddrOfPinnedObject(),
            gcUVs.AddrOfPinnedObject()
       );
            
        gcVerts.Free();
        gcUVs.Free();
        gcNorms.Free();
    }

    
    void Start()
    {
        SendMeshDataToPlugin();
    }

    
    void Update()
    {
        GL.IssuePluginEvent(GetRenderEventFunc(), 0);
        SetTimeFromUnity(Time.timeSinceLevelLoad);
    }
}
