/*
    FontRenderer.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Dependencies : 
        Freetype - used to load font file into memory
        OpenGL - used to render text
        GLM - for vector, matrix calculation

    FontRenderer class definition
*/
#ifndef FONT_RENDERER_H_
#define FONT_RENDERER_H_

#include <string>
#include <cstdint>
#include <ostream>

// ----------------
class FontRenderer
{
private:
    class SharedData;

public:
    //
    // CTOR
    //
    FontRenderer();

    //
    // width - width of the screen as pixels per row [px]
    // height - height of the screen as pixels per column [px]
    //
    static void SetScreenSize(float width, float height);

    //
    // x - position of origin of the screen as pixel. default value is 0 [px]
    // y - position of origin of the screen as pixel. default value is 0 [px]
    //
    static void SetScreenOrigin(float x, float y);

    //
    // path - path to the font file. [basic char set]
    //
    void SetFont(const char* const path);

    //
    // str - utf32 string to be printed
    // offset_x - offset of point is (screen_origin + offset). this defines x. positive goes right. [px]
    // offset_y - offset of point is (screen_origin + offset). this defines y. positive goes up. [px]
    // font_scale - scale of the font. when given (1.0f) as value, the size follows the glyph_size(set_glyph_size). [scalar]
    //
    void RenderText(const std::u32string& str
        , float offset_x = .0f
        , float offset_y = .0f
        , float font_scale = 1.0f);

    //
    // size - size of glyph in pixel. Note that most character will be smaller than the size given here.
    //        For example, 'I' is thin in x direction. default value is 1.0f. [px]
    //
    void SetGlyphSize(float size);
    float GetGlyphSize() const;

    //
    // os - report ostream wherer all the message sent from "ogl_font" to you will be stored.
    //      default is std::cout
    //
    static void SetReportStream(std::ostream& os);
    static std::ostream& GetReportStream();

private:
    static SharedData& GetSharedData();

private:
    //
    // path to the font file. [ansi]
    //
    std::string m_fontPath;
    //
    // size of each glyph in pixel. Note that most character will be smaller than the size given here.
    // For example, 'I' is thin in x direction. default value is 1.0f. [px]
    //
    float       m_glyphSize;
};

#endif