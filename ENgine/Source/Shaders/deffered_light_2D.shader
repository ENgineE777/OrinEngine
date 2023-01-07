#define M_PI   3.1415926535897932384626433832795
#define M_PI_2 1.5707963267948966192313216916398
#define M_2PI  6.283185307179586476925286766559
#define M_3PI  9.4247779607693797153879301498385

static const float3 dielectric      = float3(0.4f.xxx); 
static const float3 v_norm          = float3(0.0, 0.0, 1.0);

cbuffer ps_params : register( b0 )
{
	float4 color;
	float4 u_lights[3 + 4 * 16];
};

struct VS_INPUT
{
    float2 position : POSITION;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

Texture2D diffuseMap : register(t0);
SamplerState diffuseLinear : register(s0);

Texture2D materialMap : register(t1);
SamplerState materialLinear : register(s1);

Texture2D normalsMap : register(t2);
SamplerState normalsLinear : register(s2);

Texture2D selfilumMap : register(t3);
SamplerState selfilumLinear : register(s3);

struct PS_GBUFFER_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 material : SV_TARGET1;
    float4 normals : SV_TARGET2;
	float4 selfilum : SV_TARGET3;
};

float3 get_radiance(float c)
{
	// UNPACK COLOR BITS
	float3 col;
	col.b = floor(c * 0.0000152587890625);
	float blue_bits = c - col.b * 65536.0;
	col.g = floor(blue_bits * 0.00390625);
	col.r = floor(blue_bits - col.g * 256.0);
		
	// NORMALIZE 0-255
	return col * 0.00390625;
}

