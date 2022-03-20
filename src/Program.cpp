#include "Program.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <time.h>

constexpr double MY_PI = 3.1415926;

glm::mat4 GetViewMatrix(glm::vec3 eyePos)
{
    glm::mat4 view(1.0f);
    glm::mat4 translate(1.0f);

    translate[3] = { -eyePos[0], -eyePos[1], -eyePos[2], 1.0f };
    view = translate * view;
    return view;
}

glm::mat4 GetModelMatrix(float rotation_angle)
{
    glm::mat4 rotation(1.0f);
    float c = cos((rotation_angle * MY_PI) / 180.0f);
    float s = sin((rotation_angle * MY_PI) / 180.0f);

    rotation[0] = { c, 0, -s, 0 };
    rotation[1] = { 0, 1.0f, 0, 0 };
    rotation[2] = { s, 0, c, 0 };
    rotation[3] = { 0, 0, 0, 1.0f };

    glm::mat4 scale(1.0f);
    scale[0][0] = 2.5f;
    scale[1][1] = 2.5f;
    scale[2][2] = 2.5f;

    //glm::mat4 translate(1.0f);
    //translate[3] = glm::vec4(transform, 1);


    return rotation * scale;
}

glm::mat4 GetProjectionMatrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    // Students will implement this function

    glm::mat4 projection(1.0f);

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    glm::mat4 projectionToOrtho;
    float n = zNear;
    float f = zFar;
    float t = tan(eye_fov / 2.0f * MY_PI / 180.0f) * abs(n);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    projectionToOrtho[0] = { n, 0, 0, 0 };
    projectionToOrtho[1] = { 0, n, 0, 0 };
    projectionToOrtho[2] = { 0, 0, n + f, 1.0f };
    projectionToOrtho[3] = { 0, 0, -n * f, 0 };

    glm::mat4 orthoTranslate(1.0f);
    glm::mat4 orthoLinear;

    orthoLinear[0] = { 2.0f / (l - r), 0,              0,              0 };
    orthoLinear[1] = { 0,              2.0f / (t - b), 0,              0 };
    orthoLinear[2] = { 0,              0,              2.0f / (n - f), 0 };
    orthoLinear[3] = { 0,              0,              0,              1 };

    orthoTranslate[3] = { -(r + l) / 2.0f, -(t + b) / 2.0f, -(n + f) / 2.0f, 1.0f };
    glm::mat4 ortho = orthoLinear * orthoTranslate;
    projection = ortho * projectionToOrtho;
    // Then return it.
    return projection;
}

glm::mat4 GetViewportMatrix(int width, int height)
{
    int w = width;
    int h = height;
    glm::mat4 viewportMatrix(1.0f);
    viewportMatrix[0] = { w / 2.0f, 0, 0, 0 };
    viewportMatrix[1] = { 0, h / 2.0f, 0, 0 };
    viewportMatrix[2] = { 0, 0, 1.0f, 0 };
    viewportMatrix[3] = { w / 2.0f, h / 2.0f, 0, 1.0f };
    return viewportMatrix;
}


int Program::Init()
{
    glfwInit();
    w = 1400;
    h = 1400;
    window = glfwCreateWindow(1400, 1400, "OpenGL Boilerplate", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    InitGUI();
    InitComputeShader();
    InitFrameBuffer(w, h);
    InitSSBO();
    InitModes();
    LoadTexture(Textures::DiffuseTexture, "soulspear\\soulspear_diffuse.tga");
    LoadTexture(Textures::NormalTexture, "soulspear\\soulspear_normal.tga");
    LoadTexture(Textures::SpecularTexture, "soulspear\\soulspear_specular.tga");

    Light l1 = Light{ {20, 20, 20}, {500, 500, 500} };
    Light l2 = Light{ {-20, 20, 0}, {500, 500, 500} };
    lights.push_back(l1);
    lights.push_back(l2);
    glfwSwapInterval(1);
    return 0;
}

void Program::Update()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            angles -= 5;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            angles += 5;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            transform.z -= 1;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            transform.z += 1;
        }


        Draw();
        glfwSwapBuffers(window);
    }
}

void Program::End()
{
    glfwTerminate();
    for (auto d : textures)
    {
        glDeleteTextures(1, &d.second);
    }
    textures.clear();

    for (auto d : shaders)
    {
        glDeleteShader(d.second);
    }
    shaders.clear();

    for (auto d : buffers)
    {
        glDeleteBuffers(1, &d.second);
    }
    buffers.clear();

    for (auto d : programs)
    {
        glDeleteProgram(d.second);
    }
    programs.clear();

    //glDeleteProgram()
    EndGUI();
}

