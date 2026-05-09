#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <climits>
#include <cstring>
#include <queue>
#include <sstream>
#include <stdexcept>

#include "canvas.hh"

struct Color48 {
    uint16_t ir;
    uint16_t ig;
    uint16_t ib;
};

struct Color24 {
    uint8_t ir;
    uint8_t ig;
    uint8_t ib;
};

static inline constexpr
Color48 convert_to_color48(Color c) {
    auto ClampU16 = [=](double v) {
        if (v < 0.0) { v = 0.0; }
        else if (v > 1.0) { v = 1.0; }
        uint16_t b = std::trunc(v * 65535);
        return b;
    };

    Color48 c48;
    c48.ir = ClampU16(c.r);
    c48.ig = ClampU16(c.g);
    c48.ib = ClampU16(c.b);

    return c48;
} /// Color48 convert_to_color48(...)

static inline constexpr
Color24 convert_to_color24(Color c) {
    auto ClampU8 = [=](double v) {
        if (v < 0.0) { v = 0.0; }
        else if (v > 1.0) { v = 1.0; }
        uint8_t b = std::trunc(v * 255);
        return b;
    };

    Color24 c24;
    c24.ir = ClampU8(c.r);
    c24.ig = ClampU8(c.g);
    c24.ib = ClampU8(c.b);

    return c24;
} /// Color24 convert_to_color24(...)

Canvas::Canvas(unsigned width, unsigned height, bool mode8) {
    long np = width;
    np *= height;
    if (0 == np) { throw std::invalid_argument("width * height == 0"); }

    m_np = np;
    m_width = width;
    m_height = height;

    m_mode8 = mode8;
    m_color.assign(m_np + 1, kColorBlack);
    m_color8.assign(m_np + 1, 0);

    init_color_palette();
} /// Canvas::Canvas

void Canvas::clear() {
    if (m_mode8) {
        std::fill(m_color8.begin(), m_color8.end(), m_bg_color8);
    } else {
        std::fill(m_color.begin(), m_color.end(), m_bg_color);
    }
} /// void Canvas::clear()

void Canvas::fill(uint8_t color) {
    std::fill(m_color8.begin(), m_color8.end(), color);
} /// void Canvas::fill(uint8_t)

void Canvas::fill(Color color) {
    std::fill(m_color.begin(), m_color.end(), color);
} /// void Canvas::fill(Color)

void Canvas::raster8() {
    if (not m_mode8) { return; }

    for (long i = 0; i < m_np; ++i) {
        auto c = m_palette8[m_color8[i]];
        m_color[i] = c;
    }
} /// void Canvas::raster8()

void Canvas::write_ppm(unsigned largest, std::string const& path) {
    if (m_mode8) { raster8(); }

    FILE* fout = fopen(path.c_str(), "wb+");
    if (nullptr == fout) {
        std::ostringstream o;
        o << "file " << path << " cannot be open for writing";
        auto s = o.str();
        throw std::runtime_error(s.c_str());
    }

    fprintf(fout, "P6\n");
    fprintf(fout, "%d\n", m_width);
    fprintf(fout, "%d\n", m_height);
    fprintf(fout, "%u\n", largest);

    unsigned s = (largest > 255) ? 2 : 1;
    std::vector<uint8_t> data(m_np * 3 * s);
    unsigned long bc = 0;

    for (int i = 0; i < m_height; ++i) {
        unsigned long off1 = (unsigned long)m_width * i;
        for (int j = 0; j < m_width; ++j) {
            unsigned long off = off1 + j;
            auto c = m_color[off];
            if (s > 1) {
                Color48 c48 = convert_to_color48(c);
                memcpy(&data[bc], &c48, sizeof(c48));
                bc += sizeof(c48);
            } else {
                Color24 c24 = convert_to_color24(c);
                memcpy(&data[bc], &c24, sizeof(c24));
                bc += sizeof(c24);
            }
        }
    } /// for (int i = 0; i < m_height; ++i)

    fwrite(data.data(), 1, bc, fout);
    fclose(fout);
} /// void Canvas::write_ppm(...)

