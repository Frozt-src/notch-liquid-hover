// ==WindhawkMod==
// @id              notch-liquid-hover
// @name            Notch Liquid Hover
// @description     Pulls a soft bulge of the NotchDock's body up under the pointer, like a liquid surface
// @version         0.5.0
// @author          frozt
// @github          https://github.com/frozt-src
// @license         Apache-2.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notch Liquid Hover

A liquid hover effect for the **NotchDock** taskbar theme. Point at an icon and the notch's top edge
swells up under it like the surface of a liquid, glides with the pointer from icon to icon, and sinks
back with a soft spring when the pointer leaves. At either end of the row the notch's curved slope
lifts with it. The notch around the clock and system icons does the same.

**For the best experience, install this mod alongside the NotchDock theme for
[Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler).** The theme
works on its own; this mod is what makes its notches move.

## Requirements

- [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler) with the
  NotchDock theme applied. Without the theme this mod does nothing: it checks for the NotchDock
  notch shape and stays idle on any other taskbar, so it is safe to leave enabled.
- A taskbar at the bottom of the screen.
- Optional: [Taskbar Dock Animation](https://windhawk.net/mods/taskbar-dock-animation) for icon
  magnification; the two work together.

## Clicks go through

The bulge rises a few pixels above the taskbar. Where a maximised window reaches down to the
taskbar, that part of the bulge is drawn in a thin overlay above the window. The overlay is shown only
while the bulge is up, and the mouse does not see it: clicks land on the window underneath.

## Settings

- **Dock notch** / **Tray notch**: turn the effect on or off for each notch.
- **Height**: how far the surface may rise, in pixels (2 to 24, default 8).
- **Dock bulge width** / **Tray bulge width**: how wide the raised stretch is (defaults 168 and 120).
- **Colour**: the notch's colour as `#RRGGBB` (default `#000000`, NotchDock's black). If you change
  the theme's notch colour, set the same colour here.

## Known limitations

- Built for NotchDock's notch shape only; other themes are ignored on purpose.
- The tray's bulge starts working after the dock has been hovered once since Explorer started.
- Tested on Windows 11 build 26300 with one taskbar. Taskbars on secondary monitors, top or side
  taskbars and other builds are untested.

## How it works

It hooks `TaskbarFrame::OnPointerMoved` in Taskbar.View.dll, as Taskbar Dock Animation does, only to
learn that the pointer has arrived and where the taskbar is. From then on a per-frame callback
follows the real cursor, because pointer events stop once a window preview opens. Each bulge is one
XAML `Polygon` redrawn every frame, plus the click-through overlay window described above. Everything
the mod adds is removed when it is disabled or unloaded.

## License

