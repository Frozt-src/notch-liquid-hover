// ==WindhawkMod==
// @id              notch-liquid-hover
// @name            Notch Liquid Hover
// @description     Pulls a soft bulge of the NotchDock's body up under the pointer, like a liquid surface
// @version         0.7.0
// @author          frozt
// @github          https://github.com/Frozt-src
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
lifts with it, and the icons ride up and down on the surface. Click an icon and the surface dips,
springs back and sends a ripple running out along the notch; sweep the pointer fast and the swell
leans into the motion. When an app asks for attention, the surface breathes over its icon and then
settles into a low swell until you look. The notch around the clock and system icons rises and
ripples too.

**For the best experience, install this mod alongside the NotchDock theme for
[Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler).** The theme
works on its own; this mod is what makes its notches move. The theme and its setup are linked from
[the mod's repository](https://github.com/Frozt-src/notch-liquid-hover).

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
- **Liquid motion**: the dip and ripple on a click, and the lean on a fast sweep (on by default).
- **Attention swell**: the swell over an app that asks for attention (on by default).
- **Icons ride the surface**: dock icons rise and fall with the surface under them (on by default).
- **Colour**: the notch's colour as `#RRGGBB` (default `#000000`, NotchDock's black). If you change
  the theme's notch colour, set the same colour here.

## Known limitations

- Built for NotchDock's notch shape only; other themes are ignored on purpose.
- The tray's bulge and the attention swell start working after the dock has been hovered once since
  Explorer started.
- The bulge sinks while another program's window covers the taskbar, such as a full-screen app.
- Tested on Windows 11 build 26300 with one taskbar. Taskbars on secondary monitors, top or side
  taskbars and other builds are untested.

## How it works

It hooks `TaskbarFrame::OnPointerMoved` in Taskbar.View.dll, as Taskbar Dock Animation does, only to
learn that the pointer has arrived and where the taskbar is. From then on a per-frame callback
follows the real cursor, because pointer events stop once a window preview opens. Each bulge is one
XAML `Polygon` redrawn every frame, plus the click-through overlay window described above. Everything
the mod adds is removed when it is disabled or unloaded.

## License

