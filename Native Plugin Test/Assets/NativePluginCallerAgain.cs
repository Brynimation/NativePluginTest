using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using UnityEngine.Rendering;
using System;
using Unity.VisualScripting;

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
    private static extern void SetShaderUniformsFromUnity(Matrix4x4 M, Matrix4x4 V, Matrix4x4 P);

    [DllImport("NativePlugin")]
    private static extern IntPtr GetRenderEventFunc();
    private void SendMVPMatricesToPlugin() 
    {
        Matrix4x4 M = transform.localToWorldMatrix;
        Matrix4x4 V = Camera.main.worldToCameraMatrix;
        Matrix4x4 P = Camera.main.projectionMatrix;

        GCHandle gcM = GCHandle.Alloc(M, GCHandleType.Pinned);
        GCHandle gcV = GCHandle.Alloc(V, GCHandleType.Pinned);
        GCHandle gcP = GCHandle.Alloc(P, GCHandleType.Pinned);
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