void Canvas::circle(int xc, int yc, int rad) {
    int w;
    int h;

    /// initialization of circle algorithm
    w = xc + rad;
    h = yc;
    int dw = -1;
    int dh = -1;
    long ee = 0;

    unsigned i = 0;
    while (i < kCircleN) {
        long off = at(w, h);
        if (off >= 0 and off < m_np) {
            put_pixel(off);
        }

        if ((w == xc) and (h < yc)) { dw = -1; dh = +1; }
        if ((h == yc) and (w < xc)) { dw = +1; dh = +1; }
        if ((w == xc) and (h > yc)) { dw = +1; dh = -1; }
        if ((h == yc) and (w > xc) and i > 0) { break; }

        long ew = 2L * (w - xc) * dw + 1;
        long eh = 2L * (h - yc) * dh + 1;

        auto e1 = ee + ew;
        auto e2 = ee + ew + eh;
        auto e3 = ee + eh;

        auto ae1 = std::abs(e1);
        auto ae2 = std::abs(e2);
        auto ae3 = std::abs(e3);

        if (ae1 <= ae2 and ae1 <= ae3) { w += dw; ee = e1; }
        if (ae2 <= ae1 and ae2 <= ae3) { w += dw; h += dh; ee = e2; }
        if (ae3 <= ae1 and ae3 <= ae2) { h += dh; ee = e3; }

        i += 1;
    }
} /// void Canvas::circle(...)

void Canvas::rectangle(int xl, int yb, int xr, int yt) {
    hline(xl, xr, yb);
    hline(xl, xr, yt);
    vline(xl, yb, yt);
    vline(xr, yb, yt);
} /// void Canvas::rectangle(...)

void Canvas::line(int x1, int y1, int x2, int y2) {
    if (x1 == x2) { vline(x1, y1, y2); }
    else if (y1 == y2) { hline(x1, x2, y1); }
    gline1(x1, y1, x2, y2);
} /// void Canvas::line(...)

void Canvas::gline1(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int sx = sgn(dx);
    int sy = sgn(dy);

    dx = abs(dx);
    dy = abs(dy);
    bool sw = dy > dx; /// above Pi/4
    if (sw) { int t; t = dx, dx = dy, dy = t; }
    int e = 2 * dy - dx;
    int i = 0;
    int x = x1;
    int y = y1;
    while (i < dx) {
        long off = at(x, y);
        put_pixel(off);
        while (e >= 0) {
            if (sw) { x += sx; }
            else { y += sy; }
            e -= 2 * dx;
        }
        if (sw) { y += sy; }
        else { x += sx; }
        e += 2 * dy;
        i += 1;
    } /// while (i < dx)
} /// void Canvas::gline1(...)

void Canvas::hline(int x1, int x2, int y) {
    if (y < 0 or y >= m_height) { return; }
    if (x1 > x2) { int t; t = x1, x1 = x2, x2 = t; }
    int x = x1;
    unsigned long off = at(x, y);
    while (x >= 0 and x < m_width and x <= x2) {
        put_pixel(off);
        off += 1;
        x += 1;
    } /// while (x >= 0 ...)
} /// void Canvas::hline(...)

void Canvas::vline(int x, int y1, int y2) {
    if (x < 0 or x >= m_width) { return; }
    if (y1 > y2) { int t; t = y1, y1 = y2, y2 = t; }
    int y = y1;
    unsigned long off = at(x, y1);
    while (y >= 0 and y < m_height and y <= y2) {
        put_pixel(off);
        off += m_width;
        y += 1;
    } /// while (y >= 0 ...)
} /// void Canvas::vline(...)

void Canvas::init_color_palette() {
    unsigned c = 0;
    Color cl;

    for (;c < 16; ++c) {
        double k = (c >= 8) ? 1.0 : 0.5;
        switch (c & 7) {
            case 0: cl = make_color(k * 0.0, k * 0.0, k * 0.0); break; // Black
            case 1: cl = make_color(k * 0.0, k * 0.0, k * 1.0); break; // Blue
            case 2: cl = make_color(k * 0.0, k * 1.0, k * 0.0); break; // Green
            case 3: cl = make_color(k * 0.0, k * 1.0, k * 1.0); break; // Cyan
            case 4: cl = make_color(k * 1.0, k * 0.0, k * 0.0); break; // Red
            case 5: cl = make_color(k * 1.0, k * 0.0, k * 1.0); break; // Magenta
            case 6: cl = make_color(k * 1.0, k * 1.0, k * 0.0); break; // Magenta
            case 7: cl = make_color(k * 1.0, k * 1.0, k * 1.0); break; // White
        }
        m_palette8[c] = cl;
    }

    for (unsigned r = 0; r < 6; ++r) {
        for (unsigned g = 0; g < 6; ++g) {
            for (unsigned b = 0; b < 6; ++b) {
                cl = make_color(r * 1.0 / 6.0, g * 1.0 / 6.0, b * 1.0 / 6.0);
                m_palette8[c] = cl;
                ++c;
            }
        }
    }
} /// void Canvas::init_color_palette()

