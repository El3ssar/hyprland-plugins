#include "ExpoGesture.hpp"

#include "overview.hpp"

#include <algorithm>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>

void CExpoGesture::begin(const ITrackpadGesture::STrackpadGestureBegin& e) {
    ITrackpadGesture::begin(e);

    m_lastDelta   = 0.F;
    m_firstUpdate = true;

    // Robustez: valida monitor/workspace antes de crear Overview
    auto* pMon = (g_pCompositor && g_pCompositor->m_lastMonitor) ? g_pCompositor->m_lastMonitor : nullptr;
    auto* pWS  = pMon ? pMon->m_activeWorkspace : nullptr;

    if (!g_pOverview) {
        if (!pWS) return; // sin workspace activo ⇒ evita nullptr
        g_pOverview = std::make_unique<COverview>(pWS);
    } else {
        // Si ya existe, marca “closing” para que la animación corra en sentido correcto
        g_pOverview->selectHoveredWorkspace();
        g_pOverview->setClosing(true);
    }
}

void CExpoGesture::update(const ITrackpadGesture::STrackpadGestureUpdate& e) {
    if (!g_pOverview) return;
    if (m_firstUpdate) {
        m_firstUpdate = false;
        return;
    }

    m_lastDelta += distance(e);

    // Algunos touchpads reportan delta negativo con 4 dedos vertical ⇒ clamp estable
    m_lastDelta = std::max(m_lastDelta, 0.01f); // evita crash con ≤ 0
 
    g_pOverview->onSwipeUpdate(m_lastDelta);
}

void CExpoGesture::end(const ITrackpadGesture::STrackpadGestureEnd& e) {
    if (!g_pOverview) return;
    // Importante: no accedas a g_pOverview tras onSwipeEnd() porque puede cerrarse
    // y disparar removeOverview() (reset del unique_ptr) al terminar la animación.
    g_pOverview->resetSwipe();      // primero resetea estado interno del swipe
    g_pOverview->setClosing(false); // luego desbloquea cierre
    g_pOverview->onSwipeEnd();      // puede programar removeOverview() al final de la animación
    // No tocar g_pOverview después de aquí.
}