Copyright 2026 [frozt](https://github.com/frozt-src). Licensed under the
[Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0). If you distribute this mod or a
modified version, keep its copyright and attribution notices and mark the files you changed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dock: true
  $name: Dock notch
  $description: Raise the surface under the pointer on the taskbar's main notch
- tray: true
  $name: Tray notch
  $description: Raise the surface under the pointer on the notch around the clock and system icons
- height: 8
  $name: Height
  $description: Room above the notch that the surface may rise into, in pixels (2 to 24). It settles at about 80% of this
- dockWidth: 168
  $name: Dock bulge width
  $description: Width of the raised stretch on the dock notch, in pixels (60 to 400)
- trayWidth: 120
  $name: Tray bulge width
  $description: Width of the raised stretch on the tray notch, in pixels (40 to 300)
- color: "#000000"
  $name: Colour
  $description: The notch's colour as #RRGGBB. It must match the theme's notch, or the bulge shows as a patch
*/
// ==/WindhawkModSettings==

// Notch Liquid Hover
// Copyright 2026 frozt (https://github.com/frozt-src)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Third-party notice: the Taskbar.View.dll hook setup and RunFromWindowThread follow the pattern used by
// Windhawk catalog mods, including Taskbar Dock Animation by Ph0en1x-dev and Windows 11 Taskbar Styler by
// m417z. Those mods are distributed under the MIT License:
//
// Copyright (c) Ph0en1x-dev
// Copyright (c) m417z
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace {

constexpr wchar_t kDockBulgeName[] = L"NotchLiquidBulge";
constexpr wchar_t kTrayBulgeName[] = L"NotchLiquidTrayBulge";
constexpr double kDockBulgeWidth = 168.0;  // px; the stretch of surface that lifts, about three 44 px buttons plus tails
constexpr double kTrayBulgeWidth = 120.0;  // px; the tray's icons are narrower and its notch is short
// The user's settings. Windhawk reports changes on its own thread while the taskbar's UI thread reads
// them every frame, hence atomics. Defaults equal the settings block's.
struct Settings {
    std::atomic<bool> dock{true};
    std::atomic<bool> tray{true};
    std::atomic<double> liftRoom{8.0};       // px above the notch's flat top that a bulge may reach into
    std::atomic<double> dockWidth{kDockBulgeWidth};
    std::atomic<double> trayWidth{kTrayBulgeWidth};
    std::atomic<uint32_t> colour{0xFF000000u};   // 0xAARRGGBB, always opaque
};
Settings g_settings;
constexpr double kLiftShare = 0.8;       // of that room; the rest is for the spring's overshoot
constexpr double kSeamOverlap = 1.5;     // px the outline reaches below the surface, hiding the join
constexpr double kFollowTime = 0.05;     // s; the crest closes 63% of its distance to its target in this time
constexpr double kSpringHz = 3.4;        // how briskly the surface rises and settles
constexpr double kSpringDamping = 0.5;   // below 1 it overshoots once, as a liquid surface would
constexpr double kStep = 2.0;            // px between outline points
constexpr double kPi = 3.14159265358979;

// The notch's slope as tools/make_notch_caps.py draws it; these must stay equal to that script's
// WIDTH_RATIO, CONTROL_X and START. kDockInsetRatio is the theme's body margin (dockH*0.3125): the
// dock's flat top starts that far inside the row of buttons.
constexpr double kSlopeWidthRatio = 2.5;
constexpr double kDockInsetRatio = 0.3125;
constexpr std::array<double, 6> kSlopeControlX = {0.0, 0.5492, 0.6484, 0.4716, 0.98, 1.0};
constexpr double kSlopeStart = 0.2370;

// Diagnostics for a mod that cannot be stepped through: when on, the first kDebugLines events are
// appended to %TEMP%\notch-liquid-hover.log. Keep it off in a build meant to stay installed.
constexpr bool kDebugLog = false;
constexpr int kDebugLines = 120;

void DebugLog(const wchar_t* format, ...) {
    static int remaining = kDebugLines;
    if (!kDebugLog || remaining <= 0) {
        return;
    }
    --remaining;
    wchar_t temp[MAX_PATH];
    const DWORD length = GetTempPathW(MAX_PATH, temp);
    if (length == 0 || length >= MAX_PATH) {
        return;
    }
    const std::wstring path = std::wstring(temp, length) + L"notch-liquid-hover.log";
    std::unique_ptr<FILE, int (*)(FILE*)> file(_wfopen(path.c_str(), L"a"), &fclose);
    if (!file) {
        return;
    }
    va_list args;
    va_start(args, format);
    vfwprintf(file.get(), format, args);
    va_end(args);
    fputwc(L'\n', file.get());
}

// ---------------------------------------------------------------------------------------------
// The notch's shape
// ---------------------------------------------------------------------------------------------

// Height of the notch's plate (0..1) at fraction u along a slope, from its tip at the screen's edge
// (0) to where it joins the flat top (1). The same curve as plate_height() in make_notch_caps.py;
// checked against notch-mask-48.png to 0.01 px.
double PlateHeight(double u) {
    static const auto curve = [] {
        std::array<std::pair<double, double>, 257> points{};
        for (size_t i = 0; i < points.size(); ++i) {
            const double t = static_cast<double>(i) / static_cast<double>(points.size() - 1);
            const double s = 1.0 - t;
            const double basis[6] = {s * s * s * s * s,      5 * s * s * s * s * t, 10 * s * s * s * t * t,
                                     10 * s * s * t * t * t, 5 * s * t * t * t * t, t * t * t * t * t};
            double x = 0;
            for (int k = 0; k < 6; ++k) {
                x += basis[k] * kSlopeControlX[static_cast<size_t>(k)];
            }
            points[i] = {x, 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t};
        }
        return points;
    }();
    const auto heightAt = [](double x) {
        const auto upper = std::lower_bound(curve.begin(), curve.end(), x,
                                            [](std::pair<double, double> const& p, double v) { return p.first < v; });
        if (upper == curve.begin()) {
            return upper->second;
        }
        if (upper == curve.end()) {
            return curve.back().second;
        }
        const auto lower = upper - 1;
        const double span = upper->first - lower->first;
        return span > 0 ? lower->second + (upper->second - lower->second) * (x - lower->first) / span : upper->second;
    };
    const double start = heightAt(kSlopeStart);
    return (heightAt(kSlopeStart + std::clamp(u, 0.0, 1.0) * (1.0 - kSlopeStart)) - start) / (1.0 - start);
}

// A notch's bounding box in its outline's coordinates: slope, flat top, slope. `top` is the flat top.
struct Box {
    double left = 0.0;
    double top = 0.0;
    double width = 0.0;
    double height = 0.0;
    double slope = 0.0;      // width of each slope
    double clip = 0.0;       // nothing of the notch shows above this line; normally equal to top
};

// How far the notch's top edge lies below its flat top at x from the box's left edge: 0 on the flat
// part, growing down either slope.
double SurfaceDrop(double x, Box const& box) {
    const double outside = std::max(box.slope - x, x - (box.width - box.slope));
    if (outside <= 0) {
        return 0.0;
    }
    return box.height * (1.0 - PlateHeight(1.0 - outside / box.slope));
}

// ---------------------------------------------------------------------------------------------
// A surface: one notch's bulge
// ---------------------------------------------------------------------------------------------

struct Surface {
    const wchar_t* name;       // of its canvas, so a stale one can be found and removed
    double bulgeWidth;
    uint32_t fill = 0xFF000000u;   // the outline's colour as last set, 0xAARRGGBB
    double crest = -1.0;       // where the crest is now, from the box's left edge; negative = not placed yet
    double lift = 0.0;         // how far the surface is raised: 0 flat, 1 fully, a little more on the overshoot
    double velocity = 0.0;     // of lift, per second
    // What the last frame drew, for the overlay: the crest's rise in px (0 = flat), the notch's box,
    // and where the outline's coordinates start inside the taskbar window.
    double rise = 0.0;
    Box box{};
    double originX = 0.0;
    double originY = 0.0;
};
Surface g_dock{kDockBulgeName, kDockBulgeWidth};
Surface g_tray{kTrayBulgeName, kTrayBulgeWidth};

// Canvases this mod added, for removal on unload. Touched only on the taskbar's UI thread, as is
// everything else below unless it says otherwise.
std::vector<winrt::weak_ref<Controls::Canvas>> g_canvases;

Shapes::Polygon FindOutline(Controls::Panel const& host, const wchar_t* name) {
    for (auto const& child : host.Children()) {
        auto canvas = child.try_as<Controls::Canvas>();
        if (canvas && canvas.Name() == name && canvas.Children().Size() > 0) {
            return canvas.Children().GetAt(0).try_as<Shapes::Polygon>();
        }
    }
    return nullptr;
}

void RemoveCanvas(Controls::Panel const& host, const wchar_t* name) {
    auto children = host.Children();
    for (uint32_t i = 0; i < children.Size(); ++i) {
        auto element = children.GetAt(i).try_as<FrameworkElement>();
        if (element && element.Name() == name) {
            children.RemoveAt(i);
            return;
        }
    }
}

// Adds a zero-size canvas holding one black polygon at the host's top-left corner (inside its
// padding). A canvas does not clip, so the outline may run up into the headroom and out over the
// slopes. `index` sets how far back it sits among the host's children.
Shapes::Polygon AddOutline(Controls::Panel const& host, const wchar_t* name, uint32_t index) {
    Controls::Canvas canvas;
    Shapes::Polygon outline;
    outline.Fill(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 0, 0, 0}));
    canvas.Children().Append(outline);
    canvas.Name(name);
    canvas.Width(0);
    canvas.Height(0);
    canvas.HorizontalAlignment(HorizontalAlignment::Left);
    canvas.VerticalAlignment(VerticalAlignment::Top);
    canvas.IsHitTestVisible(false);
    Controls::Grid::SetColumnSpan(canvas, 16);
    auto children = host.Children();
    children.InsertAt(std::min(index, children.Size()), canvas);
    g_canvases.push_back(winrt::make_weak(canvas));
    return outline;
}

