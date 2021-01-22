#include "CEMaterial.h"
using namespace Concept::GraphicsEngine::Direct3D;

/*
 * Material properties must be 16-byte aligned.
 * in order to ensure alignment, to material properties is allocated in aligned memory.
 */
static MaterialProperties* NewMaterialProperties(const MaterialProperties& props) {
	MaterialProperties* materialProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*materialProperties = props;
	return materialProperties;
}

/*
 * Aligned memory must be deleted using _aligned_free method;
 */
static void DeleteMaterialProperties(MaterialProperties* props) {
	_aligned_free(props);
}

CEMaterial::CEMaterial(const MaterialProperties& materialProperties): m_materialProperties(
	NewMaterialProperties(materialProperties), &DeleteMaterialProperties) {
}

CEMaterial::CEMaterial(const CEMaterial& copy): m_materialProperties(NewMaterialProperties(*copy.m_materialProperties),
                                                                     &DeleteMaterialProperties),
                                                m_textures(copy.m_textures) {
}

const ::DirectX::XMFLOAT4& CEMaterial::GetAmbientColor() const {
	return m_materialProperties->Ambient;
}

void CEMaterial::SetAmbientColor(const ::DirectX::XMFLOAT4& ambient) {
	m_materialProperties->Ambient = ambient;
}

const ::DirectX::XMFLOAT4& CEMaterial::GetDiffuseColor() const {
	return m_materialProperties->Diffuse;
}

void CEMaterial::SetDiffuseColor(const ::DirectX::XMFLOAT4& diffuse) {
	m_materialProperties->Diffuse = diffuse;
}

const ::DirectX::XMFLOAT4& CEMaterial::GetEmissiveColor() const {
	return m_materialProperties->Emissive;
}

void CEMaterial::SetEmissiveColor(const ::DirectX::XMFLOAT4& emissive) {
	m_materialProperties->Emissive = emissive;
}

const ::DirectX::XMFLOAT4& CEMaterial::GetSpecularColor() const {
	return m_materialProperties->Specular;
}

void CEMaterial::SetSpecularColor(const ::DirectX::XMFLOAT4& specular) {
	m_materialProperties->Specular = specular;
}

float CEMaterial::GetSpecularPower() const {
	return m_materialProperties->SpecularPower;
}

void CEMaterial::SetSpecularPower(float specularPower) {
	m_materialProperties->SpecularPower = specularPower;
}

const ::DirectX::XMFLOAT4& CEMaterial::GetReflectance() const {
	return m_materialProperties->Reflectance;
}

void CEMaterial::SetReflectance(const ::DirectX::XMFLOAT4& reflectance) {
	m_materialProperties->Reflectance = reflectance;
}

const float CEMaterial::GetOpacity() const {
	return m_materialProperties->Opacity;
}

void CEMaterial::SetOpacity(float opacity) {
	m_materialProperties->Opacity = opacity;
}

float CEMaterial::GetIndexOfRefraction() const {
	return m_materialProperties->IndexOfRefraction;
}

void CEMaterial::SetIndexOfRefraction(float indexOfRefraction) {
	m_materialProperties->IndexOfRefraction = indexOfRefraction;
}

float CEMaterial::GetBumpIntensity() const {
	return m_materialProperties->BumpIntensity;
}

void CEMaterial::SetBumpIntensity(float bumpIntensity) {
	m_materialProperties->BumpIntensity = bumpIntensity;
}

std::shared_ptr<CETexture> CEMaterial::GetTexture(TextureType ID) const {
	TextureMap::const_iterator iter = m_textures.find(ID);
	if (iter != m_textures.end()) {
		return iter->second;
	}
	return nullptr;
}

