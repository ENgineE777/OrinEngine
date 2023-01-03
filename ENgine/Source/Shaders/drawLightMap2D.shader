
struct VS_INPUT
{
	float2 position : POSITION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D colorMap : register(t0);
SamplerState colorMapLinear : register(s0);

cbuffer ps_params : register( b0 )
{
	float4 ambient;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = float4(input.position.x, input.position.y, 0.5f, 1.0);
	output.texCoord = float2(input.position.x, input.position.y);

	return output;
}

float4 PS_AMBIENT( PS_INPUT input) : SV_Target
{
	return ambient;
}

float4 PS( PS_INPUT input) : SV_Target
{
	float2 texCoord = input.texCoord;
	texCoord = texCoord * 0.5f + 0.5f;
	texCoord.y = 1 - texCoord.y;

	return colorMap.Sample(colorMapLinear, texCoord);
}