// One frame of one surface: move the crest part of the way to its target (the lag is what makes it
// read as liquid), advance the spring that raises or lowers it, and redraw the outline. Returns
// false once it is flat and at rest.
bool Advance(Surface& surface, Shapes::Polygon const& outline, bool over, double target, Box const& box, double dt) {
    if (over) {
        const bool visible = surface.lift > 0.02 && surface.crest >= 0;
        surface.crest = visible ? surface.crest + (target - surface.crest) * (1.0 - std::exp(-dt / kFollowTime))
                                : target;            // flat: nothing to see, so jump
    }

    // A damped spring pulls the lift toward 1 while hovered and toward 0 otherwise.
    const double omega = 2.0 * kPi * kSpringHz;
    const double goal = over ? 1.0 : 0.0;
    surface.velocity += (omega * omega * (goal - surface.lift) - 2.0 * kSpringDamping * omega * surface.velocity) * dt;
    surface.lift += surface.velocity * dt;
    if (surface.lift < 0) {                          // it comes to rest on the surface, never below it
        surface.lift = 0;
        surface.velocity = 0;
    }
    if (!over && surface.lift < 0.004 && std::abs(surface.velocity) < 0.05) {
        surface.lift = 0;
        surface.velocity = 0;
        surface.rise = 0;
        if (outline.Points().Size() > 0) {
            outline.Points().Clear();
        }
        return false;
    }

    // The outline is the notch's own top edge, raised by a bell around the crest. Over a slope the
    // edge it raises is the slope itself, so near an end the pull carries on down the notch's
    // curve; the lower edge runs just under the surface, where it is black on black. Whatever of it
    // lies above the taskbar window is not drawn by XAML; the overlay draws that part.
    const double rise = g_settings.liftRoom * kLiftShare * surface.lift;
    if (const uint32_t colour = g_settings.colour; colour != surface.fill) {
        outline.Fill(Media::SolidColorBrush(winrt::Windows::UI::Color{
            255, static_cast<uint8_t>(colour >> 16), static_cast<uint8_t>(colour >> 8), static_cast<uint8_t>(colour)}));
        surface.fill = colour;
    }
    surface.rise = rise;
    surface.box = box;
    const double half = surface.bulgeWidth / 2;
    Media::PointCollection points;
    std::vector<winrt::Windows::Foundation::Point> under;
    for (double d = -half; d <= half + 0.001; d += kStep) {
        const double at = surface.crest + d;
        const double edge = std::max(box.clip, box.top + SurfaceDrop(at, box));   // the visible top edge here
        const double bell = 0.5 * (1.0 + std::cos(kPi * d / half));
        const auto px = static_cast<float>(box.left + at);
        points.Append({px, static_cast<float>(edge - rise * bell)});
        under.push_back({px, static_cast<float>(edge + kSeamOverlap)});
    }
    for (auto it = under.rbegin(); it != under.rend(); ++it) {
        points.Append(*it);
    }
    outline.Points(points);
    return true;
}

// ---------------------------------------------------------------------------------------------
// Where things are
// ---------------------------------------------------------------------------------------------

