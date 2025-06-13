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
        "layout (location = 0) in vec2 pos;\n"
        "layout (location = 1) in vec2 tex;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "    TexCoord = tex;\n"
        "}\n";
    static const char* fragment_shader_src =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D texSampler;\n"
        "uniform vec4 keyColor;\n"
        "uniform vec4 replaceColor;\n"
        "uniform float frameCnt;\n"
        "uniform float keyAlpha;\n"
        "uniform float fps;\n"
        "uniform vec2 texSize;\n"
        "void main() {\n"
        "    vec4 texColor = texture(texSampler, TexCoord);\n"
        "    if (all(equal(texColor, keyColor))) {\n"
        "        FragColor = replaceColor;\n"
        "    }\n"
        "    else if (texColor.a == keyAlpha) {\n"
        "        vec2 pixel = floor(TexCoord * texSize);\n"
        "        float flicker = 0.4 * sin(200.0 * pixel.x + 300.0 * pixel.y + 30.0 / fps * frameCnt)\n"
        "                      + 0.2 * sin(150.0 * pixel.x - 100.0 * pixel.y - 10.0 / fps * frameCnt)\n"
        "                      + 0.3 * sin(-50.0 * pixel.x -  30.0 * pixel.y +  5.0 / fps * frameCnt)\n"
        "                      - 0.5 * sin(-30.0 * pixel.x +  10.0 * pixel.y -  1.0 / fps * frameCnt);\n"
        "        float alpha = 0.333333333333 * round(3.0 * (texColor.a + flicker));\n"
        "        FragColor = vec4(texColor.rgb, alpha);\n"
        "    } else {\n"
        "        FragColor = texColor;\n"
        "    }\n"
        "}\n";
    shader->program = wbShaderCreate(vertex_shader_src, fragment_shader_src);
    glUseProgram(shader->program); // Use the shader program

    shader->indices[0] = 0; shader->indices[1] = 1; shader->indices[2] = 2; // first triangle
    shader->indices[3] = 2; shader->indices[4] = 3; shader->indices[5] = 0; // second triangle

    glGenVertexArrays(1, &shader->vao);
    glGenBuffers(1, &shader->vbo);
    glGenBuffers(1, &shader->ebo);

    glBindVertexArray(shader->vao);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shader->vertices), shader->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shader->indices), shader->indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texcoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}