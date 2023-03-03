Shader "Custom/MainShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" "RenderPipeline" = "UniversalPipeline" }
        LOD 100

        Pass
        {
            Name "ForwardLit"
            Tags {"LightMode" = "UniversalForward"}

            HLSLPROGRAM
            //Target 5.0 means this shader supports compute buffers
            #pragma prefer_hlslcc gles
            #pragma exclude_renderers d3d11_9x
            #pragma target 5.0
            
            //Lighting and shadow keywords
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS_CASCADE
            #pragma multi_compile _ _ADDITIONAL_LIGHTS
            #pragma multi_compile _ _ADDITIONAL_LIGHT_SHADOWS
            #pragma multi_compile _ _SHADOWS_SOFT

            //register the vertex and fragment functions s
            #pragma vertex vert
            #pragma fragment frag
            
            #include "Logic.hlsl"
            ENDHLSL
        }
         Pass
        {
            Name "ForwardLit"
            Tags {"LightMode" = "ShadowCaster"}

            HLSLPROGRAM
            //Target 5.0 means this shader supports compute buffers
            #pragma prefer_hlslcc gles
            #pragma exclude_renderers d3d11_9x
            #pragma target 5.0
            
            //Lighting and shadow keywords
            #pragma multi_compile_shadowcaster

            //register the vertex and fragment functions 
            #pragma vertex vert
            #pragma fragment frag
            
            #include "Logic.hlsl"
            ENDHLSL
        }
    }
}