struct Tracking {
    winrt::weak_ref<Controls::Grid> dockGrid;    // Taskbar.TaskbarFrame > Grid#RootGrid
    winrt::weak_ref<Controls::Panel> root;       // the island's root panel, parent of both frames
    winrt::weak_ref<FrameworkElement> trayGrid;  // #SystemTrayFrameGrid, whose box is the tray notch
    winrt::weak_ref<UIElement> trayFrame;        // carries the pointer handler
    double scale = 1.0;      // screen pixels per layout pixel
    double windowX = 0.0;    // the taskbar window's corner on screen, in layout px
    double windowY = 0.0;
    RECT windowRect{};       // the taskbar window on screen, in screen px
    double dockX = 0.0;      // the dock grid's corner inside the window
    double dockY = 0.0;
};
Tracking g_tracking;
winrt::Windows::Foundation::IInspectable g_trayHandler{nullptr};

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_hooksApplied{false};

// ---------------------------------------------------------------------------------------------
// The overlay: the part of a bulge above the taskbar window
// ---------------------------------------------------------------------------------------------

// The taskbar cannot draw outside its own window. This is a strip window lying just above it, over
// whatever application is there, in which that part of a bulge is drawn. It is topmost, shown only
// while something is drawn in it, and layered with WS_EX_TRANSPARENT, so the mouse does not see it:
// every click reaches the window underneath. It uses the system's "Static" class, so no window
// procedure of this mod outlives the mod. Created, drawn and destroyed on the taskbar's UI thread.
class Overlay {
public:
    Overlay() = default;
    Overlay(Overlay const&) = delete;
    Overlay& operator=(Overlay const&) = delete;
    ~Overlay() { Destroy(); }

    // Shows `heights`, the px of black in each column counted up from the strip's bottom edge, in a
    // strip `rows` px tall whose bottom-left corner is at left,bottom on screen. Hides the strip if
    // every column is empty.
    void Present(int left, int bottom, int rows, std::vector<float> const& heights, uint32_t colour) {
        // A layered window wants each colour channel already multiplied by the pixel's alpha.
        const auto pixel = [colour](uint32_t alpha) {
            const uint32_t r = ((colour >> 16) & 0xFF) * alpha / 255;
            const uint32_t g = ((colour >> 8) & 0xFF) * alpha / 255;
            const uint32_t b = (colour & 0xFF) * alpha / 255;
            return alpha << 24 | r << 16 | g << 8 | b;
        };
        const int width = static_cast<int>(heights.size());
        if (width <= 0 || rows <= 0 ||
            std::none_of(heights.begin(), heights.end(), [](float h) { return h > 0.0f; }) || !Resize(width, rows)) {
            Hide();
            return;
        }
        std::fill_n(pixels_, static_cast<size_t>(width) * static_cast<size_t>(rows), 0u);
        for (int x = 0; x < width; ++x) {
            const float height = std::min(heights[static_cast<size_t>(x)], static_cast<float>(rows));
            if (height <= 0.0f) {
                continue;
            }
            // Premultiplied black: only the alpha byte is set. The topmost pixel is partly covered.
            const int full = static_cast<int>(height);
            for (int k = 0; k < full; ++k) {
                pixels_[static_cast<size_t>(rows - 1 - k) * static_cast<size_t>(width) + static_cast<size_t>(x)] = pixel(255);
            }
            if (full < rows) {
                const auto alpha = static_cast<uint32_t>(std::lround((height - static_cast<float>(full)) * 255.0f));
                pixels_[static_cast<size_t>(rows - 1 - full) * static_cast<size_t>(width) + static_cast<size_t>(x)] = pixel(alpha);
            }
        }
        POINT destination{left, bottom - rows};
        SIZE size{width, rows};
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        if (!UpdateLayeredWindow(window_, nullptr, &destination, &size, dc_, &source, 0, &blend, ULW_ALPHA)) {
            Hide();
            return;
        }
        if (!shown_) {
            SetWindowPos(window_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            shown_ = true;
        }
    }

    void Hide() {
        if (window_ && shown_) {
            ShowWindow(window_, SW_HIDE);
        }
        shown_ = false;
    }

    void Destroy() {
        Hide();
        ReleaseBitmap();
        if (window_) {
            DestroyWindow(window_);
            window_ = nullptr;
        }
    }

private:
    bool Resize(int width, int rows) {
        if (!window_) {
            window_ = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, L"Static",
                nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
            if (!window_) {
                return false;
            }
        }
        if (bitmap_ && width == width_ && rows == rows_) {
            return true;
        }
        ReleaseBitmap();
        dc_ = CreateCompatibleDC(nullptr);
        if (!dc_) {
            return false;
        }
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -rows;                     // top-down
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bitmap_ || !bits) {
            ReleaseBitmap();
            return false;
        }
        previous_ = SelectObject(dc_, bitmap_);
        pixels_ = static_cast<uint32_t*>(bits);
        width_ = width;
        rows_ = rows;
        return true;
    }

