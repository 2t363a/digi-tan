#ifndef REFRACT_CANVAS_HH
#define REFRACT_CANVAS_HH 1

#include <array>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

struct Color {
    double r = 1.0;
    double g = 1.0;
    double b = 1.0;
}; /// Color

static inline constexpr
Color make_color(double r, double g, double b) { return Color{ .r = r, .g = g, .b = b}; }

static inline constexpr
Color make_gray_color(double bw) { return Color{ .r = bw, .g = bw, .b = bw}; }

static inline constexpr
int compare3_color(Color const& a, Color const& b) {
    if (a.r < b.r) { return -1; }
    else if (a.r > b.r) { return +1; }
    if (a.g < b.g) { return -1; }
    else if (a.g > b.g) { return +1; }
    if (a.b < b.b) { return -1; }
    else if (a.b > b.b) { return +1; }
    return 0;
}


class Canvas {
    static constexpr unsigned kCircleN = 4'096; /// Circle drawing limit (probably never used)
    static constexpr int sgn(int x) { return (x > 0) ? 1 : ((x < 0) ? -1 : 0); }
    static constexpr Color kColorBlack = Color(0.0, 0.0, 0.0);
    static constexpr Color kColorWhite = Color(1.0, 1.0, 1.0);
public:
    Canvas(unsigned width, unsigned height, bool mode8 = true);

    void set_mode8() { m_mode8 = true; }

    void set_true_color(bool raster = true) {
        if (m_mode8 and raster) {raster8(); } 
        m_mode8 = false; 
    }

    void write_ppm16(std::string const& path) { write_ppm(65535, path); }

    void clear();
    void fill(uint8_t color);
    void fill(Color c);

    void set_bg_color(Color c) { m_bg_color = c; }
    void set_bg_color(uint8_t color) { m_bg_color8 = color; }

    uint8_t get_bg_color8() { return m_bg_color8; }

    void set_fg_color(Color c) { m_fg_color = c; }
    void set_fg_color(uint8_t color) { m_fg_color8 = color; }

    void pset(int x, int y) { auto o = at(x, y); if (o >= 0 and o < m_np) { put_pixel(o); } }
    void pset(int x, int y, uint8_t c) { auto o = at(x, y); if (o >= 0 and o < m_np) { put_pixel8(o, c); } }

    uint8_t pget8(int x, int y) {
        auto o = at(x, y);
        if (o >= 0 and o < m_np) { return m_color8[o]; }
        return -1;
    }


    void circle(int xc, int yc, int rad);
    void rectangle(int xl, int yb, int xr, int yt);
    void line(int xl, int yb, int xr, int yt);
    void flood_fill_quad(int x0, int y0);
    void flood_fill_oct(int x0, int y0);

    void wave_fill_quad(int m, int const* x0, int const* y0, uint8_t const* colors);
    void wave_fill_oct(int m, int const* x0, int const* y0, uint8_t const* colors);

    /// profiling counter
    unsigned long get_pixel_count() const { return m_plotted; }
    void reset_pixel_count() { m_plotted = 0; }
private:
    void raster8();

    void put_pixel(long offset) {
        if (m_mode8) { 
            m_color8[offset] = m_fg_color8;
        } else {
            m_color[offset] = m_fg_color;
        }
        m_plotted += 1;
    }

    void put_pixel8(long offset, uint8_t color) {
        m_color8[offset] = color;
        m_plotted += 1;
    }

    void init_color_palette();
    void gline1(int xl, int yb, int xr, int yt);
    void hline(int xl, int xr, int y);
    void vline(int x, int yb, int yt);

    void write_ppm(unsigned largest, std::string const& output_path);
    constexpr long ifma3(int x, int y, int z) { return (long)x * y + z; }
    constexpr long at(int x, int y) {
        auto i = ifma3(m_width, y, x);
        if (i < 0 or i >= m_np) { i = m_np; }
        return i;
    }


/// ---
    std::vector<Color> m_color;
    std::vector<uint8_t> m_color8; /// 8bit-color mode buffer

    long m_np = 0; /// number of pixels

    std::array<Color, 256> m_palette8;

    int m_height = 0;
    int m_width = 0;
    bool m_mode8 = false;

    Color m_bg_color = kColorBlack;
    uint8_t m_bg_color8 = 0;

    Color m_fg_color = kColorWhite;
    uint8_t m_fg_color8 = 15;

    unsigned long m_plotted = 0;
}; /// class Canvas


#endif /// #ifndef REFRACT_CANVAS_HH
