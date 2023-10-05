
cbuffer vs_params : register( b0 )
{
    float4 desc[2];
    matrix trans;
    matrix view_proj;
};

cbuffer ps_params : register( b0 )
{
	float4 color;
};

struct VS_INPUT
{
    float2 position : POSITION;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D diffuseMap : register(t0);
SamplerState samLinear : register(s0);

Texture2D maskMap : register(t1);
SamplerState maskLinear : register(s1);

PS_INPUT VS( VS_INPUT input )
{
	float4 posTemp = float4(desc[0].x + desc[0].z * input.position.x,
							desc[0].y - desc[0].w * input.position.y, 0, 1.0f);
	
	PS_INPUT output = (PS_INPUT)0;

    float4 pos = mul(float4(posTemp.x, posTemp.y, 0.0f, 1.0f), trans);
	output.pos = mul(pos, view_proj);

    output.texCoord = float2(desc[1].x + desc[1].z * input.position.x, desc[1].y + desc[1].w * input.position.y);
	
	return output;
}

float4 PS( PS_INPUT input) : SV_Target
{
    return diffuseMap.Sample(samLinear, input.texCoord)* color;
}

float4 PS_DISCARD(PS_INPUT input) : SV_Target
{
    float4 clr = diffuseMap.Sample(samLinear, input.texCoord) * color;
    if (clr.a < 0.05f)
    {
       discard;
    }

    return clr;
}

float4 PS_LIGHT(PS_INPUT input) : SV_Target
{
    float2 dir = input.texCoord * 2.0f - 1.0f;
    float intense = (1 - sqrt(dir.x * dir.x + dir.y * dir.y));

    if (intense < 0.01f)
    {
       discard;
    }

    return float4(1.0f, 1.0f, 1.0f, intense) * color;
}

struct PS_MASKED_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float2 texCoord2 : TEXCOORD1;
};

PS_MASKED_INPUT VS_MASKED(VS_INPUT input)
{
    float4 posTemp = float4(desc[0].x + desc[0].z * input.position.x,
							desc[0].y - desc[0].w * input.position.y, 0, 1.0f);
	
    PS_MASKED_INPUT output = (PS_MASKED_INPUT) 0;

    float4 pos = mul(float4(posTemp.x, posTemp.y, 0.0f, 1.0f), trans);   
    output.pos = mul(pos, view_proj);

    output.texCoord = float2(desc[1].x + desc[1].z * input.position.x, desc[1].y + desc[1].w * input.position.y);
    output.texCoord2 = float2(output.pos.x / output.pos.w, output.pos.y / output.pos.w);
    
    return output;
}

float4 PS_MASKED(PS_MASKED_INPUT input) : SV_Target
{
    float2 screenCoord = 0.5f * float2(input.texCoord2.x, -input.texCoord2.y) + 0.5f;
    
    float4 clr = maskMap.Sample(maskLinear, screenCoord.xy);
    
    if (clr.r < 0.05f)
    {
        discard;
    }
    
    return diffuseMap.Sample(samLinear, input.texCoord) * color;
}

struct VS_POLYGON_INPUT
{
    float2 position : POSITION;
    float2 texCoord : TEXCOORD;
};

PS_INPUT VS_POLYGON( VS_POLYGON_INPUT input )
{	
	PS_INPUT output = (PS_INPUT)0;

    float4 pos = mul(float4(input.position.x, input.position.y, 0.0f, 1.0f), trans);
	output.pos = mul(pos, view_proj);
	
    output.texCoord = input.texCoord;

	return output;
}