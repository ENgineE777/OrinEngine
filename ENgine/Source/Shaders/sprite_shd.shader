
#define M_PI   3.1415926535897932384626433832795

cbuffer vs_shd_params : register(b0)
{
    float4 desc[2];
    float4 rot;
    matrix trans;
    matrix view_proj;
};

cbuffer ps_shd_params : register(b0)
{
    float4 ps_desc;
    float4 color;
};

struct VS_SHD_INPUT
{
    float2 position : POSITION;
};

struct PS_SHD_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float2 texCoord2 : TEXCOORD1;
};

PS_SHD_INPUT VS_SHD(VS_SHD_INPUT input)
{
    float4 posTemp = float4(desc[0].x + desc[0].z * input.position.x, desc[0].y - desc[0].w * input.position.y, 0, 1.0f);

    PS_SHD_INPUT output = (PS_SHD_INPUT)0;

    float4 pos = mul(float4(posTemp.x, posTemp.y, 0.0f, 1.0f), trans);
    output.pos = mul(pos, view_proj);

    output.texCoord = float2(desc[1].x + desc[1].z * input.position.x, desc[1].y + desc[1].w * input.position.y);
    
    float2 rotPos = float2(input.position.x * 2.0f - 1.0f, input.position.y * 2.0f - 1.0f);
    rotPos.x *= rot.x;
    rotPos.y *= rot.y;

    output.texCoord2 = float2(rotPos.x * cos(rot.z) - rotPos.y * sin(rot.z), rotPos.x * sin(rot.z) + rotPos.y * cos(rot.z));

    return output;
}

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

Texture2D shadowMap : register(t1);
SamplerState shadowLinear : register(s1);

float shadowMapSample(float2 coord, float r)
{
    return step(r, shadowMap.SampleLevel(shadowLinear, coord, 0).r);
}

float4 PS_SHD(PS_SHD_INPUT input) : SV_Target
{
    float cast_shadow = ps_desc.x;
    float ligthRadius = ps_desc.y;
    float2 dir = float2(input.texCoord2.x, input.texCoord2.y);
    //dir.y = -dir.y;

    float theta = atan2(dir.y, dir.x);
    float r = 0.5f * min(length(dir), ligthRadius) / ligthRadius;
    float coord = (theta + M_PI) / (2.0f * M_PI);

    float2 tc = float2(coord, (cast_shadow + 0.5f) / 32.0f);

    //the center tex coord, which gives us hard shadows
    float center = shadowMapSample(tc, r);

    //we multiply the blur amount by our distance from center
    //this leads to more blurriness as the shadow "fades away"
    float blur = (1.0f / 360.0f) * smoothstep(0.0f, 1.0f, r * 2.0f);

    //now we use a simple gaussian blur
    float shadow = 0.0;

    shadow += shadowMapSample(float2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
    shadow += shadowMapSample(float2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
    shadow += shadowMapSample(float2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
    shadow += shadowMapSample(float2(tc.x - 1.0 * blur, tc.y), r) * 0.15;

    shadow += center * 0.16;

    shadow += shadowMapSample(float2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
    shadow += shadowMapSample(float2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
    shadow += shadowMapSample(float2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
    shadow += shadowMapSample(float2(tc.x + 4.0 * blur, tc.y), r) * 0.05;

    return diffuseMap.Sample(samLinear, input.texCoord)* color * shadow;
}