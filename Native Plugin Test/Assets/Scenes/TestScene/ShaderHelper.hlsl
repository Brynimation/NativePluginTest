#include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
#include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"


//returns the view direction (the direction vector from the position to the camera)
float3 GetViewDirectionFromPosition(float3 positionWS)
{
	return normalize(GetCameraPositionWS() - positionWS);
}

#ifdef SHADOW_CASTER_PASS
float3 _LightDirection;
#endif 

//HCS = Homogenous Clip Space. The vertex shader transforms positions from local to clip space.
float4 CalculatePositionHCSWithShadowCasterLogic(float3 positionWS, float3 normalWS)
{
	float4 positionHCS;
#ifdef SHADOW_CASTER_PASS
//From URP's shadow ShadowCasterPass.hlsl. If this is the shadow caster pass, we need to adjust the HCS POSITION
//to account for shadow bias and offset. This helps to reduce shadow artifacts.
	positionHCS = TransformWorldToHClip(ApplyShadowBias(positionWS, normalWS, _LightDirection)
	
	
	);
#if UNITY_REVERSED_Z
	positionHCS.z = min(positionHCS.z, positionHCS.w * UNITY_NEAR_CLIP_VALUE);
#else
	positionHCS.z = max(positionCS.z, positionCS.w * UNITY_NEAR_CLIP_VALUE);
#endif 
#else
	//This built in function transforms from world space to homogenous clip space.
	positionHCS = TransformWorldToHClip(positionWS);
#endif 
	return positionHCS;
}

//Calculates the shadow texture coordinate for lighting calculations
float4 CalculateShadowCoord(float3 positionWS, float4 positionHCS)
{
#if SHADOWS_SCREEN
	return ComputeScreenPos(positionHCS);
#else
	return TransformWorldToShadowCoord(positionWS);
#endif
}

