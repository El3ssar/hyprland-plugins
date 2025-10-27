#include "OverviewPassElement.hpp"
#include <hyprland/src/render/OpenGL.hpp>
#include "overview.hpp"

COverviewPassElement::COverviewPassElement() {
    ;
}

void COverviewPassElement::draw(const CRegion& damage) {
    // g_pOverview may be reset by animation callbacks; guard against null
    if (!g_pOverview)
        return;

    g_pOverview->fullRender();
}

bool COverviewPassElement::needsLiveBlur() {
    return false;
}

bool COverviewPassElement::needsPrecomputeBlur() {
    return false;
}

std::optional<CBox> COverviewPassElement::boundingBox() {
    if (!g_pOverview)
        return std::nullopt;

    auto monitor = g_pOverview->pMonitor.lock();
    if (!monitor)
        return std::nullopt;

    return CBox{{}, monitor->m_size};
}

CRegion COverviewPassElement::opaqueRegion() {
    if (!g_pOverview)
        return CRegion{};

    auto monitor = g_pOverview->pMonitor.lock();
    if (!monitor)
        return CRegion{};

    return CBox{{}, monitor->m_size};
}

