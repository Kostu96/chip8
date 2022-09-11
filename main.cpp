#include "chip8.hpp"
#include "disassembler.hpp"
#include "shader_sources.inl"

#include <glad/gl.h>
#include <glw/glw.hpp>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>

constexpr u16 CHIP8_WIDTH = 64;
constexpr u16 CHIP8_HEIGHT = 32;
constexpr u16 SCREEN_SIZE = CHIP8_WIDTH * CHIP8_HEIGHT;
constexpr u16 ZOOM = 16;
constexpr u16 WINDOW_WIDTH = CHIP8_WIDTH * ZOOM;
constexpr u16 WINDOW_HEIGHT = CHIP8_HEIGHT * ZOOM;
constexpr u16 BORDER = 20;

#pragma region OGLStuff
glw::Shader* pointShader = nullptr;
struct CharVertex {
    u16 x, y;
    u32 color;
} charVertices[SCREEN_SIZE];
glw::VertexBuffer* charVBO = nullptr;
glw::VertexArray* charVAO = nullptr;

glw::Shader* textureShader = nullptr;
struct TextureVertex {
    f32 x, y;
    f32 u, v;
} textureVertices[] = {
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.0f, 1.f, 0.f,
     1.f,  1.f, 1.f, 1.f,
    -1.f,  1.f, 0.f, 1.f
};
u8 textureIndices[] = {
    0, 1, 2,
    2, 3, 0
};
glw::VertexBuffer* textureVBO = nullptr;
glw::IndexBuffer* textureIBO = nullptr;
glw::VertexArray* textureVAO = nullptr;

glw::Framebuffer* FBO = nullptr;

void init()
{
    glActiveTexture(GL_TEXTURE0);

    pointShader = new glw::Shader{};
    pointShader->createFromSource(pointVertSource, pointFragSource);
    textureShader = new glw::Shader{};
    textureShader->createFromSource(textureVertSource, textureFragSource);

    charVBO = new glw::VertexBuffer{ sizeof(charVertices) };
    glw::VertexLayout pointLayout{ {
        { glw::LayoutElement::DataType::U16_2, false },
        { glw::LayoutElement::DataType::U8_4, true, true }
    } };
    charVAO = new glw::VertexArray{ *charVBO, pointLayout };

    textureVBO = new glw::VertexBuffer{ textureVertices, sizeof(textureVertices) };
    textureIBO = new glw::IndexBuffer{ textureIndices, sizeof(textureIndices) / sizeof(u8), glw::IndexBuffer::IndexType::U8 };
    glw::VertexLayout texturelayout{ {
        { glw::LayoutElement::DataType::F32_2 },
        { glw::LayoutElement::DataType::F32_2 }
    } };
    textureVAO = new glw::VertexArray{ *textureVBO, texturelayout };
    textureVAO->setIndexBuffer(*textureIBO);

    for (u16 row = 0; row < CHIP8_HEIGHT; row++)
        for (u16 col = 0; col < CHIP8_WIDTH; col++)
        {
            charVertices[row * CHIP8_WIDTH + col].x = col;
            charVertices[row * CHIP8_WIDTH + col].y = row;
            charVertices[row * CHIP8_WIDTH + col].color = 0;
        }

    FBO = new glw::Framebuffer{
        glw::Framebuffer::Properties{ CHIP8_WIDTH, CHIP8_HEIGHT, 1, {
            glw::TextureSpecification{
                glw::TextureFormat::RGBA8,
                glw::TextureFilter::Nearest,
                glw::TextureFilter::Nearest,
                glw::TextureWrapMode::Clamp
            }
        }}
    };
}

void shutdown()
{
    delete FBO;

    delete textureVAO;
    delete textureIBO;
    delete textureVBO;

    delete charVAO;
    delete charVBO;

    delete textureShader;
    delete pointShader;
}
#pragma endregion

constexpr u8 CHARSET_SIZE = 80;
const u8 charset[CHARSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!\n";
        std::terminate();
    }
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH + BORDER * 2, WINDOW_HEIGHT + BORDER * 2, "Chip8", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!\n";
        std::terminate();
    }
    glfwMakeContextCurrent(window);
    glw::init(glfwGetProcAddress);
    init();

    u8 program[0x1000];
    for (u8 i = 0; i < CHARSET_SIZE; i++)
        program[i] = charset[i];

    std::ifstream fin{ "roms/Space Invaders [David Winter].ch8", std::ios::binary };
    fin.read((char*)(program + 0x200), 0xE00);
    fin.close();
    
    disassemble(program + 0x200);

    Chip8 chip8{};
    chip8.loadProgram(program);
    chip8.reset();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (u32 i = 0; i < 32; i++)
        {
            chip8.clock();
        }

        FBO->bind();
        charVAO->bind();
        pointShader->bind();
        glViewport(0, 0, CHIP8_WIDTH, CHIP8_HEIGHT);

        for (u16 row = 0; row < CHIP8_HEIGHT; row++)
            for (u16 col = 0; col < CHIP8_WIDTH; col++)
            {
                u16 index = row * CHIP8_WIDTH + col;
                charVertices[index].color = (chip8.getScreenMemory()[index] ? 0xFFFFFFFF : 0);
            }
        charVBO->bind();
        charVBO->setData(charVertices, sizeof(charVertices));
        glDrawArrays(GL_POINTS, 0, SCREEN_SIZE);

        FBO->unbind();
        textureVAO->bind();
        textureShader->bind();
        glViewport(BORDER, BORDER, WINDOW_WIDTH, WINDOW_HEIGHT);
        FBO->getAttachments()[0].bind(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        glFinish();
    }

    shutdown();
    glfwTerminate();
    return 0;
}