void Canvas::flood_fill_quad(int x0, int y0) {
    std::queue<long> q;
    long o = at(x0, y0);
    if (o < 0 or o >= m_np) { return; }

    unsigned max_s = 0;
    q.push(o);
    while (not q.empty()) {
        auto p = q.front();
        q.pop();
        if (p < 0 or p >= m_np) { continue; }
        if (m_mode8) {
            uint8_t c = m_color8[p];
            if (c != m_bg_color8) { continue; }
        } else {
            Color c = m_color[p];
            if (compare3_color(c, m_bg_color) != 0) { continue; }
        }
        put_pixel(p);
        q.push(p + 1);
        q.push(p - m_width);
        q.push(p - 1);
        q.push(p + m_width);
    } /// while (not q.empty())

    if (q.size() > max_s) { max_s = q.size(); }
} ///void Canvas::flood_fill(int x0, int y0)

void Canvas::flood_fill_oct(int x0, int y0) {
    std::queue<long> q;
    long o = at(x0, y0);
    if (o < 0 or o >= m_np) { return; }

    unsigned max_s = 0;
    q.push(o);
    while (not q.empty()) {
        auto p = q.front();
        q.pop();
        if (p < 0 or p >= m_np) { continue; }
        if (m_mode8) {
            uint8_t c = m_color8[p];
            if (c != m_bg_color8) { continue; }
        } else {
            Color c = m_color[p];
            if (compare3_color(c, m_bg_color) != 0) { continue; }
        }
        put_pixel(p);
        q.push(p + 1);
        q.push(p + 1 - m_width);
        q.push(p - m_width);
        q.push(p - 1 - m_width);
        q.push(p - 1);
        q.push(p - 1 + m_width);
        q.push(p + m_width);
        q.push(p + 1 + m_width);
    } /// while (not q.empty())

    if (q.size() > max_s) { max_s = q.size(); }
} ///void Canvas::flood_fill(int x0, int y0)

void Canvas::wave_fill_oct(int m, int const* x0, int const* y0, uint8_t const* color) {
    if (not m_mode8) { return; }
    std::vector<std::queue<long>> vq(m);

    for (int i = 0; i < m; ++i) {
        long o = at(x0[i], y0[i]);
        if (o < 0 or o >= m_np) { return; }
        vq[i].push(o);
    }

    bool empty = false;
    bool wave_direction = true;
    while (not empty) {
        empty = true;
        int i_begin = wave_direction ? 0 : (m - 1);
        int i_end = wave_direction ? m : -1;
        int i_incr = (wave_direction) ? 1 : -1;
        wave_direction = not wave_direction;

        for (int i = i_begin; i != i_end; i += i_incr) {
            auto& q = vq[i];
            if (q.empty()) { continue; }
            empty = false;
            auto p = q.front();
            q.pop();
            if (p < 0 or p >= m_np) { continue; }
            uint8_t c = m_color8[p];
            if (c != m_bg_color8) { continue; }
            put_pixel8(p, color[i]);
            q.push(p + 1);
            q.push(p + 1 - m_width);
            q.push(p - m_width);
            q.push(p - 1 - m_width);
            q.push(p - 1);
            q.push(p - 1 + m_width);
            q.push(p + m_width);
            q.push(p + 1 + m_width);
        }
    } /// while (not q.empty())
} ///void Canvas::wave_fill_oct(int x0, int y0)

void Canvas::wave_fill_quad(int m, int const* x0, int const* y0, uint8_t const* color) {
    if (not m_mode8) { return; }
    std::vector<std::queue<long>> vq(m);

    for (int i = 0; i < m; ++i) {
        long o = at(x0[i], y0[i]);
        if (o < 0 or o >= m_np) { return; }
        vq[i].push(o);
    }

    bool empty = false;
    bool wave_direction = true;
    while (not empty) {
        empty = true;
        int i_begin = wave_direction ? 0 : (m - 1);
        int i_end = wave_direction ? m : -1;
        int i_incr = (wave_direction) ? 1 : -1;
        wave_direction = not wave_direction;

        for (int i = i_begin; i != i_end; i += i_incr) {
            auto& q = vq[i];
            if (q.empty()) { continue; }
            empty = false;
            auto p = q.front();
            q.pop();
            if (p < 0 or p >= m_np) { continue; }
            uint8_t c = m_color8[p];
            if (c != m_bg_color8) { continue; }
            put_pixel8(p, color[i]);
            q.push(p + 1);
            q.push(p - m_width);
            q.push(p - 1);
            q.push(p + m_width);
        }
    } /// while (not q.empty())
} ///void Canvas::wave_fill_quad(int x0, int y0)
