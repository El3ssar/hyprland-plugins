#include "ExpoGesture.hpp"

#include "overview.hpp"

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>

void CExpoGesture::begin(const ITrackpadGesture::STrackpadGestureBegin& e) {
    ITrackpadGesture::begin(e);

    m_lastDelta   = 0.F;
    m_firstUpdate = true;

    if (!g_pOverview) {
        // Guard against invalid monitor during overview creation
        auto monitor = g_pCompositor->m_lastMonitor.lock();
        if (!monitor || !monitor->m_activeWorkspace)
            return;
        
        g_pOverview = std::make_unique<COverview>(monitor->m_activeWorkspace);
    } else if (!g_pOverview->m_isSwiping) {
        // Only toggle if not currently swiping to avoid re-entrancy
        // Guard against invalid monitor
        auto monitor = g_pOverview->pMonitor.lock();
        if (!monitor)
            return;
            
        g_pOverview->selectHoveredWorkspace();
        g_pOverview->setClosing(true);
    }
}

void CExpoGesture::update(const ITrackpadGesture::STrackpadGestureUpdate& e) {
    if (m_firstUpdate) {
        m_firstUpdate = false;
        return;
    }

    m_lastDelta += distance(e);

    if (m_lastDelta <= 0.01) // plugin will crash if swipe ends at <= 0
        m_lastDelta = 0.01;

    // g_pOverview can be reset by animation callbacks; guard against null
    if (!g_pOverview)
        return;

    g_pOverview->onSwipeUpdate(m_lastDelta);
}

void CExpoGesture::end(const ITrackpadGesture::STrackpadGestureEnd& e) {
    // g_pOverview may have been cleared between update/end (animation callback).
    if (!g_pOverview)
        return;

    g_pOverview->setClosing(false);
    g_pOverview->onSwipeEnd();
    g_pOverview->resetSwipe();
}

