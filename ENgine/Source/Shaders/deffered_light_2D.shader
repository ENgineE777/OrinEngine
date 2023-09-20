#define M_PI   3.1415926535897932384626433832795
#define M_PI_2 1.5707963267948966192313216916398
#define M_2PI  6.283185307179586476925286766559
#define M_3PI  9.4247779607693797153879301498385

static const float3 dielectric      = float3(0.4f.xxx); 
static const float3 v_norm          = float3(0.0, 0.0, 1.0);

cbuffer vs_params : register(b0)
{
	float4 shadowParams;
};

cbuffer ps_params : register( b0 )
{
	float4 color;
	float4 emmisive;
	matrix normalTrans;
	float4 params;
	float4 u_lights[9 + 4 * 40];
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

Texture2D shadowMap : register(t4);
SamplerState shadowLinear : register(s4);


struct PS_GBUFFER_OUTPUT
{
    float4 color : SV_TARGET0;
    float4 material : SV_TARGET1;
    float4 normals : SV_TARGET2;
	float3 selfilum : SV_TARGET3;
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
    float4 clr = diffuseMap.Sample(diffuseLinear, input.texCoord);

	if (clr.a < 0.05f)
	{
		discard;
	}

	clr = lerp(float4(clr.rgb, 1.0f), float4(color.rgb, 1.0f), 1.0f - color.a);

    PS_GBUFFER_OUTPUT output;

    output.color = clr;
	float4 material = materialMap.Sample(materialLinear, input.texCoord);
    output.material = material;
	output.material.g = params.x;
	output.material.a = params.y;

	float4 normal = normalsMap.Sample(normalsLinear, input.texCoord);
	float3 f_norm = normalize(normal.rgb * 2.0f - 1.0f);
	normal = mul(float4(f_norm.xyz, 1.0f), normalTrans);
	f_norm = normal.rgb * 0.5f + 0.5f;

    output.normals = float4(f_norm.rgb, 1.0f);
	output.selfilum = float3(clr.rgb * emmisive.rgb * material.b * params.z);

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

PS_INPUT VS_CAST_SHADOW(VS_INPUT input)
{
	float cell = (shadowParams.x + input.position.y) * shadowParams.y;

	float4 posTemp = float4(input.position.x * 2.0f - 1.0f, -cell * 2.0f + 1.0f, 0, 1.0f);

	PS_INPUT output = (PS_INPUT)0;

	output.pos = float4(posTemp.x, posTemp.y, 0.0f, 1.0f);
	output.texCoord = float2(input.position.x, input.position.y);

	return output;
}

float PS_CAST_SHADOW(PS_INPUT input) : SV_Target
{
	float2 start_pos = float2(0.5f, 0.5f);
	float angle = M_PI_2 * input.texCoord.x * 4.0f - M_PI;
	float2 end_pos = start_pos + float2(cos(angle), sin(angle)) * 0.5f;
	
	float2 delta = end_pos - start_pos;

	int steps = (int)(abs(delta.x) > abs(delta.y) ? abs(delta.x / params.x) : abs(delta.y / params.y));

	delta = delta / (float)steps;

	float2 curPoint = start_pos;

	for (int i = 0; i <= steps; i++)
	{	
		float4 clr = diffuseMap.SampleLevel(diffuseLinear, curPoint, 0) * color;

		if (clr.r < 0.1f)
		{
			break;
		}

		curPoint += delta;		
	}

	return length(start_pos - curPoint);
}

float shadowSample(float2 coord, float r)
{
	return step(r, diffuseMap.SampleLevel(diffuseLinear, coord, 0).r);
}

float4 PS_RENDER_SHADOW(PS_INPUT input) : SV_Target
{
	float2 norm = input.texCoord - float2(0.5f, 0.5f);
	float theta = atan2(norm.y, norm.x);
	float r = min(length(norm), 0.5f);
	float coord = (theta + M_PI) / (2.0f * M_PI);
	
	float2 tc = float2(coord, 0.0f);

	//the center tex coord, which gives us hard shadows
	float center = shadowSample(tc, r);

	//we multiply the blur amount by our distance from center
	//this leads to more blurriness as the shadow "fades away"
	float blur = (1.0f / 360.0f) * smoothstep(0.0f, 1.0f, r * 2.0f);

	//now we use a simple gaussian blur
	float sum = 0.0;

	sum += shadowSample(float2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
	sum += shadowSample(float2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
	sum += shadowSample(float2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
	sum += shadowSample(float2(tc.x - 1.0 * blur, tc.y), r) * 0.15;

	sum += center * 0.16;

	sum += shadowSample(float2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
	sum += shadowSample(float2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
	sum += shadowSample(float2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
	sum += shadowSample(float2(tc.x + 4.0 * blur, tc.y), r) * 0.05;

	//1.0 -> in light, 0.0 -> in shadow
	float shadow = sum * smoothstep(1.0f, 0.0f, r * 2.0f);	

	return float4(shadow, shadow, shadow, 1.0f) * color;
}

float shadowMapSample(float2 coord, float r)
{
	return step(r, shadowMap.SampleLevel(shadowLinear, coord, 0).r);
}


//a random texture generator, but you can also use a pre-computed perturbation texture
float4 rnm(float2 tc)
{
	float noise = sin(dot(tc + float2(u_lights[0].x, u_lights[0].x), float2(12.9898f, 78.233f))) * 43758.5453f;
	float noiseR = frac(noise) * 2.0f - 1.0f;
	float noiseG = frac(noise * 1.2154f) * 2.0f - 1.0f;
	float noiseB = frac(noise * 1.3453f) * 2.0f - 1.0f;
	float noiseA = frac(noise * 1.3647f) * 2.0f - 1.0f;

	return float4(noiseR, noiseG, noiseB, noiseA);
}

float fade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float pnoise3D(float3 p)
{
	float3 pi = 0.00390625 * floor(p);
	pi = float3(pi.x + 0.001953125, pi.y + 0.001953125, pi.z + 0.001953125);
	float3 pf = frac(p);     // Fractional part for interpolation

	// Noise contributions from (x=0, y=0), z=0 and z=1
	float perm00 = rnm(pi.xy).a;
	float3 grad000 = rnm(float2(perm00, pi.z)).rgb * 4.0;
	grad000 = float3(grad000.x - 1.0, grad000.y - 1.0, grad000.z - 1.0);
	float n000 = dot(grad000, pf);
	float3 grad001 = rnm(float2(perm00, pi.z + 0.00390625)).rgb * 4.0;
	grad001 = float3(grad001.x - 1.0, grad001.y - 1.0, grad001.z - 1.0);
	float n001 = dot(grad001, pf - float3(0.0, 0.0, 1.0));

	// Noise contributions from (x=0, y=1), z=0 and z=1
	float perm01 = rnm(pi.xy + float2(0.0, 0.00390625)).a;
	float3 grad010 = rnm(float2(perm01, pi.z)).rgb * 4.0;
	grad010 = float3(grad010.x - 1.0, grad010.y - 1.0, grad010.z - 1.0);
	float n010 = dot(grad010, pf - float3(0.0, 1.0, 0.0));
	float3 grad011 = rnm(float2(perm01, pi.z + 0.00390625)).rgb * 4.0;
	grad011 = float3(grad011.x - 1.0, grad011.y - 1.0, grad011.z - 1.0);
	float n011 = dot(grad011, pf - float3(0.0, 1.0, 1.0));

	// Noise contributions from (x=1, y=0), z=0 and z=1
	float perm10 = rnm(pi.xy + float2(0.00390625, 0.0)).a;
	float3  grad100 = rnm(float2(perm10, pi.z)).rgb * 4.0;
	grad100 = float3(grad100.x - 1.0, grad100.y - 1.0, grad100.z - 1.0);
	float n100 = dot(grad100, pf - float3(1.0, 0.0, 0.0));
	float3  grad101 = rnm(float2(perm10, pi.z + 0.00390625)).rgb * 4.0;
	grad101 = float3(grad101.x - 1.0, grad101.y - 1.0, grad101.z - 1.0);
	float n101 = dot(grad101, pf - float3(1.0, 0.0, 1.0));

	// Noise contributions from (x=1, y=1), z=0 and z=1
	float perm11 = rnm(pi.xy + float2(0.00390625, 0.00390625)).a;
	float3  grad110 = rnm(float2(perm11, pi.z)).rgb * 4.0;
	grad110 = float3(grad110.x - 1.0, grad110.y - 1.0, grad110.z - 1.0);
	float n110 = dot(grad110, pf - float3(1.0, 1.0, 0.0));
	float3  grad111 = rnm(float2(perm11, pi.z + 0.00390625)).rgb * 4.0;
	grad111 = float3(grad111.x - 1.0, grad111.y - 1.0, grad111.z - 1.0);
	float n111 = dot(grad111, pf - float3(1.0, 1.0, 1.0));

	// Blend contributions along x
	float4 n_x = lerp(float4(n000, n001, n010, n011), float4(n100, n101, n110, n111), fade(pf.x));

	// Blend contributions along y
	float2 n_xy = lerp(n_x.xy, n_x.zw, fade(pf.y));

	// Blend contributions along z
	float n_xyz = lerp(n_xy.x, n_xy.y, fade(pf.z));

	// We're done, return the final noise value.
	return n_xyz;
}

float4 PS_DEFFERED_LIGHT( PS_INPUT input) : SV_Target
{
    // FRAGMENT COLOR
	float3 outColor = float3(0.0f.xxx);
	
	// SAMPLES AND MATERIALS
	float4 source    = diffuseMap.Sample(diffuseLinear, input.texCoord);
	float4 normal    = normalsMap.Sample(normalsLinear, input.texCoord);
	float4 material  = materialMap.Sample(materialLinear, input.texCoord);

	int selfLightGroup = round(material.g * 64.0f);

	float3 selfilum = selfilumMap.Sample(selfilumLinear, input.texCoord).rgb;

	//float4 shadow    = shadow_texture.Sample(u_shadow, input.texCoord)*255.0;	
	float3 albedo    = source.rgb;
	float3 ao        = material.b * albedo;
	float  metallic = params.x;//material.r;
	float  roughness = 1.0f - material.r * 0.5f;
	//float  emissive  = normal.a * 2.0;
		
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
	int index = 9;

	float3 world_pos = float3(input.texCoord.x * 2.0f - 1.0f, -input.texCoord.y * 2.0f + 1.0f, 0);
	world_pos = float3(world_pos.x * u_lights[2].x + u_lights[2].z, world_pos.y * u_lights[2].y + u_lights[2].w, 0.0f);

	for (int i = 0; i < count; i++)
	{
		// LIGHT ATTRIBUTES
		float3 light_pos = float3(u_lights[index].x, u_lights[index].y, u_lights[index].z * (material.a > 0.5f ? -0.0f : 1.0f));
		float directional = u_lights[index].w; // Sign of the color val1e used to flag directional lighting
		index++;

		float3 radiance = float3(u_lights[index].x, u_lights[index].y, u_lights[index].z);
		directional = float(directional < 0.0);

		float intensity = u_lights[index].w;
		index++;

		float cast_shadow = u_lights[index].x;
		float falloff = u_lights[index].y;
		float angle = -u_lights[index].z;
		float radius = u_lights[index].w;
		index++;

		float arc = u_lights[index].x;
		float width = u_lights[index].y;
		int lightGroup = round(u_lights[index].z * 64.0f);
		index++;

		// LINE LIGHT
		if (width > 0.0)
		{
			float2 line_light = float2(width * sin(angle), width * cos(angle));
			width *= width;
			float pos = line_light.x * (world_pos.x - light_pos.x) + line_light.y * (world_pos.y - light_pos.y);
			float t = clamp(pos, -width, width) / width;
			light_pos.x += t * line_light.x;
			light_pos.y += t * line_light.y;
		}

		// NORMALIZE AFTER MOVING FOR LINE
		float3 l_norm = normalize(light_pos + world_pos * directional - world_pos);
		float3 h_norm = normalize(v_norm + l_norm);
		float FdotL = max(dot(f_norm, l_norm), 0.0f);		

		float fov = 1.0;

		float ligthRadius = radius;

		// FOV ARC
		if (arc <= M_PI)
		{
			float angle_diff = abs(fmod(angle - atan2(l_norm.y, -l_norm.x) + M_3PI, M_2PI) - M_PI);
			radius = radius / max(0.0000001, abs(cos(angle_diff)));
			fov = smoothstep(arc, 0.0, angle_diff);
		}

		// ATTENUATION
		float2 dir = world_pos.xy - light_pos.xy;
		dir.y = -dir.y;
		float dist = min(length(dir), radius) / radius;

		dist = dist < falloff ? 0.0f : smoothstep(0.0f, 1.0f, (dist - falloff)/ (1.0f - falloff));

		//float det = step(dist * sign(falloff), radius);
		//dist = dist * intensity / (radius * abs(falloff)) + intensity;
		//float attenuation = det / (dist * dist) * fov;
		//float attenuation = max((intensity) * directional, (1.0 - directional) * det / (dist * dist) * fov);
		float k = (1.0f - dist);
		float attenuation = max( intensity * directional, (1.0f - dist) * fov * intensity);


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

		float shadow = 1.0f;

		if (directional < 0.5f && cast_shadow > -0.5f && material.g < 0.5f)
		{
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
			shadow = 0.0;

			shadow += shadowMapSample(float2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
			shadow += shadowMapSample(float2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
			shadow += shadowMapSample(float2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
			shadow += shadowMapSample(float2(tc.x - 1.0 * blur, tc.y), r) * 0.15;

			shadow += center * 0.16;

			shadow += shadowMapSample(float2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
			shadow += shadowMapSample(float2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
			shadow += shadowMapSample(float2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
			shadow += shadowMapSample(float2(tc.x + 4.0 * blur, tc.y), r) * 0.05;

			//1.0 -> in light, 0.0 -> in shadow
			//shadow = sum * smoothstep(1.0f, 0.0f, r * 2.0f);
		}

		// PBR LIGHTING
		float3 freq = fresnel(max(dot(h_norm, v_norm), 0.0), mat_ref);
		float3 numerator = distribution(f_norm, h_norm, roughness) * reflection(FdotL, roughness) * r_norm * freq;
		float3 specular = numerator / (FdotC * FdotL + 0.00000001);
		float3 refraction = float3((1.0 - freq) * (1.0 - metallic));


		if (selfLightGroup & lightGroup)
		{
			// ADD TO FINAL COLOR
			outColor += radiance * attenuation * FdotL * (refraction * ( material.a > 0.5f ? 0.75f : albedo) / M_PI + specular) * 5.0f * (0.0f + shadow * 1.0f) + ao * min(attenuation, 0.3);
		}
	}

	// ADD AREAS TO BLOOM AND BLUR
	float lightFactor = max(outColor.r, outColor.g);
	lightFactor = max(lightFactor, outColor.b);

	int lightIndex = 0;
	while (selfLightGroup >>= 1)
	{
		lightIndex++;
	}

	float3 albedo2 = u_lights[lightIndex + 3].rgb * u_lights[lightIndex + 3].a;

	outColor = (outColor * lightFactor + source.rgb * albedo2 * ((1.0f - lightFactor) * 0.7f + 0.3f)) * (1 - material.b) + source.rgb * material.b + selfilum.rgb * u_lights[0].z;

	if (u_lights[0].y > 0.5f)
	{
		float lum = outColor.r * 0.299 + outColor.g * 0.587 + outColor.b * 0.114;
		outColor = float3(lum, lum, lum);
	}

	float grain_amount = u_lights[0].y > 0.5f ? 0.05 : 0.02;
	float color_amount = 0.6;
	float grain_size = u_lights[0].y > 0.5f ? 1.2 : 0.333f;
	float lum_amount = 1.0f;

	float sx_y = (u_lights[2].y * 2.0f);
	float uv_y = input.texCoord.y * sx_y;
	int uv_k = (int)(uv_y / 4.0f);
	uv_y = uv_y - uv_k * 4.0f;

	float2 sz = float2(u_lights[2].x / grain_size, u_lights[2].y / grain_size) * input.texCoord;
	float3 noise = pnoise3D(float3(sz.x, sz.y, 0.0));
		
	float3 lumcoeff = float3(0.299, 0.587, 0.114);
	float luminance = lerp(0.0, dot(outColor, lumcoeff), lum_amount);
	float lum = smoothstep(0.2, 0.0, luminance);
	lum += luminance;
	
	noise = lerp(noise, float3(0.0f, 0.0f, 0.0f), pow(lum, 4.0));

	if (u_lights[0].y > 0.5f)
	{
		outColor *= float3(102.0f / 255.0f, 100.0f / 255.0f, 138.0f / 255.0f) * 1.5f;

		outColor *= sin(uv_y * 1.54f) * 0.2f + 0.8f;
		outColor *= 2.0f - max(length(input.texCoord - float2(0.5f, 0.5f)), 0.35f) * 2.85f;
	}

	outColor = outColor + noise * grain_amount;		


    return float4(outColor, 1.0f);
}