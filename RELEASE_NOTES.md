# QuickView v4.0.0 - The Titan Engine Update
**Release Date**: 2026-03-06

This is a massive milestone release focusing on the introduction of the **Titan Architecture**, full **JPEG XL** integration, and **Gigapixel Image Handling**.

### 🚀 Architecture: "Titan System"
-   **Gigapixel Tiling**: The new Titan Pipeline dynamically slices massive image datasets into manageable LOD (Level of Detail) tiles, enabling smooth 60fps panning over images that previously caused OOM (Out of Memory) crashes.
-   **Smart Pull & Prefetch**: Memory is now intelligently streamed. QuickView only decodes and uploads the tiles currently visible on your monitor, predicting panning direction to prefetch adjacent chunks.
-   **Direct-to-MMF Decode**: Zero-copy Memory-Mapped File strategy to pipeline gigantic source images directly to the render composition engine.

### ✨ Format Enhancements
-   **Native JPEG XL (JXL)**: Full, hyper-optimized support for the next-generation JXL format, backed by our parallel HeavyLane workers.
-   **Pro Design Formats**: We now support Photoshop's massive document format (PSB) and can instantly extract embedded previews from both PSDs and PSBs.
-   **Shell-Accelerated Gallery**: The `T` Gallery now taps directly into the Windows Explorer Thumbnail Cache, making initial indexing of massive folders completely instantaneous.

### 💎 UX & Scaling
-   **True PerMonitorV2 High-DPI**: The interface has been untethered from Windows' legacy scaling. We now support explicit native UI scaling with granular manual overrides (100%-250%).
-   **Always Fullscreen**: A much-requested feature—you can now command QuickView to automatically launch images in exclusive Fullscreen mode (`Off`, `Large Only`, `All`).
-   **Wheel Action Customization**: Unified and centralized logic for mouse-wheel actions with a new `WheelActionMode` for absolute control.

### 🛠 Visual Precision
-   **AVX-512 SIMD Resizing**: Critical bilinear scaling paths have been unrolled using the latest AVX2/AVX-512 instruction sets for blazing-fast zooming.
-   **Center-to-Center Topology**: Our internal rendering coordinate system was mathematically refactored, permanently eliminating "edge smearing" and pixel gaps between Titan tiles.
-   **Focus Robustness**: Implemented low-level `AttachThreadInput` logic to guarantee new QuickView windows forcefully seize focus when you open new files from Explorer.

---