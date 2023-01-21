#version 330 core

struct PositionalLight {
	vec3 Position;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct Material {
	// NOTE(Jovan): Diffuse is used as ambient as well since the light source
	// defines the ambient colour
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

uniform PositionalLight uPointLightKhufu;
uniform PositionalLight uPointLightKhafre;
uniform PositionalLight uPointLightMenkaure;
uniform DirectionalLight uSpotlight;
uniform DirectionalLight uDirLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

in vec2 TexCoords;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	// Directional light
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, TexCoords));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

	// Point lights
	// 1
	vec3 PtLightVector1 = normalize(uPointLightKhufu.Position - vWorldSpaceFragment);
	float PtDiffuse1 = max(dot(vWorldSpaceNormal, PtLightVector1), 0.0f);
	vec3 PtReflectDirection1 = reflect(-PtLightVector1, vWorldSpaceNormal);
	float PtSpecular1 = pow(max(dot(ViewDirection, PtReflectDirection1), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor1 = uPointLightKhufu.Ka * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtDiffuseColor1 = PtDiffuse1 * uPointLightKhufu.Kd * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtSpecularColor1 = PtSpecular1 * uPointLightKhufu.Ks * vec3(texture(uMaterial.Ks, TexCoords));

	float PtLightDistance1 = length(uPointLightKhufu.Position - vWorldSpaceFragment);
	float PtAttenuation1 = 1.0f / (uPointLightKhufu.Kc + uPointLightKhufu.Kl * PtLightDistance1 + uPointLightKhufu.Kq * (PtLightDistance1 * PtLightDistance1));
	vec3 PtColor1 = PtAttenuation1 * (PtAmbientColor1 + PtDiffuseColor1 + PtSpecularColor1);

	// 2
	vec3 PtLightVector2 = normalize(uPointLightKhafre.Position - vWorldSpaceFragment);
	float PtDiffuse2 = max(dot(vWorldSpaceNormal, PtLightVector2), 0.0f);
	vec3 PtReflectDirection2 = reflect(-PtLightVector2, vWorldSpaceNormal);
	float PtSpecular2 = pow(max(dot(ViewDirection, PtReflectDirection2), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor2 = uPointLightKhafre.Ka * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtDiffuseColor2 = PtDiffuse2 * uPointLightKhafre.Kd * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtSpecularColor2 = PtSpecular2 * uPointLightKhafre.Ks * vec3(texture(uMaterial.Ks, TexCoords));

	float PtLightDistance2 = length(uPointLightKhafre.Position - vWorldSpaceFragment);
	float PtAttenuation2 = 1.0f / (uPointLightKhafre.Kc + uPointLightKhafre.Kl * PtLightDistance2 + uPointLightKhafre.Kq * (PtLightDistance2 * PtLightDistance2));
	vec3 PtColor2 = PtAttenuation2 * (PtAmbientColor2 + PtDiffuseColor2 + PtSpecularColor2);

	// 3
	vec3 PtLightVector3 = normalize(uPointLightMenkaure.Position - vWorldSpaceFragment);
	float PtDiffuse3 = max(dot(vWorldSpaceNormal, PtLightVector3), 0.0f);
	vec3 PtReflectDirection3 = reflect(-PtLightVector3, vWorldSpaceNormal);
	float PtSpecular3 = pow(max(dot(ViewDirection, PtReflectDirection3), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor3 = uPointLightMenkaure.Ka * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtDiffuseColor3 = PtDiffuse3 * uPointLightMenkaure.Kd * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 PtSpecularColor3 = PtSpecular3 * uPointLightMenkaure.Ks * vec3(texture(uMaterial.Ks, TexCoords));

	float PtLightDistance3 = length(uPointLightMenkaure.Position - vWorldSpaceFragment);
	float PtAttenuation3 = 1.0f / (uPointLightMenkaure.Kc + uPointLightMenkaure.Kl * PtLightDistance3 + uPointLightMenkaure.Kq * (PtLightDistance3 * PtLightDistance3));
	vec3 PtColor3 = PtAttenuation3 * (PtAmbientColor3 + PtDiffuseColor3 + PtSpecularColor3);

	// Spotlight
	vec3 SpotlightVector = normalize(uSpotlight.Position - vWorldSpaceFragment);

	float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
	vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
	float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor = uSpotlight.Ka * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 SpotDiffuseColor = SpotDiffuse * uSpotlight.Kd * vec3(texture(uMaterial.Kd, TexCoords));
	vec3 SpotSpecularColor = SpotSpecular * uSpotlight.Ks * vec3(texture(uMaterial.Ks, TexCoords));

	float SpotlightDistance = length(uSpotlight.Position - vWorldSpaceFragment);
	float SpotAttenuation = 1.0f / (uSpotlight.Kc + uSpotlight.Kl * SpotlightDistance + uSpotlight.Kq * (SpotlightDistance * SpotlightDistance));

	float Theta = dot(SpotlightVector, normalize(-uSpotlight.Direction));
	float Epsilon = uSpotlight.InnerCutOff - uSpotlight.OuterCutOff;
	float SpotIntensity = clamp((Theta - uSpotlight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
	vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);
	
	vec3 FinalColor = DirColor + PtColor1 + PtColor2 + PtColor3 + SpotColor;
	FragColor = vec4(FinalColor, 1.0f);
}
