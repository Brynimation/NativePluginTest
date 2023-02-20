using System.Runtime.InteropServices;
using System;
using UnityEngine;
using UnityEngine.Rendering;

public class NativePluginCaller : MonoBehaviour
{
    CommandBuffer commandBuffer;
    #if UNITY_IPHONE && !UNITY_EDITOR
    [DllImport ("__Internal")]
    #else
    [DllImport("Dll1")] //name of the dll project. This imports the Dll of that name, and allows us to access its code from C#
    #endif

    /*An IntPtr is an integer the same size as a pointer. You can use IntPtr
     to store a pointer value in a non-pointer type.
     GetRenderEventFunction() is the function defined in our c++ code that returns our callback function; the function that we want unity to call
     during the render loop. The render loop is just the term used to describe
    all the rendering operations that occur in a single frame.

    We use the "extern" modifier to declare a method that is implemented 
    externally.
     */
    private static extern IntPtr getEventFunction();

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void DebugDelegate(string str); //Delegates are functions that can be passed as parameters. Any method that takes in a string and returns void can be instantiated as a DebugDelegate.

    [DllImport("Dll1")]
    public static extern void SetDebugFunction(IntPtr fp);
    static void CallBackFunction(string str) 
    {
        Debug.Log(str);
    }
    private void Start()
    {
        DebugDelegate callback = new DebugDelegate(CallBackFunction);
        /*Convert the callback delegate into a function pointer that can be 
         used in unmanaged code (ie, our c++ code).*/
        IntPtr intPtrDelegate = Marshal.GetFunctionPointerForDelegate(callback);
        SetDebugFunction(intPtrDelegate);   

        commandBuffer = new CommandBuffer();
        commandBuffer.name = "Dll1";
        Camera.main.AddCommandBuffer(CameraEvent.AfterGBuffer, commandBuffer);
    }

    // Queue a specific callback to be called on the render thread
    private void Update()
    {
       commandBuffer.IssuePluginEvent(getEventFunction(), 0);
    }
    
}