float3 fresnel(float cos_theta, float3 mat)
{
	return mat + (1.0 - mat) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution(float3 f_norm, float3 h_norm, float roughness)
{
	roughness *= roughness;
	roughness *= roughness;
	float FdotH  = max(dot(f_norm, h_norm), 0.0);	
	float denom = (FdotH * FdotH * (roughness - 1.0) + 1.0);
	
	return roughness / (M_PI * denom * denom);
}

float reflection(float num, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float denom = num * (1.0 - k) + k;
	
	return num / denom;
}

PS_GBUFFER_OUTPUT PS_GBUFFER( PS_INPUT input)
{
    float4 clr = diffuseMap.Sample(diffuseLinear, input.texCoord) * color;
    if (clr.a < 0.05f)
    {
       discard;
    }

    PS_GBUFFER_OUTPUT output;

    output.color = clr;
	float4 material = materialMap.Sample(materialLinear, input.texCoord);
    output.material = material;
    output.normals = normalsMap.Sample(normalsLinear, input.texCoord).zyxw;
	float3 selfilum = material.a * material.rgb;
	output.selfilum = float4(selfilum, material.a);

    return output;
}

PS_INPUT VS_DEFFERED_LIGHT( VS_INPUT input )
{
	float4 posTemp = float4(input.position.x * 2.0f - 1.0f, -input.position.y * 2.0f + 1.0f, 0, 1.0f);
	
	PS_INPUT output = (PS_INPUT)0;

	output.pos = float4(posTemp.x, posTemp.y, 0.0f, 1.0f);
    output.texCoord = float2(input.position.x, input.position.y);
	
	return output;
}

float4 PS_DEFFERED_LIGHT( PS_INPUT input) : SV_Target
{
    // FRAGMENT COLOR
	float3 outColor = float3(0.0f.xxx);
	
	// SAMPLES AND MATERIALS
	float4 source    = diffuseMap.Sample(diffuseLinear, input.texCoord);
	float4 normal    = normalsMap.Sample(normalsLinear, input.texCoord);
	float4 material  = materialMap.Sample(materialLinear, input.texCoord);

	float4 selfilum = selfilumMap.Sample(selfilumLinear, input.texCoord);

	//float4 shadow    = shadow_texture.Sample(u_shadow, input.texCoord)*255.0;	
	float3 albedo    = source.rgb;
	float3 ao        = material.b * albedo;
	float  metallic  = material.r;
	float  roughness = material.g;
	float  emissive  = normal.a * 2.0;
		
	// THINGS THAT ONLY NEED CALCULATED ONCE
	float3 mat_ref   = lerp(dielectric, albedo, metallic);
	float3 f_norm    = normalize(normal.rgb * 2.0f - 1.0f);
	float  FdotC     = max(dot(f_norm, v_norm), 0.0);
	float  r_norm    = reflection(FdotC, roughness);
	FdotC *= 4.0;
	
	// AMBIENT LIGHTING PLUS EMISSIVE
	//float ambient			= sqrt(1.0 - pow(abs(u_lights[0].w) - 1.0, 2.0));
	/*float ambient_intensity = u_lights[0].w;
	float3 ambient_color = float3(u_lights[0].x, u_lights[0].y, u_lights[1].z);// *ambient + albedo * emissive;
	float3 l_norm           = normalize(float3(u_lights[1].x, u_lights[1].y, u_lights[1].z));
	float3 h_norm           = normalize(v_norm + l_norm);
	float FdotL             = max(dot(f_norm, l_norm), 0.0);
	float3 freq             = fresnel(max(dot(h_norm, v_norm), 0.0), mat_ref);
	float3 numerator		= distribution(f_norm, h_norm, roughness) * reflection(FdotL, roughness) * r_norm * freq;
	float3 specular			= numerator / (FdotC * FdotL + 0.0000001);  
	float3 refraction		= float3((1.0 - freq) * (1.0 - metallic));
	float3 ambient_lighting = ambient_color * ambient_intensity * FdotL * (refraction * albedo / M_PI + specular) * 1.0f + ao * min(ambient_intensity, 0.3); 
	*/
		
	// ITERATE THROUGH LIGHTS
	int LI = 6;
	int count = int(u_lights[1].w);
	int index = 3;

	float3 world_pos = float3(input.texCoord.x * 2.0f - 1.0f, -input.texCoord.y * 2.0f + 1.0f, 0);
	world_pos = float3(world_pos.x * u_lights[2].x + u_lights[2].z, world_pos.y * u_lights[2].y + u_lights[2].w, 0.0f);

	for (int i = 0; i < count; i++)
	{		
		/*float3 light_pos = float3(u_lights[index].x, u_lights[index].y, u_lights[index].z);
		index++;

		index++;

		float dist = 1.0f - clamp(length(light_pos.xy - world_pos.xy) / u_lights[index].w, 0.0f, 1.0f);

		color += float3(dist, dist, dist);*/

		// LIGHT ATTRIBUTES
		float3 light_pos = float3(u_lights[index].x, u_lights[index].y, u_lights[index].z);
		float directional = u_lights[index].w; // Sign of the color val1e used to flag directional lighting
		index++;

		float3 radiance = float3(u_lights[index].x, u_lights[index].y, u_lights[index].z);
		directional = float(directional < 0.0);

		float intensity = u_lights[index].w;
		index++;

		float light_depth = u_lights[index].x;

		float falloff = u_lights[index].y * intensity;
		float angle = u_lights[index].z;
		float radius = u_lights[index].w;
		index++;

		float arc = u_lights[index].x;
		float width = u_lights[index].y;
		index++;

		// LINE LIGHT
		if (width > 0.0)
		{
			float2 line_light = float2(width * sin(-angle), width * cos(-angle));
			width *= width;
			float pos = line_light.x * (world_pos.x - light_pos.x) + line_light.y * (world_pos.y - light_pos.y);
			float t = clamp(pos, -width, width) / width;
			light_pos.x += t * line_light.x;
			light_pos.y += t * line_light.y;
		}

		// NORMALIZE AFTER MOVING FOR LINE
		float3 l_norm = normalize(light_pos - world_pos);
		l_norm.x = -l_norm.x;
		//l_norm.y = -l_norm.y;
		float3 h_norm = normalize(v_norm + l_norm);
		float FdotL = max(dot(f_norm, l_norm), 0.0f);
		float fov = 1.0;

		// FOV ARC
		if (arc <= M_PI)
		{
			float angle_diff = abs(fmod(angle - atan2(l_norm.y, -l_norm.x) + M_3PI, M_2PI) - M_PI);
			radius = radius / max(0.0000001, abs(cos(angle_diff)));
			fov = smoothstep(arc, 0.0, angle_diff);
		}

		// ATTENUATION
		float dist = length(light_pos.xy - world_pos.xy);
		float det = step(dist * sign(falloff), radius);
		dist = dist * intensity / (radius * abs(falloff)) + intensity;
		//float attenuation = det / (dist * dist) * fov;
		float attenuation = max((1.0 - intensity) * directional, (1.0 - directional) * det / (dist * dist) * fov);

		//if (attenuation > 0.25)
		//{
		//	attenuation = 1.0;
		//}
		//else if (attenuation > 0.0)
		//{
		//	attenuation = 0.5;
		//}

		//color += float3(attenuation, attenuation, attenuation);

		// SHADOW AND BLEND
		//float bit = get_bit(shadow[i / 8], i % 8);
		//attenuation *= max(1.0 - bit * 0.998, ceil(blend)) * float(light_depth >= blend);

		// PBR LIGHTING
		float3 freq = fresnel(max(dot(h_norm, v_norm), 0.0), mat_ref);
		float3 numerator = distribution(f_norm, h_norm, roughness) * reflection(FdotL, roughness) * r_norm * freq;
		float3 specular = numerator / (FdotC * FdotL + 0.00000001);
		float3 refraction = float3((1.0 - freq) * (1.0 - metallic));


		// ADD TO FINAL COLOR
		outColor += radiance * attenuation *FdotL* (refraction * albedo / M_PI + specular) * 5.0f + ao * min(attenuation, 0.3);
	}

	// ADD AREAS TO BLOOM AND BLUR
	outColor = (outColor + source.rgb * color.rgb * color.a) * (1 - material.a) + material.a * material.rgb + selfilum.rgb * 8.0f;

    return float4(outColor, 1.0f);
}