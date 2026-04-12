#pragma once
// ============================================================
// GeekIconLibrary.h - Windows System Icon Font Mapping
// ============================================================
// Transitioned from self-drawn vectors to Segoe Fluent Icons /
// Segoe MDL2 Assets for modern Win10/11 system consistency.
// ============================================================

#include <d2d1_1.h>

// Handle as constant strings for DWrite rendering
using IconGlyph = const wchar_t*;

namespace GeekIcons {

    // --- File Operations ---
    static constexpr IconGlyph Open        = L"\uE8E5"; // OpenWith
    static constexpr IconGlyph Rename      = L"\uE8AC"; // Rename
    static constexpr IconGlyph Edit        = L"\uE70F"; // Edit
    static constexpr IconGlyph Delete      = L"\uE74D"; // Delete
    static constexpr IconGlyph OpenWith    = L"\uE7AC"; // OpenWith (alternative)
    static constexpr IconGlyph Copy        = L"\uE8C8"; // Copy
    static constexpr IconGlyph Explorer    = L"\uEC50"; // File Explorer
    static constexpr IconGlyph Folder      = L"\uE838"; // Folder
    static constexpr IconGlyph Link        = L"\uE71B"; // Link (Copy Path)
    static constexpr IconGlyph Print       = L"\uE749"; // Print
    static constexpr IconGlyph FixExt      = L"\uEBE7"; // Repair / Fix

    // --- View & Display ---
    static constexpr IconGlyph Eye         = L"\uE7B3"; // View
    static constexpr IconGlyph Info        = L"\uE946"; // Info
    static constexpr IconGlyph Compare     = L"\uE114"; // Compare (Side-by-side)
    static constexpr IconGlyph Wallpaper   = L"\uE91B"; // Desktop
    
    // --- Transform ---
    static constexpr IconGlyph Transform   = L"\uE7AD"; // Rotate

    // --- Color & Proofing ---
    static constexpr IconGlyph Color       = L"\uE790"; // Color
    static constexpr IconGlyph SoftProof   = L"\uE7EE"; // Library / Proofing

    // --- Sort & Navigation ---
    static constexpr IconGlyph Sort        = L"\uE8CB"; // Sort
    static constexpr IconGlyph Navigation  = L"\uEBC6"; // Orientation / Nav

    // --- Application ---
    static constexpr IconGlyph Settings    = L"\uE713"; // Settings
    static constexpr IconGlyph About       = L"\uE897"; // Contact / About
    static constexpr IconGlyph Exit        = L"\uE711"; // Close / Exit

    // --- UI Glyphs ---
    static constexpr IconGlyph Chevron     = L"\uE76C"; // ChevronRight
    static constexpr IconGlyph Check       = L"\uE73E"; // CheckMark

} // namespace GeekIcons
