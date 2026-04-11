#include "pch.h"
#include "GeekGlass.h"
#include "EditState.h"
#include <cmath>

extern AppConfig g_config;

namespace QuickView::UI::GeekGlass {

void GeekGlassEngine::InitializeResources(ID2D1RenderTarget* pRT) {
    if (!pRT) return;
    
    // Effects require ID2D1DeviceContext (Windows 8+)
    ComPtr<ID2D1DeviceContext> pContext;
    if (FAILED(pRT->QueryInterface(IID_PPV_ARGS(&pContext)))) return;

    // Cache expensive D2D effects ahead of time, applying best practice for performance.
    pContext->CreateEffect(CLSID_D2D1GaussianBlur, &m_blurEffect);
    pContext->CreateEffect(CLSID_D2D1Crop, &m_cropEffect);
    pContext->CreateEffect(CLSID_D2D12DAffineTransform, &m_transformEffect);

    // Lock the blur behavior to HARD borders to prevent bleeding out from edges (transparent sampling).
    if (m_blurEffect) {
        m_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    }
}

void GeekGlassEngine::ReleaseResources() {
    m_blurEffect.Reset();
    m_cropEffect.Reset();
    m_transformEffect.Reset();
    m_diagonalBrush.Reset();
    m_bevelBrush.Reset();
    m_baseTintBrush.Reset();
}

void GeekGlassEngine::CreateOrUpdateBrushes(ID2D1RenderTarget* pRT, const GeekGlassConfig& config) {
    float width = config.panelBounds.right - config.panelBounds.left;
    float height = config.panelBounds.bottom - config.panelBounds.top;
    float currentWidth = m_currentBounds.right - m_currentBounds.left;
    float currentHeight = m_currentBounds.bottom - m_currentBounds.top;

    bool sizeChanged = (std::abs(width - currentWidth) > 0.001f) || (std::abs(height - currentHeight) > 0.001f);
    bool themeChanged = (config.theme != m_currentTheme) || (config.tintProfile != m_currentTintProfile);
    // Detect material parameter changes (tintAlpha/specularOpacity driven by sliders)
    bool materialChanged = (std::abs(config.tintAlpha - m_currentTintAlpha) > 0.001f) ||
                           (std::abs(config.specularOpacity - m_currentSpecularOpacity) > 0.001f);
    if (config.tintProfile == 1 && (
        config.customTintColor.r != m_currentCustomTintColor.r || 
        config.customTintColor.g != m_currentCustomTintColor.g || 
        config.customTintColor.b != m_currentCustomTintColor.b)) {
        themeChanged = true;
    }
    bool needsRebuild = !m_diagonalBrush || !m_bevelBrush || !m_baseTintBrush || themeChanged || sizeChanged || materialChanged;

    // Check if the brushes need to be rebuilt: only if resized or theme changes
    if (!needsRebuild) {
        // Safety Lock 1: Only translation changed, trivially update proxy points without recreating D2D Stops
        if (config.panelBounds.left != m_currentBounds.left || config.panelBounds.top != m_currentBounds.top) {
             m_diagonalBrush->SetStartPoint(D2D1::Point2F(config.panelBounds.left, config.panelBounds.top));
             m_diagonalBrush->SetEndPoint(D2D1::Point2F(config.panelBounds.right, config.panelBounds.bottom));
             
             m_bevelBrush->SetStartPoint(D2D1::Point2F(config.panelBounds.left, config.panelBounds.top));
             m_bevelBrush->SetEndPoint(D2D1::Point2F(config.panelBounds.right, config.panelBounds.bottom));
        }
        m_currentBounds = config.panelBounds;
        return; 
    }

    m_currentTheme = config.theme;
    m_currentTintProfile = config.tintProfile;
    m_currentCustomTintColor = config.customTintColor;
    m_currentTintAlpha = config.tintAlpha;
    m_currentSpecularOpacity = config.specularOpacity;
    m_currentBounds = config.panelBounds;

    // --- Effective tint alpha (Respect 0% if user requested it) ---
    float effectiveTintAlpha = config.tintAlpha;

    // --- Base Solid Tint (Providing contrast floor) ---
    if (config.tintProfile == 1) { // Custom
        D2D1_COLOR_F tint = config.customTintColor;
        tint.a = effectiveTintAlpha;
        pRT->CreateSolidColorBrush(tint, &m_baseTintBrush);
    } else { // Auto
        if (config.theme == ThemeMode::Dark) {
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.08f, 0.08f, 0.09f, effectiveTintAlpha), &m_baseTintBrush);
        } else {
            pRT->CreateSolidColorBrush(D2D1::ColorF(0.94f, 0.94f, 0.96f, effectiveTintAlpha), &m_baseTintBrush);
        }
    }

    // --- Diagonal Gradient Filling (Simulating Glass Light Wrap) ---
    // Use config.specularOpacity instead of hardcoded 0.15f
    D2D1_POINT_2F diagStart = D2D1::Point2F(config.panelBounds.left, config.panelBounds.top);
    D2D1_POINT_2F diagEnd   = D2D1::Point2F(config.panelBounds.right, config.panelBounds.bottom);

    ComPtr<ID2D1GradientStopCollection> pDiagStops;
    
    D2D1_GRADIENT_STOP stops[] = {
        { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, config.specularOpacity) }, // Top-Left: Light reflection 
        { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f) }  // Bottom-Right: Fade to clear
    };
    pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pDiagStops);

    if (pDiagStops) {
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(diagStart, diagEnd),
            pDiagStops.Get(),
            &m_diagonalBrush
        );
    }

    // --- 1px Bevel Edge (Simulating 3D physical edge highlights) ---
    D2D1_POINT_2F bevelStart = D2D1::Point2F(config.panelBounds.left, config.panelBounds.top);
    D2D1_POINT_2F bevelEnd   = D2D1::Point2F(config.panelBounds.left, config.panelBounds.bottom);

    ComPtr<ID2D1GradientStopCollection> pBevelStops;
    if (config.theme == ThemeMode::Dark) {
        D2D1_GRADIENT_STOP stops[] = {
            { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f) }, // Top edge highlight
            { 0.2f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.05f) }, // Fast falloff
            { 1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f) }  // Bottom edge contour
        };
        pRT->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pBevelStops);
    } else {
        D2D1_GRADIENT_STOP stops[] = {
            { 0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.80f) }, // Top edge sharp white
            { 0.2f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.30f) }, // Slower falloff in light mode
            { 1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f) }  // Bottom faint shadow
        };
        pRT->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pBevelStops);
    }

    if (pBevelStops) {
        pRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(bevelStart, bevelEnd),
            pBevelStops.Get(),
            &m_bevelBrush
        );
    }
}