    void ReleaseBitmap() {
        if (dc_ && previous_) {
            SelectObject(dc_, previous_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        if (dc_) {
            DeleteDC(dc_);
        }
        dc_ = nullptr;
        bitmap_ = nullptr;
        previous_ = nullptr;
        pixels_ = nullptr;
        width_ = 0;
        rows_ = 0;
    }

    HWND window_ = nullptr;
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ previous_ = nullptr;
    uint32_t* pixels_ = nullptr;     // the bitmap's own memory, top row first
    int width_ = 0;
    int rows_ = 0;
    bool shown_ = false;
};
Overlay g_overlay;

// Draws, for both surfaces, what stands above the taskbar window's top edge. The curve is the one
// Advance() gave the XAML outline, sampled once per screen column.
void UpdateOverlay() {
    const RECT& rect = g_tracking.windowRect;
    const double scale = g_tracking.scale;
    const int width = rect.right - rect.left;
    if (width <= 0 || scale <= 0) {
        g_overlay.Hide();
        return;
    }
    static std::vector<float> heights;
    heights.assign(static_cast<size_t>(width), 0.0f);
    for (Surface const* surface : {&g_dock, &g_tray}) {
        if (surface->rise <= 0 || surface->crest < 0) {
            continue;
        }
        const Box& box = surface->box;
        const double half = surface->bulgeWidth / 2;
        const double start = surface->originX + box.left + surface->crest - half;    // in the window, layout px
        const int first = std::max(0, static_cast<int>(std::floor(start * scale)));
        const int last = std::min(width - 1, static_cast<int>(std::ceil((start + 2 * half) * scale)));
        for (int column = first; column <= last; ++column) {
            const double at = (column + 0.5) / scale - surface->originX - box.left;
            const double d = at - surface->crest;
            if (std::abs(d) > half) {
                continue;
            }
            const double edge = std::max(box.clip, box.top + SurfaceDrop(at, box));
            const double bell = 0.5 * (1.0 + std::cos(kPi * d / half));
            const double above = -(surface->originY + edge - surface->rise * bell) * scale;   // px over the window's top
            auto& height = heights[static_cast<size_t>(column)];
            height = std::max(height, static_cast<float>(above));
        }
    }
    g_overlay.Present(rect.left, rect.top, static_cast<int>(std::ceil(g_settings.liftRoom * scale)), heights,
                      g_settings.colour);
}

// Following runs once per rendered frame, the same callback the Dock Animation mod animates from.
// A DispatcherTimer was tried first and never fired in Explorer (2026-11-12).
winrt::event_token g_renderingToken{};
bool g_following = false;
std::chrono::steady_clock::time_point g_lastTick;

void OnTick();

void StartFollowing() {
    if (g_following) {
        return;
    }
    g_lastTick = std::chrono::steady_clock::now();
    g_renderingToken = Media::CompositionTarget::Rendering([](auto const&, auto const&) {
        try {
            OnTick();
        } catch (winrt::hresult_error const& e) {
            DebugLog(L"tick: error 0x%08X %s", static_cast<unsigned>(e.code().value), e.message().c_str());
        } catch (...) {
            DebugLog(L"tick: unknown error");
        }
    });
    g_following = true;
}

void StopFollowing() {
    if (g_following) {
        Media::CompositionTarget::Rendering(g_renderingToken);
        g_following = false;
    }
}

// Records where the taskbar window is and its scale. The window's corner is used, not a pointer
// event's position: by the time an event is handled the cursor may have moved on, which put the
// bulge 35 px behind a fast pointer (measured 2026-11-12). False if the cursor is not on a taskbar.
bool Calibrate(FrameworkElement const& element) {
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        return false;
    }
    HWND window = GetAncestor(WindowFromPoint(cursor), GA_ROOT);
    wchar_t className[64] = {};
    RECT rect;
    if (!window || !GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (wcscmp(className, L"Shell_TrayWnd") != 0 && wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) ||
        !GetWindowRect(window, &rect)) {
        return false;
    }
    const auto root = element.XamlRoot();
    g_tracking.scale = root ? root.RasterizationScale() : 1.0;
    g_tracking.windowX = rect.left / g_tracking.scale;
    g_tracking.windowY = rect.top / g_tracking.scale;
    g_tracking.windowRect = rect;
    return true;
}

Controls::Grid FindRootGrid(FrameworkElement const& frame) {
    const int count = Media::VisualTreeHelper::GetChildrenCount(frame);
    for (int i = 0; i < count; ++i) {
        auto grid = Media::VisualTreeHelper::GetChild(frame, i).try_as<Controls::Grid>();
        if (grid && grid.Name() == L"RootGrid") {
            return grid;
        }
    }
    return nullptr;
}

// The element named `name` at most `depth` levels below `parent`.
FrameworkElement FindNamed(DependencyObject const& parent, const wchar_t* name, int depth) {
    const int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name) {
            return element;
        }
        if (depth > 1) {
            if (auto found = FindNamed(child, name, depth - 1)) {
                return found;
            }
        }
    }
    return nullptr;
}

// Centre, along the buttons' area, of the taskbar button under x; x itself if it is between buttons.
// Owner's direction 2026-11-12: the bulge belongs to the icon that comes up, not to the pointer.
double ButtonCentre(Controls::Grid const& grid, double x, double paddingLeft) {
    for (auto const& child : grid.Children()) {
        auto repeater = child.try_as<FrameworkElement>();
        if (!repeater || repeater.Name() != L"TaskbarFrameRepeater") {
            continue;
        }
        const int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
        for (int i = 0; i < count; ++i) {
            auto button = Media::VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
            if (!button || button.ActualWidth() <= 0) {
                continue;
            }
            // Layout position, so the Dock Animation mod's scaling of the button does not move it.
            const auto offset = button.ActualOffset();
            const double left = repeater.ActualOffset().x + offset.x - paddingLeft;
            if (x >= left && x < left + button.ActualWidth()) {
                return left + button.ActualWidth() / 2;
            }
        }
    }
    return x;
}