void Program::Draw()
{
    Clear();
    Render();
    UpdateTexture();
}

void Program::InitShader()
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();

    std::string vertexSource = LoadFileAsString("Plain.vert");
    const char* vertexC = vertexSource.c_str();
    int vertexSourceLength = (int)vertexSource.size();
    glShaderSource(vertexShader, 1, &vertexC, &vertexSourceLength);
    glCompileShader(vertexShader);


    std::string fragmentSource = LoadFileAsString("Plain.frag");
    const char* fragmentC = fragmentSource.c_str();
    int fragmentShaderLength = (int)fragmentSource.size();
    glShaderSource(fragmentShader, 1, &fragmentC, &fragmentShaderLength);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    shaders[Shaders::Vertex] = vertexShader;
    shaders[Shaders::Fragment] = fragmentShader;
    programs[Programs::ShaderProgram] = shaderProgram;
}

void Program::InitComputeShader()
{
    {
        GLuint rendererShader;
        GLuint rendererProgram;
        rendererShader = glCreateShader(GL_COMPUTE_SHADER);
        std::string computeSource = LoadFileAsString("Renderer.comp");
        const char* computeC = computeSource.c_str();
        int computeSourceLength = (int)computeSource.size();
    
        glShaderSource(rendererShader, 1, &computeC, &computeSourceLength);
        rendererProgram = glCreateProgram();
        glAttachShader(rendererProgram, rendererShader);
        glLinkProgram(rendererProgram);

        shaders[Shaders::Renderer] = rendererShader;
        programs[Programs::Renderer] = rendererProgram;
    }

    {
        GLuint clearShader;
        GLuint clearProgram;
        clearShader = glCreateShader(GL_COMPUTE_SHADER);
        std::string clearSource = LoadFileAsString("Clear.comp");
        const char* clearC = clearSource.c_str();
        int computeSourceLength = (int)clearSource.size();

        glShaderSource(clearShader, 1, &clearC, &computeSourceLength);
        clearProgram = glCreateProgram();
        glAttachShader(clearProgram, clearShader);
        glLinkProgram(clearProgram);

        shaders[Shaders::Clear] = clearShader;
        programs[Programs::Clear] = clearProgram;
    }

}