void CEMaterial::SetTexture(TextureType type, std::shared_ptr<CETexture> texture) {
	m_textures[type] = texture;
	switch (type) {
	case TextureType::Ambient: {
		m_materialProperties->HasAmbientTexture = (texture != nullptr);
	}
	break;
	case TextureType::Emissive: {
		m_materialProperties->HasEmissiveTexture = (texture != nullptr);
	}
	break;
	case TextureType::Diffuse: {
		m_materialProperties->HasDiffuseTexture = (texture != nullptr);
	}
	break;
	case TextureType::Specular: {
		m_materialProperties->HasSpecularTexture = (texture != nullptr);
	}
	break;
	case TextureType::SpecularPower: {
		m_materialProperties->HasSpecularPowerTexture = (texture != nullptr);
	}
	break;
	case TextureType::Normal: {
		m_materialProperties->HasNormalTexture = (texture != nullptr);
	}
	break;
	case TextureType::Bump: {
		m_materialProperties->HasBumpTexture = (texture != nullptr);
	}
	break;
	case TextureType::Opacity: {
		m_materialProperties->HasOpacityTexture = (texture != nullptr);
	}
	break;
	}
}

bool CEMaterial::IsTransparent() const {
	return (m_materialProperties->Opacity < 1.0f || m_materialProperties->HasOpacityTexture);
}

const MaterialProperties& CEMaterial::GetMaterialProperties() const {
	return *m_materialProperties;
}

void CEMaterial::SetMaterialProperties(const MaterialProperties& materialProperties) {
	*m_materialProperties = materialProperties;
}