// The flat body of the dock's notch: Taskbar.TaskbarBackground > Grid, which the theme sizes and places.
FrameworkElement FindDockBody(Controls::Grid const& grid) {
    for (auto const& child : grid.Children()) {
        if (winrt::get_class_name(child) == L"Taskbar.TaskbarBackground" &&
            Media::VisualTreeHelper::GetChildrenCount(child) > 0) {
            return Media::VisualTreeHelper::GetChild(child, 0).try_as<FrameworkElement>();
        }
    }
    return nullptr;
}

// True where the NotchDock theme has shaped the dock: the body's left slope, `Rectangle#BackgroundFill`,
// is shown and hangs one slope width outside the body. On a taskbar without the theme its margin is 0,
// and the mod must draw nothing there.
bool HasDockNotch(FrameworkElement const& body, double slope) {
    auto panel = body.try_as<Controls::Panel>();
    if (!panel) {
        return false;
    }
    for (auto const& child : panel.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == L"BackgroundFill") {
            return element.Visibility() == Visibility::Visible && std::abs(element.Margin().Left + slope) <= 1.0;
        }
    }
    return false;
}

// The dock's frame of the animation. Its outline lives in the root grid at the corner of the
// buttons' area, which is level with the notch's flat top.
bool TickDock(double cursorX, double cursorY, double dt) {
    auto grid = g_tracking.dockGrid.get();
    auto outline = grid ? FindOutline(grid, g_dock.name) : nullptr;
    if (!outline) {
        g_dock.rise = 0;
        return false;
    }
    // The notch is wherever the theme put its flat body, with a slope hung on each side. The row of
    // buttons is no guide: since Windows moved the tray into that row (2026-11) it is wider than the dock.
    auto body = FindDockBody(grid);
    if (!body || body.ActualWidth() <= 0) {
        g_dock.rise = 0;
        return false;
    }
    const auto padding = grid.Padding();
    const double notchHeight = grid.ActualHeight() - padding.Top;
    const double inset = kDockInsetRatio * notchHeight;
    const double slope = kSlopeWidthRatio * notchHeight;
    const double bodyLeft = body.TransformToVisual(grid).TransformPoint({0, 0}).X - padding.Left;
    const Box box{bodyLeft - slope, 0.0, body.ActualWidth() + 2 * slope, notchHeight, slope, 0.0};
    // The dock's buttons reach `inset` past the flat body on either side.
    const double zoneLeft = bodyLeft - inset;
    const double zoneRight = bodyLeft + body.ActualWidth() + inset;

    const double x = cursorX - g_tracking.dockX - padding.Left;
    const double y = cursorY - g_tracking.dockY;
    // A vertical taskbar fails the width test: its row of buttons is narrower than a bulge.
    g_dock.bulgeWidth = g_settings.dockWidth;
    const bool over = g_settings.dock && HasDockNotch(body, slope) && zoneRight - zoneLeft >= g_dock.bulgeWidth &&
                      x >= zoneLeft && x <= zoneRight && y >= padding.Top && y <= grid.ActualHeight();
    const double target = over ? std::clamp(ButtonCentre(grid, x, padding.Left), zoneLeft, zoneRight) - box.left : 0.0;
    g_dock.originX = g_tracking.dockX + padding.Left;
    g_dock.originY = g_tracking.dockY + padding.Top;
    return Advance(g_dock, outline, over, target, box, dt);
}

// The tray's frame. Its outline lives in the island's root panel, because the headroom above the
// tray is outside the tray's own elements; the box is wherever #SystemTrayFrameGrid is in that panel.
bool TickTray(double cursorX, double cursorY, double dt) {
    auto root = g_tracking.root.get();
    auto trayGrid = g_tracking.trayGrid.get();
    auto outline = root ? FindOutline(root, g_tray.name) : nullptr;
    auto dockGrid = g_tracking.dockGrid.get();
    if (!outline || !trayGrid || !dockGrid) {
        g_tray.rise = 0;
        return false;
    }
    // Measured 2026-11-12: the tray's frame and grid are as tall as the whole window (56 px at an
    // 8 px headroom, both at y 0), yet the notch is drawn 48 px tall against their bottom edge: the
    // mask image keeps its own height. So the tray notch is as tall as the dock's, sits on the grid's
    // bottom edge, and the strip above it is the headroom.
    const double notchHeight = dockGrid.ActualHeight() - dockGrid.Padding().Top;
    // `root` is the panel holding the tray's outline: the island's root panel, or the dock's own grid
    // where the tray is nested in the taskbar's row. The outline's canvas sits inside that panel's padding.
    Thickness hostPadding{};
    if (auto hostGrid = root.try_as<Controls::Grid>()) {
        hostPadding = hostGrid.Padding();
    }
    const auto inHost = trayGrid.TransformToVisual(root).TransformPoint({0, 0});
    const double left = inHost.X - hostPadding.Left;
    const double top = inHost.Y - hostPadding.Top + trayGrid.ActualHeight() - notchHeight;
    const Box box{left, top, trayGrid.ActualWidth(), notchHeight, kSlopeWidthRatio * notchHeight, top};
    const auto rootOrigin = root.TransformToVisual(nullptr).TransformPoint({0, 0});
    g_tray.originX = rootOrigin.X + hostPadding.Left;
    g_tray.originY = rootOrigin.Y + hostPadding.Top;
    // Content is laid out inside the border and the padding. The theme for the original Styler draws
    // the right slope as a border and pulls the padding back, so the insets are the sum of the two.
    Thickness padding{};
    Thickness border{};
    if (auto grid = trayGrid.try_as<Controls::Grid>()) {
        padding = grid.Padding();
        border = grid.BorderThickness();
    } else if (auto stack = trayGrid.try_as<Controls::StackPanel>()) {
        padding = stack.Padding();
        border = stack.BorderThickness();
    }
    const double insetLeft = border.Left + padding.Left;
    const double insetRight = border.Right + padding.Right;

    const double x = cursorX - g_tray.originX - box.left;    // from the box's left edge
    const double y = cursorY - g_tray.originY;
    g_tray.bulgeWidth = g_settings.trayWidth;
    // The theme draws the tray's right slope as a right border one slope wide; without it there is no notch.
    const bool notch = std::abs(border.Right - box.slope) <= 1.0;
    const bool over = g_settings.tray && notch && box.width >= 2 * box.slope && x >= insetLeft &&
                      x <= box.width - insetRight && y >= box.clip && y <= box.top + box.height;
    if (over) {
        DebugLog(L"tray tick: box %.0f,%.0f %.0fx%.0f, slope %.0f, x %.0f, lift %.3f", box.left, box.top, box.width,
                 box.height, box.slope, x, g_tray.lift);
    }
    return Advance(g_tray, outline, over, x, box, dt);
}

