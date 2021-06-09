#pragma once

#include "../Graphics/Main/CEGraphics.h"
#include "../Core/Compilers/CECompiler.h"
#include "../Core/Platform/Generic/CEPlatform.h"

//TODO: Make it extern in every class owner, for not leave it here to fix error

static ConceptEngine::Graphics::Main::CEGraphics* GGraphics;
static ConceptEngine::Core::Compilers::CECompiler* GCompiler;
static ConceptEngine::Core::Generic::Platform::CEPlatform* GPlatform;