const MaterialProperties CEMaterial::Zero = {
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	0.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::Red = {
	{1.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.1f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::Green = {
	{0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.0f, 0.1f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::Blue = {
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.0f, 0.0f, 0.1f, 1.0f}
};

const MaterialProperties CEMaterial::Cyan = {
	{0.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.0f, 0.1f, 0.1f, 1.0f}
};

const MaterialProperties CEMaterial::Magenta = {
	{1.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.1f, 0.0f, 0.1f, 1.0f}
};

const MaterialProperties CEMaterial::Yellow = {
	{0.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.0f, 0.1f, 0.1f, 1.0f}
};

const MaterialProperties CEMaterial::White = {
	{1.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	128.0f,
	{0.1f, 0.1f, 0.1f, 1.0f}
};

const MaterialProperties CEMaterial::WhiteDiffuse = {
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	0.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::Black = {
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	0.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::Emerald = {
	{0.07568f, 0.61424f, 0.07568f, 1.0f},
	{0.633f, 0.727811f, 0.633f, 1.0f},
	76.8f,
	{0.0215f, 0.1745f, 0.0215f, 1.0f}
};

const MaterialProperties CEMaterial::Jade = {
	{0.54f, 0.89f, 0.63f, 1.0f},
	{0.316228f, 0.316228f, 0.316228f, 1.0f},
	12.8f,
	{0.135f, 0.2225f, 0.1575f, 1.0f}
};

const MaterialProperties CEMaterial::Obsidian = {
	{0.18275f, 0.17f, 0.22525f, 1.0f},
	{0.332741f, 0.328634f, 0.346435f, 1.0f},
	38.4f,
	{0.05375f, 0.05f, 0.06625f, 1.0f}
};

const MaterialProperties CEMaterial::Pearl = {
	{1.0f, 0.829f, 0.829f, 1.0f},
	{0.296648f, 0.296648f, 0.296648f, 1.0f},
	11.264f,
	{0.25f, 0.20725f, 0.20725f, 1.0f}
};

const MaterialProperties CEMaterial::Ruby = {
	{0.61424f, 0.04136f, 0.04136f, 1.0f},
	{0.727811f, 0.626959f, 0.626959f, 1.0f},
	76.8f,
	{0.1745f, 0.01175f, 0.01175f, 1.0f}
};

const MaterialProperties CEMaterial::Turquoise = {
	{0.396f, 0.74151f, 0.69102f, 1.0f},
	{0.297254f, 0.30829f, 0.306678f, 1.0f},
	12.8f,
	{0.1f, 0.18725f, 0.1745f, 1.0f}
};

const MaterialProperties CEMaterial::Brass = {
	{0.780392f, 0.568627f, 0.113725f, 1.0f},
	{0.992157f, 0.941176f, 0.807843f, 1.0f},
	27.9f,
	{0.329412f, 0.223529f, 0.027451f, 1.0f}
};

const MaterialProperties CEMaterial::Bronze = {
	{0.714f, 0.4284f, 0.18144f, 1.0f},
	{0.393548f, 0.271906f, 0.166721f, 1.0f},
	25.6f,
	{0.2125f, 0.1275f, 0.054f, 1.0f}
};

const MaterialProperties CEMaterial::Chrome = {
	{0.4f, 0.4f, 0.4f, 1.0f},
	{0.774597f, 0.774597f, 0.774597f, 1.0f},
	76.8f,
	{0.25f, 0.25f, 0.25f, 1.0f}
};

const MaterialProperties CEMaterial::Copper = {
	{0.7038f, 0.27048f, 0.0828f, 1.0f},
	{0.256777f, 0.137622f, 0.086014f, 1.0f},
	12.8f,
	{0.19125f, 0.0735f, 0.0225f, 1.0f}
};

const MaterialProperties CEMaterial::Gold = {
	{0.75164f, 0.60648f, 0.22648f, 1.0f},
	{0.628281f, 0.555802f, 0.366065f, 1.0f},
	51.2f,
	{0.24725f, 0.1995f, 0.0745f, 1.0f}
};

const MaterialProperties CEMaterial::Silver = {
	{0.50754f, 0.50754f, 0.50754f, 1.0f},
	{0.508273f, 0.508273f, 0.508273f, 1.0f},
	51.2f,
	{0.19225f, 0.19225f, 0.19225f, 1.0f}
};

const MaterialProperties CEMaterial::BlackPlastic = {
	{0.01f, 0.01f, 0.01f, 1.0f},
	{0.5f, 0.5f, 0.5f, 1.0f},
	32.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::CyanPlastic = {
	{0.0f, 0.50980392f, 0.50980392f, 1.0f},
	{0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
	32.0f,
	{0.0f, 0.1f, 0.06f, 1.0f}
};

const MaterialProperties CEMaterial::GreenPlastic = {
	{0.1f, 0.35f, 0.1f, 1.0f},
	{0.45f, 0.55f, 0.45f, 1.0f},
	32.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::RedPlastic = {
	{0.5f, 0.0f, 0.0f, 1.0f},
	{0.7f, 0.6f, 0.6f, 1.0f},
	32.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::WhitePlastic = {
	{0.55f, 0.55f, 0.55f, 1.0f},
	{0.7f, 0.7f, 0.7f, 1.0f},
	32.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::YellowPlastic = {
	{0.5f, 0.5f, 0.0f, 1.0f},
	{0.6f, 0.6f, 0.5f, 1.0f},
	32.0f,
	{0.0f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::BlackRubber = {
	{0.01f, 0.01f, 0.01f, 1.0f},
	{0.4f, 0.4f, 0.4f, 1.0f},
	10.0f,
	{0.02f, 0.02f, 0.02f, 1.0f}
};

const MaterialProperties CEMaterial::CyanRubber = {
	{0.4f, 0.5f, 0.5f, 1.0f},
	{0.04f, 0.7f, 0.7f, 1.0f},
	10.0f,
	{0.0f, 0.05f, 0.05f, 1.0f}
};

const MaterialProperties CEMaterial::GreenRubber = {
	{0.4f, 0.5f, 0.4f, 1.0f},
	{0.04f, 0.7f, 0.04f, 1.0f},
	10.0f,
	{0.0f, 0.05f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::RedRubber = {
	{0.5f, 0.4f, 0.4f, 1.0f},
	{0.7f, 0.04f, 0.04f, 1.0f},
	10.0f,
	{0.05f, 0.0f, 0.0f, 1.0f}
};

const MaterialProperties CEMaterial::WhiteRubber = {
	{0.5f, 0.5f, 0.5f, 1.0f},
	{0.7f, 0.7f, 0.7f, 1.0f},
	10.0f,
	{0.05f, 0.05f, 0.05f, 1.0f}
};

const MaterialProperties CEMaterial::YellowRubber = {
	{0.5f, 0.5f, 0.4f, 1.0f},
	{0.7f, 0.7f, 0.04f, 1.0f},
	10.0f,
	{0.05f, 0.05f, 0.0f, 1.0f}
};
