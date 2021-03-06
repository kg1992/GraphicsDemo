/*
    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    Dependencies :
        Freetype - used to load font file into memory
        OpenGL - used to render text
        GLM - for vector, matrix calculation

    References :
        http://soen.kr/lecture/library/freetype/ft2.htm

    FontRenderer class implementation.

    * Note about "26.6 fixed-point pixel coordinate".
    FreeType uses specific fixed point 32-bit numeric type.
    The significant 26 bits are integer part, and the last 6 bits are for fractional part.
    We can apply (>> 6) (shift to right by 6 bits) to truncate fractional part.

    * Note about 'Point' 'Pixel' 'Inch'

    In real world, 72.26 points = 1 inch.
    In computer graphics, 72 points = 1 inch for convinence. ... (1)

    To convert point to 'pixel' we need to know 'resolution'
    The resolution is represented with unit 'dpi' (dots per inch)
    If a printer has 600 dpi resolution, it means printer can put 600 dots in an inch;
    the higher the dpi, the higher the precision.
    While a printer will put 'dots' on the paper,
    display device will put 'pixels' on the screen.
    So to speark for screen, it is more like 'ppi'(pixels per inch) than 'dpi'(dots per inch)
    
    Thus, with the above fact and equation (1) we can say,

    (Font size in pixel unit) = point / 72   *   resolution 
                                ----------       ----------
                    font size in inch(inch)      pixels per inch(pixel/inch)

    GetDeviceCaps function can be used to acquire the resolution of the display device.
    However, in reality, it is impossible to find and use perfect size,
    because there are too many variable ( user display settings, projector, mag/minification )

    Sometimes application just picks a resolution and sticks to it.
    i.e. Macintoshi 72 dpi for convinient computation
    i.e. Windows 96 dpi for bigger font

    One may use FT_Set_Pixel_Sizes() instead of FT_Set_Char_Size to set the size of font
    with pixel unit instead of point.
*/
#include "Common.h"
#include "FontRenderer.h"
#include "Errors.h"

class FontRenderer::SharedData
{
public:
    SharedData()
        : m_screenWidth(0)
        , m_screenHeight(0)
        , m_screenOriginX(0)
        , m_screenOriginY(0)
        , m_freetype(0)
        , m_face(0)
        , m_program()
        , m_vbo(0)
        , m_vao(0)
        , m_reportStream(&std::cout)
    {
    }
    void            initialize();

public:
    float           m_screenWidth;
    float           m_screenHeight;
    float           m_screenOriginX;
    float           m_screenOriginY;
    FT_Library      m_freetype;
    FT_Face         m_face;
    ShaderProgram   m_program;
    GLuint          m_vbo;
    GLuint          m_vao;
    std::ostream*   m_reportStream;
};