void OnTick() {
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        g_overlay.Hide();
        StopFollowing();
        return;
    }
    const auto now = std::chrono::steady_clock::now();
    const double dt = std::clamp(std::chrono::duration<double>(now - g_lastTick).count(), 0.0, 1.0 / 30);
    g_lastTick = now;

    // The cursor inside the taskbar window, in layout px.
    const double x = cursor.x / g_tracking.scale - g_tracking.windowX;
    const double y = cursor.y / g_tracking.scale - g_tracking.windowY;
    const bool dockActive = TickDock(x, y, dt);
    const bool trayActive = TickTray(x, y, dt);
    UpdateOverlay();                                         // hides itself once both are flat
    if (!dockActive && !trayActive) {
        StopFollowing();                                     // both flat: nothing to animate until the next wake-up
    }
}

// Finds the tray from the dock's frame (they share a parent), gives it its outline and a pointer
// handler. The handler is an ordinary XAML one; it is removed on unload.
void AttachTray(FrameworkElement const& taskbarFrame) {
    if (g_tracking.trayFrame.get() && g_tracking.trayGrid.get() && g_tracking.root.get()) {
        return;
    }
    // Windows builds from late 2026 nest the tray in the taskbar's own row of buttons. Its outline
    // then lives in the dock's grid, and the tray's grid itself carries the pointer handler.
    auto dockGrid = FindRootGrid(taskbarFrame);
    auto nested = dockGrid ? FindNamed(taskbarFrame, L"SystemTrayFrameGrid", 10) : nullptr;
    if (nested) {
        RemoveCanvas(dockGrid, g_tray.name);
        AddOutline(dockGrid, g_tray.name, 1);
        g_tray = Surface{kTrayBulgeName, kTrayBulgeWidth};
        g_trayHandler = winrt::box_value(Input::PointerEventHandler(
            [](winrt::Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const&) {
                // Nothing may escape into Explorer: a failure here only costs the bulge.
                try {
                    auto element = sender.try_as<FrameworkElement>();
                    if (element && Calibrate(element)) {
                        StartFollowing();
                    }
                } catch (...) {
                }
            }));
        nested.AddHandler(UIElement::PointerMovedEvent(), g_trayHandler, true);
        g_tracking.root = winrt::make_weak(dockGrid.as<Controls::Panel>());
        g_tracking.trayGrid = winrt::make_weak(nested);
        g_tracking.trayFrame = winrt::make_weak(nested.as<UIElement>());
        DebugLog(L"tray: attached nested, grid %.0fx%.0f", nested.ActualWidth(), nested.ActualHeight());
        return;
    }

    auto parent = Media::VisualTreeHelper::GetParent(taskbarFrame);
    auto root = parent.try_as<Controls::Panel>();
    if (!root) {
        DebugLog(L"tray: the taskbar frame's parent is not a panel (%s)",
                 parent ? winrt::get_class_name(parent).c_str() : L"none");
        return;
    }
    for (auto const& child : root.Children()) {
        DebugLog(L"tray: root child %s", winrt::get_class_name(child).c_str());
    }
    for (auto const& child : root.Children()) {
        if (winrt::get_class_name(child) != L"SystemTray.SystemTrayFrame") {
            continue;
        }
        auto trayGrid = FindNamed(child, L"SystemTrayFrameGrid", 3);
        if (!trayGrid) {
            DebugLog(L"tray: SystemTrayFrameGrid not found");
            return;
        }
        RemoveCanvas(root, g_tray.name);
        AddOutline(root, g_tray.name, 0);
        g_tray = Surface{kTrayBulgeName, kTrayBulgeWidth};

        g_trayHandler = winrt::box_value(Input::PointerEventHandler(
            [](winrt::Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const&) {
                // Nothing may escape into Explorer: a failure here only costs the bulge.
                try {
                    auto element = sender.try_as<FrameworkElement>();
                    if (element && Calibrate(element)) {
                        StartFollowing();
                    }
                } catch (...) {
                }
            }));
        child.AddHandler(UIElement::PointerMovedEvent(), g_trayHandler, true);
        g_tracking.root = winrt::make_weak(root);
        g_tracking.trayGrid = winrt::make_weak(trayGrid);
        g_tracking.trayFrame = winrt::make_weak(child);
        DebugLog(L"tray: attached, grid %.0fx%.0f", trayGrid.ActualWidth(), trayGrid.ActualHeight());
        return;
    }
    DebugLog(L"tray: no SystemTrayFrame beside the taskbar frame");
}