void GeekGlassEngine::DrawGeekGlassPanel(ID2D1RenderTarget* pRT, const GeekGlassConfig& config) {
    if (!pRT) return;

    // 0. Effects require DeviceContext
    ComPtr<ID2D1DeviceContext> pContext;
    pRT->QueryInterface(IID_PPV_ARGS(&pContext));

    // 1. Prepare panel geometry
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(config.panelBounds, config.cornerRadius, config.cornerRadius);

    // Create the rounded rectangle geometry used for clipping
    ComPtr<ID2D1Factory> factory;
    pRT->GetFactory(&factory);
    ComPtr<ID2D1RoundedRectangleGeometry> roundedGeometry;
    if (FAILED(factory->CreateRoundedRectangleGeometry(&roundedRect, &roundedGeometry))) {
        return;
    }

    // 2. Control master opacity completely in hardware through Layer Parameters
    D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters(
        config.panelBounds,
        roundedGeometry.Get(),
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
        D2D1::IdentityMatrix(),
        config.opacity, // Allows unified crossfading of entire effect system
        nullptr,
        D2D1_LAYER_OPTIONS_NONE
    );

    pRT->PushLayer(layerParams, nullptr);

    // 3. Track execution
    if (pContext && config.enableGeekGlass && config.track == RenderTrack::TrackA_CommandList && config.pBackgroundCommandList && m_blurEffect && m_cropEffect && m_transformEffect) {
        
        // Feed the command list into transform effect to map DComp coordinates to D2D screen space
        m_transformEffect->SetInput(0, config.pBackgroundCommandList);
        m_transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
        m_transformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, config.backgroundTransform);

        // Feed transformed output into blur algorithm
        m_blurEffect->SetInputEffect(0, m_transformEffect.Get());
        m_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, config.blurStandardDeviation);

        // Optimal approach: Chain a crop effect to avoid blurring pixels outside our rect
        m_cropEffect->SetInputEffect(0, m_blurEffect.Get());
        
        // Expand the crop region to account for blur spreading.
        D2D1_RECT_F cropRect = config.panelBounds;
        cropRect.left -= config.blurStandardDeviation * 3.0f;
        cropRect.top -= config.blurStandardDeviation * 3.0f;
        cropRect.right += config.blurStandardDeviation * 3.0f;
        cropRect.bottom += config.blurStandardDeviation * 3.0f;
        
        D2D1_VECTOR_4F cropVec = { cropRect.left, cropRect.top, cropRect.right, cropRect.bottom };
        m_cropEffect->SetValue(D2D1_CROP_PROP_RECT, cropVec);

        // Render the processed background into the current layer
        pContext->DrawImage(m_cropEffect.Get());
    }
    // Track B (DWM): System provides blur via Acrylic, we skip D2D blur pipeline entirely.
    // Tint, specular, and bevel still render below.

    // 4. Update Brushes
    CreateOrUpdateBrushes(pRT, config);

    // Render Base Tint Fill
    if (m_baseTintBrush) {
        pRT->FillRoundedRectangle(&roundedRect, m_baseTintBrush.Get());
    }

    // Render Diagonal Specular Highlight Fill (with smart luminance-based suppression)
    if (m_diagonalBrush) {
        // Smart Specular Suppression: Linear Remap in [0.05, 0.15] luminance range.
        // When background is extremely dark, the specular highlight naturally fades
        // to prevent the "grey haze" artifact on pure black backgrounds.
        float specularScale = 1.0f;
        if (config.backgroundLuminance >= 0.0f) {
            // L in [0.0, 0.05) → fully suppressed (0.0)
            // L in [0.05, 0.15] → linear ramp from 0.0 to 1.0
            // L > 0.15         → full intensity (1.0)
            constexpr float kLumLow = 0.05f;
            constexpr float kLumHigh = 0.15f;
            if (config.backgroundLuminance < kLumLow) {
                specularScale = 0.0f;
            } else if (config.backgroundLuminance < kLumHigh) {
                specularScale = (config.backgroundLuminance - kLumLow) / (kLumHigh - kLumLow);
            }
        }

        if (specularScale > 0.001f) {
            m_diagonalBrush->SetOpacity(specularScale);
            pRT->FillRoundedRectangle(&roundedRect, m_diagonalBrush.Get());
            m_diagonalBrush->SetOpacity(1.0f); // Reset for next frame
        }
    }

    // Render 1px Bevel Edge
    if (m_bevelBrush) {
        // D2D draws centered on the outline. Inset by 0.5f to get a crisp internal 1px line.
        D2D1_ROUNDED_RECT strokeRect = roundedRect;
        strokeRect.rect.left += 0.5f;   strokeRect.rect.top += 0.5f;
        strokeRect.rect.right -= 0.5f;  strokeRect.rect.bottom -= 0.5f;
        pRT->DrawRoundedRectangle(&strokeRect, m_bevelBrush.Get(), 1.0f);
    }

    // 5. Unclip
    pRT->PopLayer();
}

GeekGlassConfig GetGlobalThemeConfig() {
    GeekGlassConfig config;
    config.enableGeekGlass = g_config.EnableGeekGlass;
    config.theme = IsLightThemeActive() ? ThemeMode::Light : ThemeMode::Dark;
    
    config.blurStandardDeviation = g_config.GlassBlurSigma;
    config.tintAlpha = g_config.GlassTintAlpha;
    config.specularOpacity = g_config.GlassSpecularOpacity;
    
    // GlassModalsOpacity is 0-100 percentage
    config.opacity = g_config.GlassModalsOpacity / 100.0f;

    config.tintProfile = g_config.GlassTintProfile;
    config.customTintColor = D2D1::ColorF(
        g_config.GlassCustomTintR,
        g_config.GlassCustomTintG,
        g_config.GlassCustomTintB
    );
    
    // Corner radius synced with global setting
    config.cornerRadius = 8.0f; // Default for context menus
    
    // Use Track B for context menus by default as they use UpdateLayeredWindow
    config.track = RenderTrack::TrackB_DWM;

    return config;
}

} // namespace QuickView::UI::GeekGlass
