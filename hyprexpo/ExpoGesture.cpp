#include "ExpoGesture.hpp"

#include "overview.hpp"

#include <algorithm>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>

 void CExpoGesture::begin(const ITrackpadGesture::STrackpadGestureBegin& e) {
    ITrackpadGesture::begin(e);
 
    m_lastDelta   = 0.F;
    m_firstUpdate = true;
 
    // Asegura monitor/workspace válidos antes de crear el overview.
    if (!g_pOverview) {
        if (!g_pCompositor || g_pCompositor->m_lastMonitor.expired())
            return;
        const auto ws = g_pCompositor->m_lastMonitor->m_activeWorkspace; // PHLWORKSPACE
        if (!ws)
            return;
        g_pOverview = std::make_unique<COverview>(ws);
    } else {
        g_pOverview->selectHoveredWorkspace();
        g_pOverview->setClosing(true);
    }
 }

 void CExpoGesture::update(const ITrackpadGesture::STrackpadGestureUpdate& e) {
    if (!g_pOverview)
        return;
    if (m_firstUpdate) {
        m_firstUpdate = false;
        return;
    }

    m_lastDelta += distance(e);

    // Algunos touchpads reportan <= 0; clamp estable para evitar crash.
    m_lastDelta = std::max(m_lastDelta, 0.01f);

    g_pOverview->onSwipeUpdate(m_lastDelta);
 }

void CExpoGesture::end(const ITrackpadGesture::STrackpadGestureEnd& e) {
    if (!g_pOverview) return;
    g_pOverview->setClosing(false); // luego desbloquea cierre
    g_pOverview->onSwipeEnd();      // puede programar removeOverview() al final de la animación
    g_pOverview->resetSwipe();      // primero resetea estado interno del swipe
}