void Program::InitQuad()
{
    GLuint quadBuffer;
    glGenBuffers(1, &quadBuffer);

    float vertexPositionData[] = {
        1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,

        -1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertexPositionData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    buffers[Buffers::Quad] = quadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Program::InitSSBO()
{
    GLuint vertexBuffer, indexBuffer, depthBuffer;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);
    glGenBuffers(1, &depthBuffer);

    buffers[Buffers::Vertex] = vertexBuffer;
    buffers[Buffers::Index] = indexBuffer;
    buffers[Buffers::Depth] = depthBuffer;

    //glUseProgram(clearProgram);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, depthBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Depth) * depthBufferV.size(), &depthBufferV[0], GL_STATIC_DRAW);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, depthBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Program::InitTexture()
{
    GLuint frameBufferTexture;
    glGenTextures(1, &frameBufferTexture);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, frameBuffer.data());
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    textures[Textures::FrameBufferTexture] = frameBufferTexture;
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Program::LoadTexture(Textures texture, std::string path)
{
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    textures[texture] = textureID;
    stbi_image_free(data);
}

void Program::UpdateTexture()
{
    GLuint shaderProgram = programs[Programs::ShaderProgram];
    GLuint frameBufferTexture = textures[Textures::FrameBufferTexture];
    GLuint quadBuffer = buffers[Buffers::Quad];
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "screen"), 0);
    //textures["screen"] = frameBufferTexture;

    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Program::InitFrameBuffer(int width, int height)
{
    this->w = width;
    this->h = height;
    frameBuffer.resize(w * h);
    std::fill(frameBuffer.begin(), frameBuffer.end(), glm::vec4(0,0,0,1));
    depthBufferV.resize(w * h);
    std::fill(depthBufferV.begin(), depthBufferV.end(), 1e9);
    InitTexture();
    InitQuad();
    InitShader();
}

void Program::InitModes()
{
    models.push_back(LoadModel("soulspear/soulspear.obj"));
}

void Program::SetPixel(int x, int y, glm::vec4 color)
{
    if (x < 0 || x >= w) return;
    if (y < 0 || y >= h) return;
    frameBuffer[y * w + x] = color;
}

void Program::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
    {
        GLuint clearProgram = programs[Programs::Clear];
        GLuint frameBufferTexture = textures[Textures::FrameBufferTexture];
        GLuint depthBuffer = buffers[Buffers::Depth];
        glUseProgram(clearProgram);
        int operations = w * h;
        int processes = 8 * 8;
        int dimension = ceil(sqrt(operations / processes));
        glUniform1i(glGetUniformLocation(clearProgram, "dimension"), dimension);
        glBindImageTexture(0, frameBufferTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, depthBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, depthBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glDispatchCompute(dimension, dimension, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
}

void Program::Render()
{
    GLuint rendererProgram = programs[Programs::Renderer];
    GLuint frameBufferTexture = textures[Textures::FrameBufferTexture];
    GLuint depthBuffer = buffers[Buffers::Depth];
    GLuint vertexBuffer = buffers[Buffers::Vertex];
    GLuint indexBuffer = buffers[Buffers::Index];
    GLuint diffuseTexture = textures[Textures::DiffuseTexture];
    GLuint normalTexture = textures[Textures::NormalTexture];
    GLuint specularTexture = textures[Textures::SpecularTexture];
    glUseProgram(rendererProgram);
    // this can be done in another compute shader
    glBindImageTexture(0, frameBufferTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glad_glBindTextureUnit(1, diffuseTexture);
    glUniform1i(glGetUniformLocation(rendererProgram, "diffuseTexture"), 1);

    glBindTexture(GL_TEXTURE_2D, specularTexture);
    glad_glBindTextureUnit(2, specularTexture);
    glUniform1i(glGetUniformLocation(rendererProgram, "specularTexture"), 2);

    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glad_glBindTextureUnit(3, normalTexture);
    glUniform1i(glGetUniformLocation(rendererProgram, "normalTexture"), 3);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (int i = 0; i < lights.size(); i++)
    {
        std::string s = "lights[";
        s += std::to_string(i);
        s += "].position";
        glUniform3fv(glGetUniformLocation(rendererProgram, s.c_str()), 1, &lights[i].position.x);
        s = "lights[";
        s += std::to_string(i);
        s += "].intensity";
        glUniform3fv(glGetUniformLocation(rendererProgram, s.c_str()), 1, &lights[i].intensity.x);
    }
    glUniform1i(glGetUniformLocation(rendererProgram, "lightCount"), lights.size());


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, depthBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, depthBuffer);

    for (int i = 0; i < models.size(); i++)
    {
        glm::vec3 eyePos = transform;
        glUniform3fv(glGetUniformLocation(rendererProgram, "eyePosition"), 1, &eyePos.x);

        glm::mat4 model = GetModelMatrix(angles);
        glUniformMatrix4fv(glGetUniformLocation(rendererProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glm::mat4 view = GetViewMatrix(eyePos);
        glUniformMatrix4fv(glGetUniformLocation(rendererProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glm::mat4 projection = GetProjectionMatrix(45, (float)w / (float)h, 0.1, 50);
        glUniformMatrix4fv(glGetUniformLocation(rendererProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        glm::ivec2 size = glm::ivec2(w, h);
        glUniform2iv(glGetUniformLocation(rendererProgram, "size"), 1, &size.x);

        for (int j = 0; j < models[i]->meshes.size(); j++)
        {
            //glBindImageTexture(1, depthBufferTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
            int operations = models[i]->meshes[j].indices.size() / 3;
            int processes = 8 * 8;
            int dimension = ceil(sqrt(operations / processes));
            //std::cout << operations << std::endl;
            glUniform1i(glGetUniformLocation(rendererProgram, "dimension"), dimension);

            //std::cout << dimension << std::endl;

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vertex) * models[i]->meshes[j].vertices.size(), &models[i]->meshes[j].vertices[0], GL_STATIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Index) * models[i]->meshes[j].indices.size(), &models[i]->meshes[j].indices[0], GL_STATIC_DRAW); //sizeof(data) only works for statically sized C/C++ arrays.

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vertexBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indexBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
            glDispatchCompute(dimension, dimension, 1);
        }
    }
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Program::InitGUI()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void Program::UpdateGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Make window dockable
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    //ImGui::Dock

    // begin imgui window
    ImGui::Begin("Imgui window");
    // draw ui element in between
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void Program::EndGUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
