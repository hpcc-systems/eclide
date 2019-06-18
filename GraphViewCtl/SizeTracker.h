#pragma once

#include "GraphTypes.h"
#include "Canvas.h"

struct CSizeTracker
{
    GraphTypes::SizeF m_scaledExtent, m_extent;
    GraphTypes::REAL m_scale;
    int m_dpiX, m_dpiY;
    // sizes that vary with DPI 
    SizeFMap m_extentMap;

    CSizeTracker() : m_scale(0), m_dpiX(0), m_dpiY(0) {}
    
    bool Update(CCanvas* canvas)
    {
        GraphTypes::REAL scale=canvas->GetScale();
        int	dpiX=canvas->GetDPIX(), dpiY=canvas->GetDPIY();
        bool ret=scale!=m_scale || dpiX!=m_dpiX || dpiY!=m_dpiY;
        if(ret)
        {
            m_scale=scale;
            m_dpiX=dpiX;
            m_dpiY=dpiY;
        }
        return ret;
    }

    void Reset()
    {
        m_scaledExtent=GraphTypes::SizeF();
        m_extent=GraphTypes::SizeF();
        m_scale=0;
        m_dpiX=m_dpiY=0;
        m_extentMap.clear();
    }
};
