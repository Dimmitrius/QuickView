# QuickView v3.2.5 - Precision & Expansion Update
**Release Date**: 2026-01-26

This is a massive update focusing on **Precision UI**, **Multi-Monitor Capabilities**, and **Core robust-ness**.

### ✨ New Features
-   **Span Displays (Video Wall)**: New "Span Across Monitors" feature (`Ctrl+F11`) allows the window to instantly stretch across all available displays, perfect for video walls or ultra-wide viewing.
-   **Modern D2D Rename Dialog**: Replaced the legacy Windows GDI rename dialog with a fully hardware-accelerated Direct2D version that matches the application's dark theme.
-   **Help Overlay**: Added a built-in keyboard shortcut guide (`F1`).
-   **Window Rounded Corners**: New toggle in Settings -> View to enable/disable Windows 11-style rounded window corners.
-   **AVX2 Detection**: Added start-up check for AVX2 instruction set support to ensure compatibility with optimized decoders.

### 💎 Precision UI & Typography
-   **Binary Search Text Truncation**: Completely rewrote the text shortening engine. Using a binary search algorithm, filenames in dialogs and the Info Panel now use *every available pixel*, eliminating wasted space and "..." artifacts for long filenames (especially CJK).
-   **Pixel-Perfect 100% Zoom**: Fixed scaling logic to ensure images of ALL sizes (from 16px icons to 50MP RAWs) render crisp and accurate at 100% zoom.
-   **Smart Double-Click**: Double-clicking now rationally adapts to context:
    -   **Fullscreen**: Exits Fullscreen.
    -   **Windowed**: Toggles between "Fit to Window" and "100% View".
-   **Info Panel Zoom**: The Info Panel now displays the current Zoom percentage.
-   **Zoom Snap Damping**: Added configurable damping mechanics for "Magnetic Zoom", making it feel more organic.
-   **Robust Auto-Hide**: Optimized toolbars and navigation arrows to hide reliably (100% success rate) even when the mouse exits the window rapidly.

### 🛠 Core Architecture
-   **Unified Window Controls**: Moved all window control rendering (Minimize/Maximize/Close) into the `UIRenderer` pipeline for consistent styling and correct behavior in Fullscreen/Maximized modes.
-   **Unified Zoom Logic**: Decoupled zoom logic from window resizing, fixing "fighting" issues between user input and auto-fit logic.
-   **Portable Mode Polish**: Improved logic for switching between Portable and Install modes, ensuring cleaner registry handling.

### 🐛 Bug Fixes
-   **Scaling Anomalies**: Fixed visual glitches when high-DPI scaling was active.
-   **Gallery Tooltips**: Fixed issue where image dimensions would show as 0x0 in gallery tooltips.
-   **RAW Toggle**: Fixed state persistence issue where the "Force RAW Decode" setting wasn't saving correctly.
-   **Cursor Visibility**: Fixed Navigation Arrows remaining visible even when the cursor was set to hidden.
-   **Settings UI**: Fixed layout displacement of the "Back" arrow in the General settings tab.

---