// The taskbar's own pointer events are only the wake-up call: they stop arriving once a window
// preview opens, and no exit event follows, which left the bulge standing (seen 2026-11-12). From
// here the per-frame callback follows the real cursor.
void OnDockPointerMoved(FrameworkElement const& frame) {
    auto grid = FindRootGrid(frame);
    if (!grid) {
        DebugLog(L"moved: no RootGrid in the frame");
        return;
    }
    if (!Calibrate(frame)) {
        return;                                              // the cursor has already left the taskbar
    }
    if (!FindOutline(grid, g_dock.name)) {
        RemoveCanvas(grid, g_dock.name);
        AddOutline(grid, g_dock.name, 1);                    // behind the buttons, in front of the background
        g_dock = Surface{kDockBulgeName, kDockBulgeWidth};
    }
    const auto origin = grid.TransformToVisual(nullptr).TransformPoint({0, 0});
    g_tracking.dockGrid = winrt::make_weak(grid);
    g_tracking.dockX = origin.X;
    g_tracking.dockY = origin.Y;
    AttachTray(frame);
    StartFollowing();
}

// Returns the TaskbarFrame behind a hooked call, or null for any other control.
FrameworkElement AsTaskbarFrame(void* pThis) {
    FrameworkElement element = nullptr;
    static_cast<IUnknown*>(pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") {
        return nullptr;
    }
    return element;
}

using TaskbarFrame_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;

int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    // Nothing may escape into Explorer: a failure here only costs the bulge.
    try {
        if (auto frame = AsTaskbarFrame(pThis)) {
            OnDockPointerMoved(frame);
        }
    } catch (winrt::hresult_error const& e) {
        DebugLog(L"moved: error 0x%08X %s", static_cast<unsigned>(e.code().value), e.message().c_str());
    } catch (...) {
        DebugLog(L"moved: unknown error");
    }
    return TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

// Taskbar.View.dll is not loaded yet when Explorer starts; hook it when it arrives.
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (HookTaskbarViewDllSymbols(module) && !g_hooksApplied.exchange(true)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}

using RunFromWindowThreadProc_t = void (*)(void*);

// Runs proc on the thread that owns hWnd (the taskbar's UI thread) and waits for it.
bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT message = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == message) {
                    const auto* param = reinterpret_cast<const Param*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    Param param{proc, procParam};
    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// Takes back everything this mod put into the taskbar. Runs on the thread that owns the XAML tree.
void RemoveEverything() {
    StopFollowing();
    g_overlay.Destroy();
    if (auto trayFrame = g_tracking.trayFrame.get(); trayFrame && g_trayHandler) {
        trayFrame.RemoveHandler(UIElement::PointerMovedEvent(), g_trayHandler);
    }
    g_trayHandler = nullptr;
    for (auto const& weak : g_canvases) {
        auto canvas = weak.get();
        auto host = canvas ? Media::VisualTreeHelper::GetParent(canvas).try_as<Controls::Panel>() : nullptr;
        uint32_t index = 0;
        if (host && host.Children().IndexOf(canvas, index)) {
            host.Children().RemoveAt(index);
        }
    }
    g_canvases.clear();
    g_tracking = {};
}

// Reads the settings, keeping each inside the range its description gives. A colour that is not six
// hex digits leaves black.
void LoadSettings() {
    g_settings.dock = Wh_GetIntSetting(L"dock") != 0;
    g_settings.tray = Wh_GetIntSetting(L"tray") != 0;
    g_settings.liftRoom = std::clamp(Wh_GetIntSetting(L"height"), 2, 24);
    g_settings.dockWidth = std::clamp(Wh_GetIntSetting(L"dockWidth"), 60, 400);
    g_settings.trayWidth = std::clamp(Wh_GetIntSetting(L"trayWidth"), 40, 300);

    uint32_t colour = 0xFF000000u;
    PCWSTR text = Wh_GetStringSetting(L"color");
    if (text) {
        const wchar_t* digits = text[0] == L'#' ? text + 1 : text;
        wchar_t* end = nullptr;
        const unsigned long value = wcstoul(digits, &end, 16);
        if (end - digits == 6 && *end == L'\0') {
            colour = 0xFF000000u | static_cast<uint32_t>(value);
        }
        Wh_FreeStringSetting(text);
    }
    g_settings.colour = colour;
}

}  // namespace

void Wh_ModSettingsChanged() {
    LoadSettings();
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"));
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
    }
}

void Wh_ModBeforeUninit() {
    if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RunFromWindowThread(
            taskbar,
            [](void*) {
                try {
                    RemoveEverything();
                } catch (...) {
                }
            },
            nullptr);
    }
}
