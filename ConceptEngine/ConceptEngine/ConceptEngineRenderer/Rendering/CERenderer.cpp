#include "CERenderer.h"

void CERenderer::OnWindowResize(const WindowResizeEvent& Event)
{
    ResizeResources(Event.Width, Event.Height);
}