Copyright 2026 [frozt](https://github.com/Frozt-src). Licensed under the
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
- motion: true
  $name: Liquid motion
  $description: The surface dips and sends out a ripple when you click an icon, and leans into fast pointer moves
- attention: true
  $name: Attention swell
  $description: The surface breathes over an app that asks for attention, then rests as a low swell until you look
- ride: true
  $name: Icons ride the surface
  $description: Dock icons rise and fall with the surface under them
- color: "#000000"
  $name: Colour
  $description: The notch's colour as #RRGGBB. It must match the theme's notch, or the bulge shows as a patch
*/
// ==/WindhawkModSettings==

// Notch Liquid Hover
// Copyright 2026 frozt (https://github.com/Frozt-src)
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
#include <winrt/Windows.Foundation.Numerics.h>
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
#include <cwctype>
#include <limits>
#include <string_view>
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
    std::atomic<bool> motion{true};          // the dip and ripple on a click, the lean on a fast sweep
    std::atomic<bool> attention{true};       // the swell over an app that asks for attention
    std::atomic<bool> ride{true};            // dock icons rise and fall with the surface
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
constexpr double kTipTaper = 4.0;        // px of plate under which a slope stops being raised, so its tip stays put
constexpr double kMinZone = 24.0;        // px; a dock narrower than this does not rise
// Liquid motion. A click knocks the spring down by kPressImpulse (lift per second), which dips the
// surface by about a third before it springs back. A travelling crest leans: the side it moves toward
// shortens and the side behind stretches, by up to kLeanMax, reached at kLeanSpeed px per second.
constexpr double kPressImpulse = 12.0;
constexpr double kLeanSpeed = 6000.0;     // a hop to the next icon leans about half as far as a fast sweep
constexpr double kLeanMax = 0.3;
constexpr double kLeanTime = 0.06;       // s; how quickly the lean follows the crest's speed
constexpr double kLeanSag = 0.5;         // a leaning crest is this share of its lean lower
// A click also sends a ripple out each way from the crest: a small crest kRippleHalf px either side of
// its front, travelling at kRippleSpeed, its height dying away with time constant kRippleDecay.
constexpr double kRippleSpeed = 260.0;   // px per second
constexpr double kRippleHalf = 16.0;     // px
constexpr double kRippleShare = 0.8;     // of the hover's full rise (0.45 measured only 1 px past the bulge)
constexpr double kRippleDecay = 0.5;     // s
constexpr double kRippleLife = 1.6;      // s; by then it is under 5% of its height
constexpr size_t kMaxRipples = 4;
// An app asking for attention gets a swell over its icon that breathes kSwellBreaths times, then
// stays as a low swell (kSwellRest of its height) until the app no longer asks.
constexpr double kSwellWidth = 120.0;    // px
constexpr double kSwellShare = 0.7;      // of the hover's full rise
constexpr double kSwellPeriod = 1.8;     // s per breath
constexpr int kSwellBreaths = 3;
constexpr double kSwellRest = 0.35;
constexpr double kSwellFade = 0.3;       // s; how quickly a swell comes and goes
constexpr double kSwellMatch = 12.0;     // px; a button that moved less than this keeps its swell
constexpr double kRideShare = 0.9;       // of the surface's rise under an icon that the icon rises
constexpr double kScanInterval = 0.25;   // s between looks at the buttons while animating
constexpr UINT kAttentionPollMs = 400;   // ms between looks while idle

// The notch's slope as the NotchDock theme's images draw it (a quintic Bezier, 2.5 times the notch's
// height wide). kDockInsetRatio is the theme's body margin (dockH*0.3125): the dock's flat top starts
// that far inside the row of buttons. All of these must stay equal to the theme's.
constexpr double kSlopeWidthRatio = 2.5;
constexpr double kDockInsetRatio = 0.3125;
constexpr std::array<double, 6> kSlopeControlX = {0.0, 0.5492, 0.6484, 0.4716, 0.98, 1.0};
constexpr double kSlopeStart = 0.2370;

// Set first thing when Windhawk starts unloading the mod. From then on nothing new is added to the
// taskbar, so nothing can be left behind pointing into the unloaded DLL.
std::atomic<bool> g_unloading{false};

// ---------------------------------------------------------------------------------------------
// The notch's shape
// ---------------------------------------------------------------------------------------------

// Height of the notch's plate (0..1) at fraction u along a slope, from its tip (0) to where it joins
// the flat top (1). The same curve the theme's slope images are drawn from, to 0.01 px.
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

struct Ripple {
    double origin;             // where the click was, from the box's left edge
    double age;                // s
};
struct Swell {
    double centre;             // of the icon whose app asks for attention, from the box's left edge
    double level;              // 0..1, fades in while the app asks and out once it stops
    double age;                // s since it appeared; it breathes until kSwellBreaths breaths are done
    bool wanted;               // the app still asks
};

struct Surface {
    const wchar_t* name;       // of its canvas, so a stale one can be found and removed
    double bulgeWidth;
    uint32_t fill = 0xFF000000u;   // the outline's colour as last set, 0xAARRGGBB
    double crest = -1.0;       // where the crest is now, from the box's left edge; negative = not placed yet
    double lift = 0.0;         // how far the surface is raised: 0 flat, 1 fully, a little more on the overshoot
    double velocity = 0.0;     // of lift, per second
    double lean = 0.0;         // -kLeanMax..kLeanMax; positive while the crest travels right
    bool over = false;         // the pointer was over this notch last frame
    std::vector<Ripple> ripples{};
    std::vector<Swell> swells{};   // only the dock has any
    // What the last frame drew, for the overlay: the crest's rise in px (0 = flat), the notch's box,
    // and where the outline's coordinates start inside the taskbar window.
    double rise = 0.0;
    Box box{};
    double originX = 0.0;
    double originY = 0.0;
    // The inputs of the outline as last drawn. When a frame's are the same, nothing is redrawn.
    std::array<double, 13> drawn{};
    bool changed = false;      // the overlay must redraw this surface's part
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

// The bell's half-widths behind and ahead of the crest: equal at rest. While the crest travels, the
// side it moves toward shortens and the side behind it stretches, so the swell leans into the motion.
std::pair<double, double> Halves(Surface const& surface) {
    const double half = surface.bulgeWidth / 2;
    return {half * (1.0 + surface.lean), half * (1.0 - surface.lean)};   // left, right
}

// A raised cosine: 1 at u = 0, falling smoothly to 0 at u = -1 and 1.
double Bell(double u) {
    return std::abs(u) >= 1.0 ? 0.0 : 0.5 * (1.0 + std::cos(kPi * u));
}

// The hover's full rise in px, before the spring's overshoot.
double FullRise() {
    return g_settings.liftRoom * kLiftShare;
}

// A swell's height: it breathes from kSwellRest up to full and back, then rests at kSwellRest.
double SwellHeight(Swell const& swell) {
    double breath = kSwellRest;
    if (swell.age < kSwellBreaths * kSwellPeriod) {
        breath += (1.0 - kSwellRest) * 0.5 * (1.0 - std::cos(2.0 * kPi * swell.age / kSwellPeriod));
    }
    return FullRise() * kSwellShare * swell.level * breath;
}

// How far the surface is raised at `at` px from the box's left edge. The hover's bell and any
// attention swells are combined by taking the higher, so a hovered icon that asks for attention does
// not rise twice; ripples ride on top. The whole fades to nothing where a slope's plate grows thinner
// than kTipTaper, so a raise near an end never lifts the slope's tip off the bar. Advance() and the
// overlay both draw from this.
double RaiseAt(Surface const& surface, double at) {
    if (at < 0 || at > surface.box.width) {
        return 0.0;
    }
    double shape = 0.0;
    if (surface.rise > 0 && surface.crest >= 0) {
        const auto [left, right] = Halves(surface);
        const double d = at - surface.crest;
        const double half = d < 0 ? left : right;
        if (half > 0) {
            shape = surface.rise * Bell(d / half);
        }
    }
    for (auto const& swell : surface.swells) {
        shape = std::max(shape, SwellHeight(swell) * Bell((at - swell.centre) / (kSwellWidth / 2)));
    }
    double waves = 0.0;
    for (auto const& ripple : surface.ripples) {
        const double reach = kRippleSpeed * ripple.age;
        // Both fronts start on top of each other; the ramp keeps them from doubling at the click.
        const double height = FullRise() * kRippleShare * std::exp(-ripple.age / kRippleDecay) *
                              std::min(1.0, reach / (2 * kRippleHalf));
        waves += height * (Bell((at - (ripple.origin - reach)) / kRippleHalf) +
                           Bell((at - (ripple.origin + reach)) / kRippleHalf));
    }
    const double total = std::min(shape + waves, static_cast<double>(g_settings.liftRoom));
    if (total <= 0) {
        return 0.0;
    }
    const double plate = surface.box.height - SurfaceDrop(at, surface.box);
    return total * std::clamp(plate / kTipTaper, 0.0, 1.0);
}

// The stretch of the box, from its left edge, that RaiseAt() may lift now; empty (first >= second)
// when nothing is raised.
std::pair<double, double> Extent(Surface const& surface) {
    double low = std::numeric_limits<double>::infinity();
    double high = -low;
    const auto take = [&](double from, double to) {
        low = std::min(low, from);
        high = std::max(high, to);
    };
    if (surface.rise > 0 && surface.crest >= 0) {
        const auto [left, right] = Halves(surface);
        take(surface.crest - left, surface.crest + right);
    }
    for (auto const& swell : surface.swells) {
        if (swell.level > 0) {
            take(swell.centre - kSwellWidth / 2, swell.centre + kSwellWidth / 2);
        }
    }
    for (auto const& ripple : surface.ripples) {
        const double reach = kRippleSpeed * ripple.age + kRippleHalf;
        take(ripple.origin - reach, ripple.origin + reach);
    }
    return {std::max(low, 0.0), std::min(high, surface.box.width)};
}

// One frame of one surface: move the crest part of the way to its target (the lag is what makes it
// read as liquid), advance the spring that raises or lowers it, age the ripples and swells, and
// redraw the outline if anything it depends on changed. Returns false once nothing moves: either
// flat, or holding still with only resting attention swells, which stay drawn.
bool Advance(Surface& surface, Shapes::Polygon const& outline, bool over, double target, Box const& box, double dt) {
    surface.over = over;
    surface.box = box;
    const bool visible = surface.lift > 0.02 && surface.crest >= 0;
    const double previous = surface.crest;
    if (over) {
        surface.crest = visible ? surface.crest + (target - surface.crest) * (1.0 - std::exp(-dt / kFollowTime))
                                : target;            // flat: nothing to see, so jump
        if (std::abs(target - surface.crest) < 0.05) {
            surface.crest = target;
        }
    }
    // The lean follows the crest's speed, and returns to 0 once the crest stops.
    double leanGoal = 0.0;
    if (g_settings.motion && over && visible && dt > 0) {
        leanGoal = std::clamp((surface.crest - previous) / dt / kLeanSpeed, -kLeanMax, kLeanMax);
    }
    surface.lean += (leanGoal - surface.lean) * (1.0 - std::exp(-dt / kLeanTime));
    if (std::abs(surface.lean) < 0.001) {
        surface.lean = 0;
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
    if (over && std::abs(surface.lift - 1.0) < 0.002 && std::abs(surface.velocity) < 0.02) {
        surface.lift = 1.0;                          // settled: snap, so a resting frame matches the last one
        surface.velocity = 0;
    }
    const bool hoverFlat = !over && surface.lift < 0.004 && std::abs(surface.velocity) < 0.05;
    if (hoverFlat) {
        surface.lift = 0;
        surface.velocity = 0;
        surface.lean = 0;
    }

    // Ripples run outward and die away; swells fade in or out and breathe, then rest.
    for (auto& ripple : surface.ripples) {
        ripple.age += dt;
    }
    std::erase_if(surface.ripples, [](Ripple const& ripple) { return ripple.age > kRippleLife; });
    bool swellsMoving = false;
    const double fade = 1.0 - std::exp(-dt / kSwellFade);
    for (auto& swell : surface.swells) {
        const double goal = swell.wanted ? 1.0 : 0.0;
        swell.level += (goal - swell.level) * fade;
        if (std::abs(goal - swell.level) < 0.002) {
            swell.level = goal;
        }
        swell.age += dt;
        swellsMoving = swellsMoving || swell.level != goal || swell.age < kSwellBreaths * kSwellPeriod;
    }
    std::erase_if(surface.swells, [](Swell const& swell) { return !swell.wanted && swell.level == 0; });

    const double rise = hoverFlat ? 0.0 : FullRise() * surface.lift * (1.0 - kLeanSag * std::abs(surface.lean));
    if (hoverFlat && surface.ripples.empty() && surface.swells.empty()) {
        surface.changed = surface.changed || surface.drawn != decltype(surface.drawn){};
        surface.rise = 0;
        surface.drawn = {};
        if (outline.Points().Size() > 0) {
            outline.Points().Clear();
        }
        return false;
    }
    const bool moving = !hoverFlat || !surface.ripples.empty() || swellsMoving;

    // The outline is the notch's own top edge, raised where RaiseAt() says. Over a slope the edge it
    // raises is the slope itself, so near an end the pull carries on down the notch's curve; the lower
    // edge runs just under the surface, where it is black on black. Whatever of it lies above the
    // taskbar window is not drawn by XAML; the overlay draws that part.
    const uint32_t colour = g_settings.colour;
    double swellKey = 0.0;                           // changes when a swell moves, comes or goes
    for (size_t i = 0; i < surface.swells.size(); ++i) {
        swellKey += static_cast<double>(i + 1) * (surface.swells[i].centre * 1024.0 + surface.swells[i].level);
    }
    const std::array<double, 13> inputs{surface.crest, rise, surface.lean, surface.bulgeWidth, box.left, box.top,
                                        box.width, box.height, box.clip, static_cast<double>(colour), swellKey,
                                        static_cast<double>(surface.ripples.size()), 1.0};
    // Ripples and breathing swells change every frame without changing these inputs.
    if (surface.ripples.empty() && !swellsMoving && inputs == surface.drawn) {
        return moving;                               // resting: nothing to redraw
    }
    if (colour != surface.fill) {
        outline.Fill(Media::SolidColorBrush(winrt::Windows::UI::Color{
            255, static_cast<uint8_t>(colour >> 16), static_cast<uint8_t>(colour >> 8), static_cast<uint8_t>(colour)}));
        surface.fill = colour;
    }
    surface.rise = rise;
    Media::PointCollection points;
    const auto [low, high] = Extent(surface);
    if (low < high) {
        std::vector<winrt::Windows::Foundation::Point> under;
        const int steps = std::max(1, static_cast<int>(std::ceil((high - low) / kStep)));
        for (int k = 0; k <= steps; ++k) {
            const double at = low + (high - low) * k / steps;
            const double edge = std::max(box.clip, box.top + SurfaceDrop(at, box));   // the visible top edge here
            const auto px = static_cast<float>(box.left + at);
            points.Append({px, static_cast<float>(edge - RaiseAt(surface, at))});
            under.push_back({px, static_cast<float>(edge + kSeamOverlap)});
        }
        for (auto it = under.rbegin(); it != under.rend(); ++it) {
            points.Append(*it);
        }
    }
    outline.Points(points);
    surface.drawn = inputs;
    surface.changed = true;
    return moving;
}

// Flattens a surface that can no longer be drawn (its element went away). The overlay redraws if it
// was showing part of it.
void Flatten(Surface& surface) {
    if (surface.rise != 0 || !surface.ripples.empty() || !surface.swells.empty()) {
        surface.changed = true;
    }
    surface.rise = 0;
    surface.ripples.clear();
    surface.swells.clear();
    surface.drawn = {};
}

// ---------------------------------------------------------------------------------------------
// Where things are
// ---------------------------------------------------------------------------------------------

struct Tracking {
    winrt::weak_ref<Controls::Grid> dockGrid;    // Taskbar.TaskbarFrame > Grid#RootGrid
    winrt::weak_ref<Controls::Panel> root;       // the island's root panel, parent of both frames
    winrt::weak_ref<FrameworkElement> trayGrid;  // #SystemTrayFrameGrid, whose box is the tray notch
    winrt::weak_ref<UIElement> trayFrame;        // carries the pointer handler
    HWND window = nullptr;   // the taskbar window the pointer last woke the mod on
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
        const auto [low, high] = Extent(*surface);
        if (low >= high) {
            continue;
        }
        const Box& box = surface->box;
        const double start = surface->originX + box.left;    // the box's left edge in the window, layout px
        const int first = std::max(0, static_cast<int>(std::floor((start + low) * scale)));
        const int last = std::min(width - 1, static_cast<int>(std::ceil((start + high) * scale)));
        for (int column = first; column <= last; ++column) {
            const double at = (column + 0.5) / scale - surface->originX - box.left;
            const double raise = RaiseAt(*surface, at);
            if (raise <= 0) {
                continue;
            }
            const double edge = std::max(box.clip, box.top + SurfaceDrop(at, box));
            const double above = -(surface->originY + edge - raise) * scale;   // px over the window's top
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
void ResetAnimation();

void StartFollowing() {
    if (g_following || g_unloading) {
        return;
    }
    g_lastTick = std::chrono::steady_clock::now();
    g_renderingToken = Media::CompositionTarget::Rendering([](auto const&, auto const&) {
        try {
            OnTick();
        } catch (winrt::hresult_error const& e) {
            // Usually an element that left the tree while Explorer rebuilt the taskbar. Stop and flatten
            // everything rather than fail again every frame; the next pointer move starts afresh.
            Wh_Log(L"tick: error 0x%08X %s", static_cast<unsigned>(e.code().value), e.message().c_str());
            ResetAnimation();
        } catch (...) {
            Wh_Log(L"tick: unknown error");
            ResetAnimation();
        }
    });
    g_following = true;
}

void StopFollowing() {
    if (g_following) {
        Media::CompositionTarget::Rendering(g_renderingToken);
        g_following = false;                     // only once revoked, so a failed revoke is retried
    }
}

// The taskbar window under the cursor, or null if the cursor is not on a taskbar.
HWND TaskbarUnderCursor() {
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        return nullptr;
    }
    HWND window = GetAncestor(WindowFromPoint(cursor), GA_ROOT);
    wchar_t className[64] = {};
    if (!window || !GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (wcscmp(className, L"Shell_TrayWnd") != 0 && wcscmp(className, L"Shell_SecondaryTrayWnd") != 0)) {
        return nullptr;
    }
    return window;
}

// Records where the taskbar window is and its scale. The window's corner is used, not a pointer
// event's position: by the time an event is handled the cursor may have moved on, which put the
// bulge 35 px behind a fast pointer (measured 2026-11-12). False if the cursor is not on a taskbar.
bool Calibrate(FrameworkElement const& element) {
    HWND window = TaskbarUnderCursor();
    RECT rect;
    if (!window || !GetWindowRect(window, &rect)) {
        return false;
    }
    const auto root = element.XamlRoot();
    g_tracking.window = window;
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
// The bulge belongs to the icon under the pointer, not to the pointer itself.
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

// The dock's notch as the theme placed it. The notch is wherever the theme put its flat body, with a
// slope hung on each side. The row of buttons is no guide: since Windows moved the tray into that row
// (2026-11) it is wider than the dock. X positions are along the buttons' area (inside the padding).
struct DockGeometry {
    Controls::Grid grid{nullptr};
    FrameworkElement body{nullptr};
    Thickness padding{};
    double slope = 0.0;
    double inset = 0.0;      // the dock's buttons reach this far past the flat body on either side
    double bodyLeft = 0.0;
    Box box{};
};

bool MeasureDock(DockGeometry& dock) {
    dock.grid = g_tracking.dockGrid.get();
    dock.body = dock.grid ? FindDockBody(dock.grid) : nullptr;
    if (!dock.body || dock.body.ActualWidth() <= 0) {
        return false;
    }
    dock.padding = dock.grid.Padding();
    const double notchHeight = dock.grid.ActualHeight() - dock.padding.Top;
    dock.inset = kDockInsetRatio * notchHeight;
    dock.slope = kSlopeWidthRatio * notchHeight;
    dock.bodyLeft = dock.body.TransformToVisual(dock.grid).TransformPoint({0, 0}).X - dock.padding.Left;
    dock.box = Box{dock.bodyLeft - dock.slope, 0.0, dock.body.ActualWidth() + 2 * dock.slope, notchHeight, dock.slope, 0.0};
    return true;
}

// Follows the taskbar window, which can slide (auto-hide) or vanish (display change) while the
// pointer rests. False if it is gone.
bool RefreshWindow() {
    RECT rect;
    HWND window = g_tracking.window;
    if (!window || !IsWindow(window) || !GetWindowRect(window, &rect)) {
        return false;
    }
    if (!EqualRect(&rect, &g_tracking.windowRect)) {
        g_tracking.windowRect = rect;
        g_tracking.windowX = rect.left / g_tracking.scale;
        g_tracking.windowY = rect.top / g_tracking.scale;
    }
    return true;
}

// A dock icon that rides the surface. `item` is the button's slot in the row, whose layout position
// does not move when the Dock Animation mod scales the button; `icon` is its Image#Icon, moved with
// UIElement.Translation, a property the Dock Animation mod (which uses RenderTransform) leaves alone.
struct RideIcon {
    winrt::weak_ref<UIElement> icon;
    winrt::weak_ref<FrameworkElement> item;
    double rowX = 0.0;       // the row's offset along the buttons' area
    float applied = 0.0f;    // px the icon is raised now
};
std::vector<RideIcon> g_rideIcons;
std::chrono::steady_clock::time_point g_lastScan{};

// Puts every icon the mod raised back where it was.
void ClearRide() {
    for (auto const& ride : g_rideIcons) {
        try {
            if (auto icon = ride.icon.get(); icon && ride.applied != 0.0f) {
                icon.Translation({0.0f, 0.0f, 0.0f});
            }
        } catch (...) {
        }
    }
    g_rideIcons.clear();
}

// Raises each dock icon by the surface's rise under its centre.
void RideIcons() {
    for (auto& ride : g_rideIcons) {
        auto icon = ride.icon.get();
        auto item = ride.item.get();
        if (!icon || !item) {
            continue;
        }
        double lift = 0.0;
        if (g_settings.ride) {
            const double centre = ride.rowX + item.ActualOffset().x + item.ActualWidth() / 2 - g_dock.box.left;
            lift = kRideShare * RaiseAt(g_dock, centre);
        }
        const auto value = static_cast<float>(std::round(lift * 10.0) / 10.0);
        if (value != ride.applied) {
            icon.Translation({0.0f, -value, 0.0f});
            ride.applied = value;
        }
    }
}

FrameworkElement FindClass(DependencyObject const& parent, std::wstring_view className, int depth) {
    const int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (winrt::get_class_name(child) == className) {
            return child.try_as<FrameworkElement>();
        }
        if (depth > 1) {
            if (auto found = FindClass(child, className, depth - 1)) {
                return found;
            }
        }
    }
    return nullptr;
}

// True while the button's app asks for attention. Two of its icon panel's state groups show it:
// CommonStates (the plate, RequestingAttention*) only while the button flashes, and
// RunningIndicatorStates (the dot, RequestingAttentionRunningIndicator) for as long as the app asks
// (measured 2026-11-24: CommonStates alone lost the swell after about two breaths).
bool AsksForAttention(FrameworkElement const& panel) {
    for (auto const& group : VisualStateManager::GetVisualStateGroups(panel)) {
        const auto name = group.Name();
        if (name != L"CommonStates" && name != L"RunningIndicatorStates") {
            continue;
        }
        if (auto state = group.CurrentState(); state && std::wstring_view(state.Name()).starts_with(L"RequestingAttention")) {
            return true;
        }
    }
    return false;
}

// True if the taskbar can be seen at x along the dock's buttons: it is not slid away, and no other
// window (a full-screen app) covers it there. The overlay is topmost, so a swell must not be drawn
// over an app that covers the taskbar.
bool ShownAt(DockGeometry const& dock, double x) {
    const double scale = g_tracking.scale;
    const POINT point{
        static_cast<LONG>(std::lround((g_tracking.windowX + g_tracking.dockX + dock.padding.Left + x) * scale)),
        static_cast<LONG>(std::lround(g_tracking.windowRect.bottom - 0.5 * dock.box.height * scale))};
    return GetAncestor(WindowFromPoint(point), GA_ROOT) == g_tracking.window;
}

// Looks at the dock's buttons: refreshes the icons that ride the surface, and gives each button whose
// app asks for attention a swell (when `swells` is false, every swell is let go). Returns true if
// the swells changed.
bool ScanButtons(DockGeometry const& dock, bool swells) {
    g_lastScan = std::chrono::steady_clock::now();
    std::vector<RideIcon> icons;
    std::vector<double> asking;                              // swell centres, from the box's left edge
    for (auto const& child : dock.grid.Children()) {
        auto row = child.try_as<FrameworkElement>();
        if (!row || row.Name() != L"TaskbarFrameRepeater") {
            continue;
        }
        const double rowX = row.ActualOffset().x - dock.padding.Left;
        const int count = Media::VisualTreeHelper::GetChildrenCount(row);
        for (int i = 0; i < count; ++i) {
            auto item = Media::VisualTreeHelper::GetChild(row, i).try_as<FrameworkElement>();
            if (!item || item.ActualWidth() <= 0) {
                continue;
            }
            // Only app buttons: Start, the tray's icons and the rest have no Image#Icon to ride or attention.
            auto button = winrt::get_class_name(item) == L"Taskbar.TaskListButton"
                              ? item
                              : FindClass(item, L"Taskbar.TaskListButton", 2);
            auto panel = button ? FindNamed(button, L"IconPanel", 2) : nullptr;
            if (!panel) {
                continue;
            }
            if (auto icon = FindNamed(panel, L"Icon", 2)) {
                icons.push_back({winrt::make_weak(icon.as<UIElement>()), winrt::make_weak(item), rowX, 0.0f});
            }
            const double centre = rowX + item.ActualOffset().x + item.ActualWidth() / 2;
            if (swells && AsksForAttention(panel) && ShownAt(dock, centre)) {
                asking.push_back(centre - dock.box.left);
            }
        }
    }
    // Icons that are still there keep their lift; the ones that left are put back.
    for (auto const& old : g_rideIcons) {
        auto element = old.icon.get();
        auto match = std::find_if(icons.begin(), icons.end(), [&](RideIcon const& ride) { return ride.icon.get() == element; });
        if (match != icons.end()) {
            match->applied = old.applied;
        } else if (element && old.applied != 0.0f) {
            element.Translation({0.0f, 0.0f, 0.0f});
        }
    }
    g_rideIcons = std::move(icons);

    // Pair each asking button with the swell nearest it; new ones start a swell, the rest let go.
    bool changed = false;
    std::vector<bool> seen(g_dock.swells.size(), false);
    for (const double centre : asking) {
        size_t match = g_dock.swells.size();
        for (size_t j = 0; j < seen.size(); ++j) {
            if (!seen[j] && std::abs(g_dock.swells[j].centre - centre) < kSwellMatch) {
                match = j;
                break;
            }
        }
        if (match == g_dock.swells.size()) {
            g_dock.swells.push_back({centre, 0.0, 0.0, true});
            seen.push_back(true);
            changed = true;
            continue;
        }
        Swell& swell = g_dock.swells[match];
        seen[match] = true;
        changed = changed || !swell.wanted || std::abs(swell.centre - centre) > 0.25;
        swell.wanted = true;
        swell.centre = centre;
    }
    for (size_t j = 0; j < seen.size(); ++j) {
        if (!seen[j] && g_dock.swells[j].wanted) {
            g_dock.swells[j].wanted = false;
            changed = true;
        }
    }
    return changed;
}

// The dock's frame of the animation. Its outline lives in the root grid at the corner of the
// buttons' area, which is level with the notch's flat top.
bool TickDock(double cursorX, double cursorY, bool reachable, double dt) {
    auto grid = g_tracking.dockGrid.get();
    auto outline = grid ? FindOutline(grid, g_dock.name) : nullptr;
    DockGeometry dock;
    if (!outline || !MeasureDock(dock)) {
        Flatten(g_dock);
        ClearRide();
        if (outline && outline.Points().Size() > 0) {
            outline.Points().Clear();
        }
        return false;
    }
    const double zoneLeft = dock.bodyLeft - dock.inset;
    const double zoneRight = dock.bodyLeft + dock.body.ActualWidth() + dock.inset;

    const double x = cursorX - g_tracking.dockX - dock.padding.Left;
    const double y = cursorY - g_tracking.dockY;
    // A dock with few icons gets a bulge no wider than its row of buttons. A vertical taskbar has no
    // notch (the theme leaves it alone), so HasDockNotch already rules it out.
    g_dock.bulgeWidth = std::min<double>(g_settings.dockWidth, zoneRight - zoneLeft);
    const bool notch = g_settings.dock && HasDockNotch(dock.body, dock.slope) && zoneRight - zoneLeft >= kMinZone;
    const bool over = reachable && notch && x >= zoneLeft && x <= zoneRight && y >= dock.padding.Top &&
                      y <= dock.grid.ActualHeight();
    const double target =
        over ? std::clamp(ButtonCentre(dock.grid, x, dock.padding.Left), zoneLeft, zoneRight) - dock.box.left : 0.0;
    g_dock.originX = g_tracking.dockX + dock.padding.Left;
    g_dock.originY = g_tracking.dockY + dock.padding.Top;
    if (std::chrono::steady_clock::now() - g_lastScan >= std::chrono::duration<double>(kScanInterval)) {
        ScanButtons(dock, notch && g_settings.attention);
    }
    const bool active = Advance(g_dock, outline, over, target, dock.box, dt);
    RideIcons();
    return active;
}

// While nothing animates, the mod looks at the buttons every kAttentionPollMs for an app that starts
// asking for attention (or stops). The timer belongs to a hidden message-only window of the system's
// "Static" class, so destroying that window ends the timer. Created and destroyed on the taskbar's
// UI thread.
HWND g_timerWindow = nullptr;

void CALLBACK AttentionTick(HWND, UINT, UINT_PTR, DWORD) {
    try {
        if (g_unloading || g_following) {
            return;                                          // while following, TickDock looks every frame
        }
        DockGeometry dock;
        if (!RefreshWindow() || !MeasureDock(dock)) {
            return;
        }
        const bool notch = g_settings.dock && HasDockNotch(dock.body, dock.slope);
        if (ScanButtons(dock, notch && g_settings.attention)) {
            StartFollowing();
        }
        RideIcons();                                         // lets go of the icons if riding was just turned off
    } catch (...) {
    }
}

void StartAttentionTimer() {
    if (g_timerWindow || g_unloading) {
        return;
    }
    g_timerWindow = CreateWindowExW(0, L"Static", nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
    if (g_timerWindow && !SetTimer(g_timerWindow, 1, kAttentionPollMs, AttentionTick)) {
        DestroyWindow(g_timerWindow);
        g_timerWindow = nullptr;
    }
}

void StopAttentionTimer() {
    if (g_timerWindow) {
        KillTimer(g_timerWindow, 1);
        DestroyWindow(g_timerWindow);
        g_timerWindow = nullptr;
    }
}

// The tray's frame. Its outline lives in the island's root panel, because the headroom above the
// tray is outside the tray's own elements; the box is wherever #SystemTrayFrameGrid is in that panel.
bool TickTray(double cursorX, double cursorY, bool reachable, double dt) {
    auto root = g_tracking.root.get();
    auto trayGrid = g_tracking.trayGrid.get();
    auto outline = root ? FindOutline(root, g_tray.name) : nullptr;
    auto dockGrid = g_tracking.dockGrid.get();
    if (!outline || !trayGrid || !dockGrid) {
        Flatten(g_tray);
        if (outline && outline.Points().Size() > 0) {
            outline.Points().Clear();
        }
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
    const bool over = reachable && g_settings.tray && notch && box.width >= 2 * box.slope && x >= insetLeft &&
                      x <= box.width - insetRight && y >= box.clip && y <= box.top + box.height;
    return Advance(g_tray, outline, over, x, box, dt);
}

// Clears both outlines in whatever taskbar they were last drawn.
void ClearOutlines() {
    try {
        if (auto grid = g_tracking.dockGrid.get()) {
            if (auto outline = FindOutline(grid, g_dock.name)) {
                outline.Points().Clear();
            }
        }
    } catch (...) {
    }
    try {
        if (auto root = g_tracking.root.get()) {
            if (auto outline = FindOutline(root, g_tray.name)) {
                outline.Points().Clear();
            }
        }
    } catch (...) {
    }
}

// Flattens both surfaces at once and stops following, after an error or when the taskbar the mod was
// following goes away. Never throws.
void ResetAnimation() {
    g_overlay.Hide();
    try {
        StopFollowing();
    } catch (...) {
    }
    for (Surface* surface : {&g_dock, &g_tray}) {
        surface->crest = -1.0;
        surface->lift = 0.0;
        surface->velocity = 0.0;
        surface->lean = 0.0;
        surface->over = false;
        surface->rise = 0.0;
        surface->ripples.clear();
        surface->swells.clear();
        surface->drawn = {};
        surface->changed = false;
        surface->fill = 0;                       // not a colour the mod uses: the next frame sets the brush again
    }
    ClearRide();
    g_lastScan = {};                             // look at the buttons again on the next frame
    ClearOutlines();
}

void DetachTray();

bool g_buttonWasDown = false;
std::array<double, 8> g_overlayInputs{};

void OnTick() {
    POINT cursor;
    // Follow the taskbar's window every frame, not only as of the last pointer event.
    if (!GetCursorPos(&cursor) || !RefreshWindow()) {
        ResetAnimation();
        return;
    }
    const RECT rect = g_tracking.windowRect;
    // The pointer only counts as on the taskbar if no other window covers that spot, such as a
    // full-screen app. (The overlay strip is click-through, so WindowFromPoint looks past it.)
    const bool reachable = GetAncestor(WindowFromPoint(cursor), GA_ROOT) == g_tracking.window;

    const auto now = std::chrono::steady_clock::now();
    const double dt = std::clamp(std::chrono::duration<double>(now - g_lastTick).count(), 0.0, 1.0 / 30);
    g_lastTick = now;

    // A click on a raised notch knocks its surface down, and sends a ripple out each way from the
    // crest; the spring brings the surface back with a bounce.
    const int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    const bool down = (GetAsyncKeyState(button) & 0x8000) != 0;
    if (down && !g_buttonWasDown && g_settings.motion) {
        for (Surface* surface : {&g_dock, &g_tray}) {
            if (surface->over && surface->lift > 0.5) {
                surface->velocity -= kPressImpulse;
                if (surface->ripples.size() < kMaxRipples) {
                    surface->ripples.push_back({surface->crest, 0.0});
                }
            }
        }
    }
    g_buttonWasDown = down;

    // The cursor inside the taskbar window, in layout px.
    const double x = cursor.x / g_tracking.scale - g_tracking.windowX;
    const double y = cursor.y / g_tracking.scale - g_tracking.windowY;
    const bool dockActive = TickDock(x, y, reachable, dt);
    // A tray that fails (its element left the tree) is dropped and found again on the next pointer
    // move; the dock carries on.
    bool trayActive = false;
    try {
        trayActive = TickTray(x, y, reachable, dt);
    } catch (...) {
        Flatten(g_tray);
        try {
            DetachTray();
        } catch (...) {
        }
    }
    // The overlay is redrawn only when a surface or the window moved.
    const std::array<double, 8> overlayInputs{
        static_cast<double>(rect.left), static_cast<double>(rect.top), static_cast<double>(rect.right),
        g_dock.originX, g_dock.originY, g_tray.originX, g_tray.originY, 1.0};
    if (g_dock.changed || g_tray.changed || overlayInputs != g_overlayInputs) {
        UpdateOverlay();                                     // hides itself once both are flat
        g_overlayInputs = overlayInputs;
        g_dock.changed = false;
        g_tray.changed = false;
    }
    if (!dockActive && !trayActive) {
        StopFollowing();                                     // both flat: nothing to animate until the next wake-up
        g_overlayInputs = {};
    }
}

// Takes the tray's pointer handler off the element it was put on and forgets the tray, so it can be
// attached afresh (another taskbar, or a rebuilt tray) without leaving a handler behind.
std::chrono::steady_clock::time_point g_lastTrayAttempt{};

void DetachTray() {
    try {
        auto trayFrame = g_tracking.trayFrame.get();
        if (trayFrame && g_trayHandler) {
            trayFrame.RemoveHandler(UIElement::PointerMovedEvent(), g_trayHandler);
        }
    } catch (...) {
    }
    g_trayHandler = nullptr;
    try {
        if (auto root = g_tracking.root.get()) {
            if (auto outline = FindOutline(root, g_tray.name)) {
                outline.Points().Clear();
            }
        }
    } catch (...) {
    }
    Flatten(g_tray);
    g_tracking.trayFrame = nullptr;
    g_tracking.trayGrid = nullptr;
    g_tracking.root = nullptr;
    g_lastTrayAttempt = {};
}

// Puts the tray's outline into `host` and a pointer handler on `target`. The handler is an ordinary
// XAML one; DetachTray() or the unload removes it.
void HookUpTray(Controls::Panel const& host, uint32_t index, FrameworkElement const& trayGrid, UIElement const& target) {
    RemoveCanvas(host, g_tray.name);
    AddOutline(host, g_tray.name, index);
    g_tray = Surface{kTrayBulgeName, kTrayBulgeWidth};
    g_tray.changed = true;                                   // the overlay drops whatever it showed of the old tray
    g_trayHandler = winrt::box_value(Input::PointerEventHandler(
        [](winrt::Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const&) {
            // Nothing may escape into Explorer: a failure here only costs the bulge.
            try {
                if (g_unloading) {
                    return;
                }
                auto element = sender.try_as<FrameworkElement>();
                if (element && Calibrate(element)) {
                    StartFollowing();
                }
            } catch (...) {
            }
        }));
    target.AddHandler(UIElement::PointerMovedEvent(), g_trayHandler, true);
    g_tracking.root = winrt::make_weak(host);
    g_tracking.trayGrid = winrt::make_weak(trayGrid);
    g_tracking.trayFrame = winrt::make_weak(target);
    Wh_Log(L"tray: attached, grid %.0fx%.0f", trayGrid.ActualWidth(), trayGrid.ActualHeight());
}

// Finds the tray from the dock's frame and gives it its outline and pointer handler. A tray that is
// not found is looked for again at most every two seconds, not on every pointer move.
void AttachTray(FrameworkElement const& taskbarFrame) {
    if (g_tracking.trayFrame.get() && g_tracking.trayGrid.get() && g_tracking.root.get()) {
        return;
    }
    const auto now = std::chrono::steady_clock::now();
    if (now - g_lastTrayAttempt < std::chrono::seconds(2)) {
        return;
    }
    DetachTray();                                            // whatever is left of an earlier tray
    g_lastTrayAttempt = now;

    // Windows builds from late 2026 nest the tray in the taskbar's own row of buttons. Its outline
    // then lives in the dock's grid, and the tray's grid itself carries the pointer handler.
    auto dockGrid = FindRootGrid(taskbarFrame);
    if (auto nested = dockGrid ? FindNamed(taskbarFrame, L"SystemTrayFrameGrid", 10) : nullptr) {
        HookUpTray(dockGrid, 1, nested, nested);
        return;
    }
    // Earlier builds: the tray is a SystemTray.SystemTrayFrame beside the taskbar frame, in the island's
    // root panel, which then also holds the tray's outline (the space above the tray is outside the
    // tray's own elements).
    auto root = Media::VisualTreeHelper::GetParent(taskbarFrame).try_as<Controls::Panel>();
    if (!root) {
        Wh_Log(L"tray: the taskbar frame's parent is not a panel");
        return;
    }
    for (auto const& child : root.Children()) {
        if (winrt::get_class_name(child) != L"SystemTray.SystemTrayFrame") {
            continue;
        }
        if (auto trayGrid = FindNamed(child, L"SystemTrayFrameGrid", 3)) {
            HookUpTray(root, 0, trayGrid, child);
        } else {
            Wh_Log(L"tray: SystemTrayFrameGrid not found");
        }
        return;
    }
    Wh_Log(L"tray: no SystemTrayFrame beside the taskbar frame");
}

HWND FindOwnTaskbar();

// The taskbar's own pointer events are only the wake-up call: they stop arriving once a window
// preview opens, and no exit event follows, which left the bulge standing. From here the per-frame
// callback follows the real cursor.
void OnDockPointerMoved(FrameworkElement const& frame) {
    auto grid = FindRootGrid(frame);
    if (!grid) {
        return;
    }
    // Without the NotchDock notch there is nothing to raise, and nothing is added to the taskbar.
    auto body = FindDockBody(grid);
    if (!body || !HasDockNotch(body, kSlopeWidthRatio * (grid.ActualHeight() - grid.Padding().Top))) {
        return;
    }
    // A late event from a taskbar the cursor has already left for another one: the window under the
    // cursor is not this grid's, so it must not be paired with it.
    auto previous = g_tracking.dockGrid.get();
    if (previous == grid && g_tracking.window && TaskbarUnderCursor() != g_tracking.window) {
        return;
    }
    if (!Calibrate(frame)) {
        return;                                              // the cursor has already left the taskbar
    }
    // Another taskbar (another monitor) than the one the mod was following: flatten what it drew
    // there and let the tray attach to this taskbar's tray.
    if (previous && previous != grid) {
        ResetAnimation();
        DetachTray();
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
    try {
        AttachTray(frame);                                   // a tray that fails must not stop the dock
    } catch (...) {
        Wh_Log(L"tray: attach failed");
    }
    StartFollowing();
    // The idle attention timer is only started on the thread the unload's clean-up runs on (the
    // main taskbar window's), so that clean-up can always destroy it.
    if (HWND taskbar = FindOwnTaskbar(); taskbar && GetWindowThreadProcessId(taskbar, nullptr) == GetCurrentThreadId()) {
        StartAttentionTimer();
    }
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
        if (g_unloading) {
            // Being unloaded: add nothing that would outlive the DLL.
        } else if (auto frame = AsTaskbarFrame(pThis)) {
            OnDockPointerMoved(frame);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"moved: error 0x%08X %s", static_cast<unsigned>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"moved: unknown error");
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
        if (HookTaskbarViewDllSymbols(module)) {
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
// Each step is attempted on its own, so one that fails does not leave the others undone.
void RemoveEverything() {
    const auto attempt = [](auto&& step) {
        try {
            step();
        } catch (...) {
        }
    };
    attempt([] { StopFollowing(); });
    attempt([] { StopAttentionTimer(); });
    attempt([] { ClearRide(); });
    attempt([] { g_overlay.Destroy(); });
    attempt([] { DetachTray(); });
    g_trayHandler = nullptr;
    for (auto const& weak : g_canvases) {
        attempt([&weak] {
            auto canvas = weak.get();
            auto host = canvas ? Media::VisualTreeHelper::GetParent(canvas).try_as<Controls::Panel>() : nullptr;
            uint32_t index = 0;
            if (host && host.Children().IndexOf(canvas, index)) {
                host.Children().RemoveAt(index);
            }
        });
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
    g_settings.motion = Wh_GetIntSetting(L"motion") != 0;
    g_settings.attention = Wh_GetIntSetting(L"attention") != 0;
    g_settings.ride = Wh_GetIntSetting(L"ride") != 0;

    uint32_t colour = 0xFF000000u;
    PCWSTR text = Wh_GetStringSetting(L"color");
    if (text) {
        const wchar_t* digits = text[0] == L'#' ? text + 1 : text;
        const bool hex = wcslen(digits) == 6 && std::all_of(digits, digits + 6, [](wchar_t c) {
                             return std::iswxdigit(static_cast<wint_t>(c)) != 0;
                         });
        if (hex) {
            colour = 0xFF000000u | static_cast<uint32_t>(wcstoul(digits, nullptr, 16));
        }
        Wh_FreeStringSetting(text);
    }
    g_settings.colour = colour;
}

// The taskbar window of this Explorer process, or null. Another explorer.exe (separate-process folder
// windows) must not be asked to run anything of this mod.
HWND FindOwnTaskbar() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD process = 0;
    if (taskbar) {
        GetWindowThreadProcessId(taskbar, &process);
    }
    return process == GetCurrentProcessId() ? taskbar : nullptr;
}

void RemoveEverythingOnTaskbarThread() {
    if (HWND taskbar = FindOwnTaskbar()) {
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

bool HookTaskbarViewIfLoaded() {
    HMODULE module = GetTaskbarViewModuleHandle();
    if (!module || g_taskbarViewDllLoaded.exchange(true)) {
        return false;
    }
    return HookTaskbarViewDllSymbols(module);
}

}  // namespace

void Wh_ModSettingsChanged() {
    LoadSettings();
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarViewIfLoaded()) {
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibraryExW =
            kernelBase ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW")) : nullptr;
        if (!loadLibraryExW) {
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // Taskbar.View.dll may have arrived between Wh_ModInit and the hooks going live, when the
    // LoadLibraryExW hook could not see it yet. If it was already hooked, this does nothing.
    if (HookTaskbarViewIfLoaded()) {
        Wh_ApplyHookOperations();
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;                                      // first: from here nothing new is added
    RemoveEverythingOnTaskbarThread();
}

void Wh_ModUninit() {
    // The hooks are gone by now, so nothing can add to the taskbar again; take back anything a call
    // that was already under way added after the first clean-up.
    RemoveEverythingOnTaskbarThread();
}
