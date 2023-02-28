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
    [SerializeField] float xOffset;
    [SerializeField] float yOffset;
    [SerializeField] float zOffset;
    const string dll = "PluginSourceAgain";
   /* [DllImport(dll)]

    private static extern void SetTimeFromUnity(float time);
    [DllImport(dll)]
    private static extern IntPtr Hello();
    [DllImport(dll)]
    private static extern void SetMeshBuffersFromUnity(
        IntPtr vertexBuffer,int vertexCount,IntPtr sourceVerts,
        IntPtr sourceColours
    );

    [DllImport(dll)]
    private static extern void SetShaderUniformsFromUnity(ConstantBufferData cbd);*/

    [DllImport(dll)]
    private static extern IntPtr GetRenderEventFunc();
   /* private void SendMVPMatricesToPlugin() 
    {
        Debug.Log("sending uniforms");
        float[] M = MatrixToArray(transform.localToWorldMatrix);
        float[] V = MatrixToArray(Camera.main.worldToCameraMatrix);
        float[] P = MatrixToArray(Camera.main.projectionMatrix);

        GCHandle gcM = GCHandle.Alloc(M, GCHandleType.Pinned);
        GCHandle gcV = GCHandle.Alloc(V, GCHandleType.Pinned);
        GCHandle gcP = GCHandle.Alloc(P, GCHandleType.Pinned);
        ConstantBufferData cbd = new ConstantBufferData(xOffset, yOffset, zOffset, M, V, P);
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

        //GCHandle allows us to access managed objects (in our c# code) from
        //unmanaged memory (our c++ code).
        GCHandle gcVerts = GCHandle.Alloc(verts, GCHandleType.Pinned);
        GCHandle gcColours = GCHandle.Alloc(colours, GCHandleType.Pinned);

        SetMeshBuffersFromUnity( //calling external script from plugin
            mesh.GetNativeVertexBufferPtr(0), //returns an IntPtr to the graphic API vertex buffer (of type ID3D11Buffer*).
            mesh.vertexCount,
            gcVerts.AddrOfPinnedObject(),
            gcColours.AddrOfPinnedObject()
       );
            
        gcVerts.Free();
        gcColours.Free();
    }
*/
    
    void Start()
    {
        //SendMeshDataToPlugin();
        StartCoroutine(PassData());
    }

    IEnumerator PassData() 
    {
        while (true) 
        {
            yield return new WaitForEndOfFrame();
            //SendMVPMatricesToPlugin();
            //SetTimeFromUnity(Time.timeSinceLevelLoad);
            GL.IssuePluginEvent(GetRenderEventFunc(), 0);
        }
    }
    
    void Update()
    {
        /*Debug.Log(Marshal.PtrToStringAnsi(Hello()));
        SendMVPMatricesToPlugin();
        GL.IssuePluginEvent(GetRenderEventFunc(), 0);
        SetTimeFromUnity(Time.timeSinceLevelLoad);*/
    }
}