void FontRenderer::SharedData::initialize()
{
    FT_Init_FreeType(&m_freetype);

    ShaderProgram sp;

    const GLchar vssource[] =
        "#version 440 \n"
        "layout(location=0) in vec3 position;"
        "layout(location=1) in vec2 uv;"
        "out vec2 vUv;"
        "uniform mat4 uTransform;"
        "void main()"
        "{"
        " gl_Position = uTransform * vec4(position,1);"
        " vUv = vec2(uv.x, 1-uv.y);"
        "}\n"
        ;

    const GLchar fssource[] =
        "#version 440 \n"
        "in vec2 vUv;"
        "out vec4 FragColor;"
        "uniform sampler2D uTexture;"
        "void main()"
        "{"
        "   FragColor = texture(uTexture, vUv);"
        "}"
        ;

    m_program.InitBySource("FontRenderer Shader", vssource, fssource);

    // --------------------
    // Vertex Buffer Object
    float vb[] =
    {
        -0.5f, -0.5f, 0,
        0.5f, -0.5f, 0,
        -0.5f, 0.5f, 0,

        0.5f, -0.5f, 0,
        0.5f, 0.5f, 0,
        -0.5f, 0.5f, 0,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vb), vb, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // --------------------
    // Vertex Array Object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(float) * 18));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

FontRenderer::FontRenderer()
    : m_fontPath()
    , m_glyphSize(32.0f)
{
}

void FontRenderer::SetScreenSize(float width, float height)
{
    GetSharedData().m_screenWidth = width;
    GetSharedData().m_screenHeight = height;
}

void FontRenderer::SetScreenOrigin(float x, float y)
{
    GetSharedData().m_screenOriginX = x;
    GetSharedData().m_screenOriginY = y;
}

void FontRenderer::SetFont(const char* const path)
{
    m_fontPath = path;
}

void FontRenderer::RenderText(const std::u32string& str
    , float offset_x
    , float offset_y
    , float font_scale)
{
    ////////////////////////////////////////
    // Rendering context
    // Width of the render target rect
    const float screen_width = GetSharedData().m_screenWidth;
    // Height of the render target rect
    const float screen_height = GetSharedData().m_screenHeight;
    // Aspect ratio of the render target rect
    const float aspect_ratio = screen_width / screen_height;
    ////////////////////////////////////////
    // renderig assets from opengl
    // rendering program
    GLuint gl_program = GetSharedData().m_program.Handle();
    // rendering vertex array object
    GLuint vao = GetSharedData().m_vao;
    // glyph size measured from freetype font data
    float glyph_size = m_glyphSize;
    // beginning point of drawing
    float begin_x = GetSharedData().m_screenOriginX + offset_x;
    float begin_y = GetSharedData().m_screenOriginY + offset_y;

    // Load Face
    FT_Face face;
    auto error = FT_New_Face(GetSharedData().m_freetype, m_fontPath.c_str(), 0, &face);
    if (error)
    {
        // Cannot opne font file. escape.
        return;
    }
    error = FT_Set_Char_Size(face, 0, static_cast<FT_F26Dot6>(glyph_size) * 64, 0, 72);

    // Prepare GL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(gl_program);
    GLuint glyphTexture = 0;
    glGenTextures(1, &glyphTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glyphTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindVertexArray(vao);
    glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);
    // For Each Glyph
    float pen_x = static_cast<float>(begin_x - screen_width / 2);
    float pen_y = static_cast<float>(begin_y - screen_height / 2);
    for (std::uint32_t character : str)
    {
        error = FT_Load_Char(face, character, FT_LOAD_RENDER);
        if (error)
            continue;

        FT_GlyphSlot glyphSlot = face->glyph;
        FT_Bitmap bitmap = glyphSlot->bitmap;
        FT_Glyph_Metrics glyphMetrics = glyphSlot->metrics;
        FT_Glyph glyph;
        FT_Get_Glyph(glyphSlot, &glyph);
        FT_BBox cbox;
        FT_Glyph_Get_CBox(glyph, 0, &cbox);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap.width, bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);
        //
        // 150901 - Repeatedly throws GL_INVALID_ENUM error only on relrease build(from GAM400 project)
        //
        GLenum glerror = glGetError(); if (glerror) GetReportStream() << glerror << std::endl;
        float width = static_cast<float>((cbox.xMax - cbox.xMin) >> 6);
        float height = static_cast<float>((cbox.yMax - cbox.yMin) >> 6);
        float cx = static_cast<float>((cbox.xMin + cbox.xMax) >> 7);
        float cy = static_cast<float>((cbox.yMin + cbox.yMax) >> 7);
        glm::vec3 translate((pen_x + cx * font_scale) / screen_width * 2
            , (pen_y + cy * font_scale) / screen_height * 2
            , 0);
        glm::mat4 transform = glm::translate(translate)
            * glm::scale(glm::vec3(width * font_scale / screen_width * 2
                , height * font_scale / screen_height * 2
                , 1));
        glUniformMatrix4fv(glGetUniformLocation(gl_program, "uTransform"), 1, GL_FALSE, &transform[0][0]);
        GET_AND_HANDLE_GL_ERROR();
        glUniform1i(glGetUniformLocation(gl_program, "uTexture"), 0);
        GET_AND_HANDLE_GL_ERROR();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        GET_AND_HANDLE_GL_ERROR();
        pen_x += (glyphMetrics.horiAdvance >> 6) * font_scale;
        FT_Done_Glyph(glyph);
    }

    // End GL
    glDeleteTextures(1, &glyphTexture);
    glyphTexture = 0;
    glUseProgram(0);
    glBindVertexArray(0);

    // End FreeTYpe
    FT_Done_Face(face);
}

void FontRenderer::SetGlyphSize(float size)
{
    m_glyphSize = size;
}

float FontRenderer::GetGlyphSize() const
{
    return m_glyphSize;
}

void FontRenderer::SetReportStream(std::ostream& os)
{
    GetSharedData().m_reportStream = &os;
}

std::ostream& FontRenderer::GetReportStream()
{
    return *GetSharedData().m_reportStream;
}

FontRenderer::SharedData& FontRenderer::GetSharedData()
{
    static std::unique_ptr<SharedData> data;
    if (!data)
    {
        data.reset(new SharedData);
        data->initialize();
    }

    return *data;
}
