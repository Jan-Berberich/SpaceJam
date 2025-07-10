#include "wizball.h"

// --- Helper function to check shader compilation/linking errors ---
void wbShaderCheckError(GLuint shader, const char* type) {
    GLint success;
    GLchar info_log[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, info_log);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, info_log);
        }
    }
}

// --- Function to compile and link shaders ---
GLuint wbShaderCreate(const char* vertex_source, const char* fragment_source) {
    // Vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glCompileShader(vertex);
    wbShaderCheckError(vertex, "VERTEX");

    // Fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
    wbShaderCheckError(fragment, "FRAGMENT");

    // Shader Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    wbShaderCheckError(program, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

void wbShaderInit(WBShader* shader) {
    // Create and Compile Shaders
    static const char* vertex_shader_src =
        "#version 330 core\n"
        "layout (location = 0) in vec2 pos;"
        "layout (location = 1) in vec2 tex;"
        "out vec2 TexCoord;"
        "void main() {"
        "    gl_Position = vec4(pos, 0.0, 1.0);"
        "    TexCoord = tex;"
        "}";
    
    static const char* fragment_shader_src =
        "#version 330 core\n"
        "in vec2 TexCoord;"
        "out vec4 FragColor;"
        "uniform sampler2D texSampler;"
        "uniform vec4 keyColor;"
        "uniform vec4 replaceColor;"
        "uniform float time;"
        "uniform float keyAlpha;"
        "uniform float texSize;"
        "uniform int keyColorMode;"
        "uniform float windowScale;"
        "const int replaceColorCntMax = 32;"
        "uniform vec4 replaceColors[replaceColorCntMax];"
        "uniform float replaceColorbandHeight;"
        "uniform float replaceColorSpeed;"
        "uniform int replaceColorCnt;"
        "uniform float subpixelCnt;"
        "uniform float replaceColorMirrorHeight;"
        "uniform float fillLevel;"
        "void main() {"
        "    if (TexCoord.x < 0.0 || TexCoord.x > 1.0 || TexCoord.y < 0.0 || TexCoord.y > 1.0) {"
        "        discard;"
        "    }"
        "    vec4 texColor = texture(texSampler, TexCoord);"
        "    if (all(equal(texColor, keyColor))) {"
        "        int idx;"
        "        switch (keyColorMode) {"
        "            case 0:"
        "            FragColor = replaceColors[0];"
        "            break;"
        "            case 1:"
        "            float row_offset = abs(gl_FragCoord.y * windowScale - replaceColorMirrorHeight);"
        "            idx = int(mod(((row_offset + replaceColorMirrorHeight) / subpixelCnt -"
        "                floor(replaceColorSpeed * time)) / replaceColorbandHeight, replaceColorCnt));"
        "            FragColor = replaceColors[idx];"
        "            break;"
        "            case 2:"
        "            if (gl_FragCoord.y * windowScale > (fillLevel + 1) * subpixelCnt) {"
        "                FragColor = vec4(0.0f);"
        "            } else {"
        "                FragColor = replaceColors[0];"
        "            }"
        "            break;"
        "        }"
        "    }"
        "    else if (texColor.a == keyAlpha) {"
        "        vec2 pixel = floor(TexCoord * texSize);"
        "        float flicker = 0.4 * sin(200.0 * pixel.x + 300.0 * pixel.y + 30.0 * time)"
        "                      + 0.2 * sin(150.0 * pixel.x - 100.0 * pixel.y - 10.0 * time)"
        "                      + 0.3 * sin(-50.0 * pixel.x -  30.0 * pixel.y +  5.0 * time)"
        "                      - 0.5 * sin(-30.0 * pixel.x +  10.0 * pixel.y -  1.0 * time);"
        "        float alpha = 0.333333333333 * round(3.0 * (texColor.a + flicker));"
        "        FragColor = vec4(texColor.rgb, alpha);"
        "    } else {"
        "        FragColor = texColor;"
        "    }"
        "}";
    
    shader->program = wbShaderCreate(vertex_shader_src, fragment_shader_src);
    glUseProgram(shader->program); // Use the shader program

    for (int i = 0; i < WB_GRAPHIC_BATCH_CNT; i++) {
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 0] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 0;
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 1] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 1;
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 2] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 2;
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 3] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 2;
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 4] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 3;
        shader->indices[WB_GRAPHIC_SPRITE_INDICES_CNT * i + 5] = WB_GRAPHIC_SPRITE_VERTICES_CNT * i + 0;
    }

    shader->indices[0] = 0; shader->indices[1] = 1; shader->indices[2] = 2; // first triangle
    shader->indices[3] = 2; shader->indices[4] = 3; shader->indices[5] = 0; // second triangle

    glGenVertexArrays(1, &shader->vao);
    glGenBuffers(1, &shader->vbo);
    glGenBuffers(1, &shader->ebo);

    glBindVertexArray(shader->vao);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shader->vertices), shader->vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shader->indices), shader->indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texcoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader->loc.key_color                   = glGetUniformLocation(shader->program, "keyColor");
    shader->loc.time                        = glGetUniformLocation(shader->program, "time");
    shader->loc.key_alpha                   = glGetUniformLocation(shader->program, "keyAlpha");
    shader->loc.tex_size                    = glGetUniformLocation(shader->program, "texSize");
    shader->loc.key_color_mode              = glGetUniformLocation(shader->program, "keyColorMode");
    shader->loc.window_scale                = glGetUniformLocation(shader->program, "windowScale");
    shader->loc.replace_color_mirror_height = glGetUniformLocation(shader->program, "replaceColorMirrorHeight");
    shader->loc.fill_level                  = glGetUniformLocation(shader->program, "fillLevel");
    shader->loc.replace_colorband_height    = glGetUniformLocation(shader->program, "replaceColorbandHeight");
    shader->loc.replace_color_speed         = glGetUniformLocation(shader->program, "replaceColorSpeed");
    shader->loc.replace_color_cnt           = glGetUniformLocation(shader->program, "replaceColorCnt");
    shader->loc.replace_colors              = glGetUniformLocation(shader->program, "replaceColors");
    shader->loc.subpixel_cnt                = glGetUniformLocation(shader->program, "subpixelCnt");

    glUniform4f(shader->loc.key_color, WB_GRAPHIC_KEY_COLOR_R, WB_GRAPHIC_KEY_COLOR_G, WB_GRAPHIC_KEY_COLOR_B, WB_GRAPHIC_KEY_COLOR_A);
    glUniform1f(shader->loc.key_alpha, -1.0);
    glUniform1f(shader->loc.tex_size, WB_GRAPHIC_MAP_DUST_SPRITE_SIZE);
    glUniform1f(shader->loc.replace_colorband_height, WB_GRAPHIC_TEXT_COLORBAND_HEIGHT);
    glUniform1f(shader->loc.subpixel_cnt, WB_GRAPHIC_SUBPIXEL_CNT);
}

void wbShaderUninit(WBShader* shader) {
    glDeleteVertexArrays(1, &shader->vao);
    glDeleteBuffers(1, &shader->vbo);
    glDeleteBuffers(1, &shader->ebo);
    glDeleteProgram(shader->program);
}