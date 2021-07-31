#pragma once
#include "../Core/Input/InputCodes.h"
#include "../Core/Application/Events.h"

#include "../RenderLayer/Resources.h"
#include "../RenderLayer/ResourceViews.h"

#include "../Core/CERef.h"

#include "../Dependencies/imgui/imgui.h"

// Used when rendering images with ImGui
struct ImGuiImage
{
    ImGuiImage() = default;

    ImGuiImage(const CERef<ShaderResourceView>& InImageView, const CERef<Texture>& InImage, EResourceState InBefore, EResourceState InAfter)
        : ImageView(InImageView)
        , Image(InImage)
        , BeforeState(InBefore)
        , AfterState(InAfter)
    {
    }

    CERef<ShaderResourceView> ImageView;
    CERef<Texture>  Image;
    EResourceState BeforeState;
    EResourceState AfterState;
    bool AllowBlending = false;
};

class CEDebugUI
{
public:
    typedef void(*UIDrawFunc)();

    static bool Create();
    static void Release();

    static void DrawUI(UIDrawFunc DrawFunc);
    static void DrawDebugString(const std::string& DebugString);
    
    static void OnKeyPressed(const KeyPressedEvent& Event);
    static void OnKeyReleased(const KeyReleasedEvent& Event);
    static void OnKeyTyped(const KeyTypedEvent& Event);

    static void OnMousePressed(const MousePressedEvent& Event);
    static void OnMouseReleased(const MouseReleasedEvent& Event);
    static void OnMouseScrolled(const MouseScrolledEvent& Event);

    // Should only be called by the renderer
    static void Render(class CommandList& CmdList);

    static ImGuiContext* GetCurrentContext();
};