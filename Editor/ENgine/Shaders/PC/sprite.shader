
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

PS_INPUT VS( VS_INPUT input )
{
	float4 posTemp = float4(desc[0].x + desc[0].z * input.position.x,
							desc[0].y - desc[0].w * input.position.y, 0, 1.0f);
	
	/*posTemp = mul(posTemp, trans);

	posTemp.x = -1.0f + posTemp.x/desc[2].x * 2.0f;
	posTemp.y = 1.0f - posTemp.y/desc[2].y * 2.0f;*/
	
	PS_INPUT output = (PS_INPUT)0;

    float4 pos = mul(float4(posTemp.x, posTemp.y, 0.0f, 1.0f), trans);
	output.pos = mul(pos, view_proj);

    output.texCoord = float2(desc[1].x + desc[1].z * input.position.x, desc[1].y + desc[1].w * input.position.y);
	
	return output;
}

float4 PS( PS_INPUT input) : SV_Target
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

struct PS_POLYGON_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PS_POLYGON_INPUT VS_POLYGON( VS_INPUT input )
{	
	PS_POLYGON_INPUT output = (PS_POLYGON_INPUT)0;

    float4 pos = mul(float4(input.position.x, input.position.y, 0.0f, 1.0f), trans);
	output.pos = mul(pos, view_proj);
	
	return output;
}

float4 PS_POLYGON( PS_INPUT input) : SV_Target
{
    float4 clr = color;
    if (clr.a < 0.05f)
    {
       discard;
    }

    return clr;
}