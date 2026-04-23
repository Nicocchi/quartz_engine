#define IMGUI_IMPLEMENTATION
#define IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#define IMGUI_DEFINE_MATH_OPERATORS
#include "editor.hpp"
#include "platform.hpp"
#include "save.hpp"
#include "game.hpp"

static bool opt_fullscreen = true;
static bool opt_padding = false;
static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;


// Iterates over a given directory and adds it to a list. This is to
// reduce calls to hit the os filesystem reads
void build_assets(const std::filesystem::path &root)
{
    if (content_browser.files.size() > 0)
    {
        content_browser.files.clear();
    }

    for (auto& directory_entry : std::filesystem::directory_iterator(root))
    {
        // const auto& path = directory_entry.path();
        auto relative_path = std::filesystem::relative(directory_entry.path(), root);
        // std::string filename_string = relative_path.filename().string();

        ANode node;
        node.path = directory_entry.path();
        node.filename = relative_path.filename().string();

        

        if (directory_entry.is_directory())
        {
            node.isDirectory = true;
        }

        if (directory_entry.path().extension() == ".png")
        {
            node.isDirectory = false;
            node.texture = new Texture2D();
            LoadTexture(node.texture, directory_entry.path().string().c_str(), true);
        }

        if (directory_entry.path().extension() == ".bin")
        {
            node.isDirectory = false;
        }

        content_browser.files.emplace_back(node);
    }
}

// Converts a std::filesystem string into a std::string
std::string canonical_to_string(const std::filesystem::path &p)
{
    try {
        return std::filesystem::weakly_canonical(p).string();
    } catch(...) {
        return p.lexically_normal().string();
    }
}



// Default theme
void SetupImGuiStyle()
{
	// Future Dark style by rewrking from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 1.0f;
	style.WindowPadding = ImVec2(12.0f, 12.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(20.0f, 20.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(6.0f, 6.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(12.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
	style.CellPadding = ImVec2(12.0f, 6.0f);
	style.IndentSpacing = 20.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 12.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 0.0f;
	//style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.27450982f, 0.31764707f, 0.4509804f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.23529412f, 0.21568628f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549f, 0.5529412f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.23529412f, 0.21568628f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.23529412f, 0.21568628f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.23529412f, 0.21568628f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.52156866f, 0.6f, 0.7019608f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.039215688f, 0.98039216f, 0.98039216f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901961f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.99607843f, 0.4745098f, 0.69803923f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.23529412f, 0.21568628f, 0.59607846f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f);
}

void init_editor(Window *window)
{
    // Editor newEditor;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    ImGui_ImplGlfw_InitForOpenGL(window->window, true);
    ImGui_ImplOpenGL3_Init(editor.glsl_version);
    editor.firstTime = true;
    editor.window = window;

    editor.auto_scroll = true;
    editor.prev_displayed_count = 0;
    editor.displayed_count = 0;
    editor.user_at_bottom = false;

    SetupImGuiStyle();

    // Content browser
    content_browser.asset_path = "assets";
    content_browser.current_directory = "assets";
    content_browser.current_filename = "assets";
    content_browser.padding = 16;
    content_browser.preview_size = 48;
    content_browser.thumbnail_size = 90;

    content_browser.folder_icon = new Texture2D();
    content_browser.file_icon = new Texture2D();
    LoadTexture(content_browser.folder_icon, "assets/editor/folder.png", true);
    LoadTexture(content_browser.file_icon, "assets/editor/file.png", true);

    build_assets("assets");
    editor.sprite_temp = new Texture2D();
    editor.tilemap_temp = new Texture2D();

}

bool open = true;

void AddLog(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    va_end(args);
    editor.log_messages.push_back(buf);
}

ImVec4 Vec4ToImVec4(const Vector4& v) {
    return ImVec4(v.x, v.y, v.z, v.w);
}

Vector4 ImVec4ToVec4(const ImVec4& v) {
    return Vector4{v.x, v.y, v.z, v.w };
}

Vector4 Vector4Transform(Vector4 v, Matrix m)
{
    Vector4 result;

    result.x = v.x*m.m0  + v.y*m.m4  + v.z*m.m8  + v.w*m.m12;
    result.y = v.x*m.m1  + v.y*m.m5  + v.z*m.m9  + v.w*m.m13;
    result.z = v.x*m.m2  + v.y*m.m6  + v.z*m.m10 + v.w*m.m14;
    result.w = v.x*m.m3  + v.y*m.m7  + v.z*m.m11 + v.w*m.m15;

    return result;
}

Vector2 ScreenToWorld(const Vector2& mouse, const Matrix& projection, int screenWidth, int screenHeight)
{
    // Convert screen → clip
    float x =  (2.0f * mouse.x) / screenWidth - 1.0f;
    float y = -(2.0f * mouse.y) / screenHeight + 1.0f; // ImGui has Y flipped

    Vector4 clip = { x, y, 0.0f, 1.0f };

    Matrix columnMajorVP = MatrixTranspose(projection);
    Matrix invViewProj = MatrixInvert(columnMajorVP);
    Vector4 world = Vector4Transform(clip, invViewProj);

    return { world.x, world.y };
}

void DrawTilemapGrid(Entity& entity, Matrix viewProjection, render_context *context)
{
    Vector2 tilemapPos = entity.transform.position;
    float cw = entity.tilemap.cell_width;
    float ch = entity.tilemap.cell_height;
    int gridStartX = (int)floor((context->camera->position.x - 1280/2 - tilemapPos.x) / cw) - 1;
    int gridEndX = (int)ceil((context->camera->position.x + 1280/2 - tilemapPos.x) / cw) + 1;
    int gridStartY = (int)floor((context->camera->position.y - 720/2 - tilemapPos.y) / ch) - 1;
    int gridEndY = (int)ceil((context->camera->position.y + 720/2 - tilemapPos.y) / ch) + 1;

    Vector4 gridColor = {0.3f, 0.3f, 0.3f, 0.5f};  // Semi-transparent gray

    // Draw vertical lines
    for (int col = gridStartX; col <= gridEndX; col++)
    {
        float x = tilemapPos.x + col * cw;
        float y1 = tilemapPos.y + gridStartY * ch;
        float y2 = tilemapPos.y + gridEndY * ch;

        render_command draw_line;
        draw_line.type = DRAW_LINE;
        // draw_line.id = entity.id;
        draw_line.color = gridColor;
        draw_line.line_pos_a.x = x;
        draw_line.line_pos_a.y = y1;
        draw_line.line_pos_b.x = x;
        draw_line.line_pos_b.y = y2;
        context->editor_render_commands.push(draw_line);
    }

    // Draw horizontal lines
    for (int row = gridStartY; row <= gridEndY; row++)
    {
        float y = tilemapPos.y + row * ch;
        float x1 = tilemapPos.x + gridStartX * cw;
        float x2 = tilemapPos.x + gridEndX * cw;

        render_command draw_line;
        draw_line.type = DRAW_LINE;
        // draw_line.id = entity.id;
        draw_line.color = gridColor;
        draw_line.line_pos_a.x = x1;
        draw_line.line_pos_a.y = y;
        draw_line.line_pos_b.x = x2;
        draw_line.line_pos_b.y = y;
        context->editor_render_commands.push(draw_line);
    }
}

void process_editor_input(render_context *render_state, input_state *Input, float dt)
{
    float velocity = 50 * dt;

    if (Input->keys[GLFW_KEY_A])
    {
        render_state->camera->position.x -= velocity;
    }

    if (Input->keys[GLFW_KEY_D])
    {
        render_state->camera->position.x += velocity;
    }

    if (Input->keys[GLFW_KEY_W])
    {
        render_state->camera->position.y += velocity;
    }

    if (Input->keys[GLFW_KEY_S])
    {
        render_state->camera->position.y -= velocity;
    }
}

void draw_scene(game_state *state, render_context *context, input_state *Input, int fps)
{
    ImGui::Begin("Scene");

    // Get window size, aspect ratio and scale and position the image to preserve image aspect ratio
    ImVec2 availSize = ImGui::GetContentRegionAvail();

    float imageAspectRatio = (float)editor.window->vwidth / (float)editor.window->vheight;
    float windowAspectRatio = availSize.x / availSize.y;

    float scaledImageWidth;
    float scaledImageHeight;

    if (imageAspectRatio > windowAspectRatio)
    {
        scaledImageWidth = availSize.x;
        scaledImageHeight = availSize.x / imageAspectRatio;
    } else {
        scaledImageHeight = availSize.y;
        scaledImageWidth = availSize.y * imageAspectRatio;
    }

    ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
    float offsetX = (availSize.x - scaledImageWidth) * 0.5f;
    float offsetY = (availSize.y - scaledImageHeight) * 0.5f;

    ImGui::SetCursorScreenPos(ImVec2(cursorScreenPos.x + offsetX, cursorScreenPos.y + offsetY));

    ImGui::Image(
            context->fbo->texture,
            ImVec2(scaledImageWidth, scaledImageHeight),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

    ImVec2 imageMin = ImGui::GetItemRectMin();
    ImVec2 textPos = ImVec2(imageMin.x + 20, imageMin.y + 20);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    char buffer[64];
    snprintf(buffer, IM_ARRAYSIZE(buffer), "FPS: %d", fps);
    draw_list->AddText(textPos, IM_COL32(255, 255, 0, 255), buffer);

    if (editor.selected_tile >= 0)
    {
        if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            Vector2 mouse = { (float)Input->mx, (float)Input->my};

            ImVec2 imageMin = ImGui::GetItemRectMin();
            ImVec2 imageMax = ImGui::GetItemRectMax();
            float displayW = imageMax.x - imageMin.x;
            float displayH = imageMax.y - imageMin.y;

            float localX = mouse.x - imageMin.x;
            float localY = mouse.y - imageMin.y;

            float fboW = 1280;
            float fboH = 720;

            float fbMouseX = (localX / displayW) * fboW;
            float fbMouseY = (localY / displayH) * fboH;

            Vector2 fbMouse = { fbMouseX, fbMouseY };
            Vector2 worldPos = ScreenToWorld(fbMouse, context->projection, (int)fboW, (int)fboH);

            Vector2 tilemapPos = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.position;

            float cw = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_width;
            float ch = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_height;

            int col = (int)floor((worldPos.x - tilemapPos.x) / cw);
            int row = (int)floor((worldPos.y - tilemapPos.y) / ch);

            GridCoord key { col, row };

            Tile tiled;
            tiled.id = state->nextID++;
            Vector2 pos = {tilemapPos.x + col * cw, tilemapPos.y + row * ch};
            tiled.transform.position = pos;
            float size = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_width;
            tiled.transform.size = {cw, ch};
            tiled.transform.scale = {1, 1};
            tiled.u0 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[editor.selected_tile].u0;
            tiled.v0 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[editor.selected_tile].v0;
            tiled.u1 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[editor.selected_tile].u1;
            tiled.v1 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[editor.selected_tile].v1;
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.map[key] = tiled;
        }

        if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            Vector2 mouse = { (float)Input->mx, (float)Input->my};

            ImVec2 imageMin = ImGui::GetItemRectMin();
            ImVec2 imageMax = ImGui::GetItemRectMax();
            float displayW = imageMax.x - imageMin.x;
            float displayH = imageMax.y - imageMin.y;

            float localX = mouse.x - imageMin.x;
            float localY = mouse.y - imageMin.y;

            float fboW = 1280;
            float fboH = 720;

            float fbMouseX = (localX / displayW) * fboW;
            float fbMouseY = (localY / displayH) * fboH;

            Vector2 fbMouse = { fbMouseX, fbMouseY };
            Vector2 worldPos = ScreenToWorld(fbMouse, context->projection, (int)fboW, (int)fboH);

            Vector2 tilemapPos = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.position;

            float cw = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_width;
            float ch = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_height;

            int col = (int)floor((worldPos.x - tilemapPos.x) / cw);
            int row = (int)floor((worldPos.y - tilemapPos.y) / ch);

            GridCoord key { col, row };
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.map.erase(key);
        }
    }

    ImGui::End();
}

void draw_transform_hierarchy(game_state *state, int w_width, ImVec2 available_size)
{

    ImGui::Text("Position");
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity X", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.position.x, 1.0f, 0.0f, 0.0f, "X: %.02f");
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity Y", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.position.y, 1.0f, 0.0f, 0.0f, "Y: %.02f");
    ImGui::PopItemWidth();

    ImGui::Text("Rotation");
    ImGui::PushItemWidth(available_size.x / 2.5);
    ImGui::DragFloat("##Entity Rotation", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.rotation, 0.01f, -4.0f, 4.0f, "Rotation: %.02f");
    ImGui::PopItemWidth();

    ImGui::Text("Size");
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity Size X", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.size.x, 1.0f, 0.0f, 0.0f, "X: %.02f");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity Size Y", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.size.y, 1.0f, 0.0f, 0.0f, "Y: %.02f");
    ImGui::PopItemWidth();
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Base size of the entity");
    }

    ImGui::Text("Scale");
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity Scale X", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.scale.x, 1.0f, 0.0f, 0.0f, "X: %.02f");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##Entity Scale Y", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].transform.scale.y, 1.0f, 0.0f, 0.0f, "Y: %.02f");
    ImGui::PopItemWidth();
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Scale of the entity");
    }
}

void draw_sprite_hierarchy(game_state *state, int w_width, ImVec2 available_size)
{
    ImVec4 color = Vec4ToImVec4(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.color);
    ImGuiColorEditFlags base_flags = ImGuiColorEditFlags_None;
    static bool saved_palette_init = true;
    static ImVec4 saved_palette[32] = {};
    static ImVec4 backup_color;

    // Generate a default palette. The palette will persist and can be edited.
    if (saved_palette_init)
    {
        for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
        {
            ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
            saved_palette[n].w = 1.0f; // Alpha
        }
        saved_palette_init = false;
    }

    ImGui::Text("Sprite Color");
    bool open_popup = ImGui::ColorButton("MyColor##3b", color, base_flags);
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    open_popup |= ImGui::Button("Palette");
    bool popup_opened_last_frame = false;

    if (open_popup && !popup_opened_last_frame)
    {
        ImGui::OpenPopup("mypicker");
        backup_color = color;
    }
    popup_opened_last_frame = open_popup;
    if (ImGui::BeginPopup("mypicker"))
    {
        ImGui::Text("Entity Color Picker");
        ImGui::Separator();
        if (ImGui::ColorPicker4("##picker", (float*)&color, base_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaBar))
        {
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.color = ImVec4ToVec4(color);
        }
        ImGui::SameLine();

        ImGui::BeginGroup(); // Lock X position
        ImGui::Text("Current");
        ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
        ImGui::Text("Previous");
        ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
        ImGui::Separator();
        ImGui::Text("Palette");
        for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
        {
            ImGui::PushID(n);
            if ((n % 8) != 0)
            {
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
            }

            ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
            if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
            {

                color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!
                state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.color = ImVec4ToVec4(color);
            }

            // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
            // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                {
                    memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                }
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                {
                    memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PopID();
        }
        ImGui::EndGroup();
        ImGui::EndPopup();
    }


    ImGui::Text("Texture");
    if (editor.sprite_temp != nullptr)
    {
        ImGui::Image(editor.sprite_temp->ID, ImVec2(100, 100));
    }
    if (ImGui::IsItemHovered())
    {
        // ImGui::SetTooltip(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->path);
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char *payload_data = static_cast<const char*>(payload->Data);
            std::string path = payload_data;
            LoadTexture(editor.sprite_temp, path.c_str());
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.filename = path;
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture = editor.sprite_temp;
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::Text("Z-Index");
    ImGui::PushItemWidth(w_width);
    ImGui::DragInt("##Entity z-index", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.z_index);
    // ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushID("ResetSpriteZIndex");
    if (ImGui::Button("Reset"))
    {

    }
    ImGui::PopItemWidth();
    ImGui::PopID();

    ImGui::Checkbox("Flip Horizontal", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.flip_x);
    ImGui::Checkbox("Flip Vertical", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.flip_y);
}

void draw_asprite_hierarchy(game_state *state)
{
    ImVec2 available_size = ImGui::GetContentRegionAvail();
    ImGui::Text("Current Frame: %d", state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.frame_index);
        //ImGui::SameLine();
        //ImGui::PushItemWidth(available_size.x);
        //ImGui::DragInt("##Entity FrameIndex", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.frame_index, 1.0f, 0.0f, game_scene->entities[game_scene->selected_entity].animated_sprite.max_frames - 1);

        ImGui::Text("Frame Duration");
        ImGui::PushItemWidth(available_size.x / 2.5f);
        ImGui::DragFloat("##Entity Frame Duration", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.frame_duration, 0.01f, 0.0f, 1.0f, "%.02f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 2.5f);
        ImGui::PushID("ResetAnimatedSpriteDuration");
        if (ImGui::Button("Reset"))
        {

        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        ImGui::Text("Frame Range");
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##Entity Start Frame", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.start_frame, 1, 0, state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.max_frames);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::PushID("ResetAnimatedSpriteStart");
        if (ImGui::Button("Reset"))
        {

        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##Entity End Frame", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.end_frame, 1, 0, state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.max_frames);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::PushID("ResetAnimatedSpriteEnd");
        if (ImGui::Button("Reset"))
        {

        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        ImGui::Text("Cell Size");
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##EntityCellSizeX", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.cell_width, 1, 0, 0,"X: %d");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##EntityCellSizeY", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.cell_height, 1, 0, 0, "Y: %d");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::PushID("ResetEntityCellSize");
        if (ImGui::Button("Reset"))
        {

        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        ImGui::Text("Texture Size");
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##EntityTextureSizeX", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.texture_width, 1, 0, 0,"W: %d");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::DragInt("##EntityTextureSizeY", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].animated_sprite.texture_height, 1, 0, 0, "H: %d");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(available_size.x / 3.5f);
        ImGui::PushID("ResetEntityTextureSize");
        if (ImGui::Button("Reset"))
        {

        }
        ImGui::PopItemWidth();
        ImGui::PopID();
}

void draw_tilemap_hierarchy(game_state *state, int w_width, ImVec2 available_size)
{
    ImGui::Text("Texture");
    if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture)
    {
        ImGui::Image(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->ID, 
            ImVec2(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->width * 0.25f, 
                state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->height * 0.25f));
    }
    else {
        ImGui::Text("Image: No tilemap texture loaded");
    }
    if (ImGui::IsItemHovered())
    {
        // ImGui::SetTooltip(game_scene->entities[game_scene->selected_entity].sprite.texture->path);
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const char *payload_data = static_cast<const char*>(payload->Data);
            std::string path = payload_data;
            LoadTexture(editor.tilemap_temp, path.c_str());
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.filename = path.c_str();
            state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture = editor.tilemap_temp;
        }

        ImGui::EndDragDropTarget();
    }

    ImGui::Text("Cell Size");
    ImGui::PushItemWidth(w_width);
    ImGui::DragInt("##Entity Tilemap cell_width", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_width);
    ImGui::SameLine();
    ImGui::DragInt("##Entity Tilemap cell_height", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.cell_height);
    ImGui::PopItemWidth();

    ImGui::Text("Texture Size");
    ImGui::PushItemWidth(w_width);
    ImGui::DragInt("##Entity Tilemap texture_width", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.texture_width);
    ImGui::SameLine();
    ImGui::DragInt("##Entity Tilemap texture_height", &state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.texture_height);
    ImGui::PopItemWidth();

    ImGui::Checkbox("Show Grid", &editor.draw_grid);

    float cell_size = content_browser.preview_size + content_browser.padding;
    float panel_width = ImGui::GetContentRegionAvail().x;
    int column_count = (int)(panel_width / cell_size);

    ImGui::SliderInt("##Tile Preview Size", &content_browser.preview_size, 16, 512);
    ImGui::SliderInt("##Tile Padding Size", &content_browser.padding, 0, 32);
    if (ImGui::CollapsingHeader("Tilemap Available Tiles"))
    {
        ImGui::Columns(column_count, 0, false);
        for (int i = 0; i < state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles.size(); i++)
        {
            float u0 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[i].u0;
            float v0 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[i].v0;
            float u1 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[i].u1;
            float v1 = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[i].v1;

            ImGui::PushID(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.tiles[i].id + 7000);
            if (i == editor.selected_tile)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 1));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            }
            if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture)
            {
                if (ImGui::ImageButton("tile_btn", state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->ID,
                    ImVec2(content_browser.preview_size, content_browser.preview_size), ImVec2(u0, v0), ImVec2(u1, v1)))
                {
                    editor.selected_tile = i;
                }
            }
            else {
                ImGui::Text("Button: No tilemap texture loaded");
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
            ImGui::NextColumn();

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Index: %d", i);
            }
        }
        ImGui::Columns(1);
    }


    if (ImGui::CollapsingHeader("Tilemap Used Tiles"))
    {
        ImGui::Columns(column_count, 0, false);
        int tile_count = 0;
        for (auto& pair : state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].tilemap.map)
        {
            Tile &tile = pair.second;

            float u0 = tile.u0;
            float v0 = tile.v0;
            float u1 = tile.u1;
            float v1 = tile.v1;

            ImGui::PushID(tile.id);
            if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture)
            {
                if (ImGui::ImageButton("tile_btn", state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].sprite.texture->ID,
                    ImVec2(content_browser.preview_size, content_browser.preview_size), ImVec2(u0, v0), ImVec2(u1, v1)))
                {
                    editor.selected_tile = tile_count;
                }
            }
            else {
                ImGui::Text("No tilemap texture loaded");
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Index: %d", tile_count);
            }
            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }
}

void draw_camera_hierarchy(render_context *context)
{
    // ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float w_width = available_size.x * 0.5f;
    ImGui::PushItemWidth(w_width);
    ImGui::DragFloat("##X", &context->camera->position.x, 0.1f, 0.0f, 0.0f, "X: %.02f");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(w_width - 8.0f);
    ImGui::DragFloat("##Y", &context->camera->position.y, 0.1f, 0.0f, 0.0f, "Y: %.02f");
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(available_size.x);
    ImGui::DragFloat("##Camera Zoom", &context->camera->zoom, 0.1f, 0.0f, 1000.0f, "Zoom: %.02f");
    ImGui::DragFloat("##Camera Rotation", &context->camera->rotation, 0.01f, -4.0f, 4.0f, "Rotation: %.02f");
    ImGui::PopItemWidth();
}

void draw_inspector(game_state *state, render_context *context)
{
    ImGui::Begin("Inspector");
    {
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        float w_width = available_size.x / 2.5;
        if (state->scenes[state->current_scene].entities.size() > 0)
        {
            std::string entity_uuid = state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].uuid.str();
            ImGui::Text(entity_uuid.c_str());
            
            ImGui::Text(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].name.c_str());
    
            // ============ ENTITY TRANSFORM ============
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                draw_transform_hierarchy(state, w_width, available_size);
            }
    
    
            // ============ ENTITY TILEMAP ============
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].flags & ENTITY_TILEMAP)
            {
                // TODO: Place into it's own window
                if (ImGui::CollapsingHeader("Tilemap", ImGuiTreeNodeFlags_DefaultOpen))
                {
    
                    draw_tilemap_hierarchy(state, w_width, available_size);
                }
    
            }
    
            // ============ ENTITY SPRITE ============
            if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].flags & ENTITY_SPRITE)
            {
                if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
                {
    
                    draw_sprite_hierarchy(state, w_width, available_size);
                }
            }
    
            // ============ ENTITY ANIMATED SPRITE ============
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::AlignTextToFramePadding();
    
            bool enable_animated_sprite = true;
            if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].flags & ENTITY_ANIMATED_SPRITE)
            {
                bool as_open = ImGui::CollapsingHeader("##Animated Sprite", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
                ImGui::SameLine();
                ImGui::Checkbox("##EnableAnimatedSprite", &enable_animated_sprite);
                ImGui::SameLine();
                ImGui::Text("Animated Sprite");
                if (as_open)
                {
                    draw_asprite_hierarchy(state);
                }
            }
        }
    
        // ============ CAMERA ============
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            draw_camera_hierarchy(context);
        }
    
        ImGui::End();
    }
    
}

void draw_hierarchy(game_state *state, render_context *context)
{
    ImGui::Begin("Scene Hierarchy");
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        
        static int selection_mask = (1 << 2);
        int node_clicked = -1;
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
        ImGuiTreeNodeFlags flag_selected = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
        ImGuiTreeNodeFlags flag_unselected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;;
        
        for (int i = 0; i < state->scenes[state->current_scene].entities.size(); i++)
        {
            bool isSelected = state->scenes[state->current_scene].selected_entity == i;
            if (isSelected)
            {
                node_flags = flag_selected;
                if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].flags & ENTITY_SPRITE)
                {
                    editor.sprite_temp = state->scenes[state->current_scene].entities[i].sprite.texture;
                    editor.selected_tile = -1;
                }
                else
                {
                    editor.sprite_temp = nullptr;
                }
                if (state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity].flags & ENTITY_TILEMAP)
                {
                    editor.tilemap_temp = state->scenes[state->current_scene].entities[i].sprite.texture;
                    if (editor.draw_grid)
                    {
                        DrawTilemapGrid(state->scenes[state->current_scene].entities[state->scenes[state->current_scene].selected_entity], context->projection, context);
                    }
                }
                else
                {
                    editor.tilemap_temp = nullptr;
                    editor.selected_tile = -1;
                }
            }
            else
            {
                node_flags = flag_unselected;
            }
        
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 10.0f));
            if (isSelected)
            {
                ImU32 col = ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
                ImGui::PushStyleColor(ImGuiCol_Header, col);
            }
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)&state->scenes[state->current_scene].entities[i], node_flags, state->scenes[state->current_scene].entities[i].name.c_str(), i);
        
            if (isSelected)
            {
                ImGui::PopStyleColor();
            }
            ImGui::PopStyleVar();
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                state->scenes[state->current_scene].selected_entity = i;
            }
        
            if (node_open)
            {
                ImGui::TreePop();
            }
        
        
        }
        
    }
    ImGui::End();
}

void draw_browser(game_state *state, render_context *context, int fps)
{
    ImGui::Begin("Content Browser");
    ImGui::BeginChild("##cb_topbar", ImVec2(0, 34), false);
    bool disabled = content_browser.current_directory != std::filesystem::path(content_browser.asset_path);
    if (!disabled)
    {
        ImGui::BeginDisabled(true);
    }

    if (ImGui::Button("<-"))
    {
        content_browser.current_directory = content_browser.current_directory.parent_path();
        content_browser.current_filename = content_browser.current_directory.string();
        build_assets(content_browser.current_directory);
    }
    if (!disabled)
    {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();

    ImGui::TextUnformatted(canonical_to_string(content_browser.current_filename).c_str());
    ImGui::EndChild();

    ImVec2 parentContentSize = ImGui::GetContentRegionAvail();
    float childHeight = parentContentSize.y * 0.8f;
    ImGui::BeginChild("##cb_content", ImVec2(0, childHeight), false);

    float cell_size = content_browser.thumbnail_size + content_browser.padding;
    float panel_width = ImGui::GetContentRegionAvail().x;
    int column_count = (int)(panel_width / cell_size);

    if (column_count < 1)
    {
        column_count = 1;
    }

    ImGui::Columns(column_count, 0, false);

    for (int i = 0; i < content_browser.files.size(); i++)
    {
        Texture2D *icon = content_browser.files[i].isDirectory ? content_browser.folder_icon : content_browser.file_icon;
        if (content_browser.files[i].texture != nullptr)
        {
            icon = content_browser.files[i].texture;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton(content_browser.files[i].path.string().c_str(), icon->ID, ImVec2(content_browser.thumbnail_size, content_browser.thumbnail_size),
                            ImVec2(0, 1), ImVec2(1,0));


        if (ImGui::BeginDragDropSource())
        {
            // const wchar_t *item_path = relative_path.c_str();
            std::string rel = content_browser.files[i].path.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", rel.c_str(), (rel.size() + 1) * sizeof(char), ImGuiCond_Once);

            ImVec2 mousePos = ImGui::GetIO().MousePos;
            ImVec2 imageSize = ImVec2(50, 50);
            ImVec2 tooltipPos = ImVec2(mousePos.x - imageSize.x * 0.8f, mousePos.y - imageSize.y * 0.8f);

            ImGui::SetNextWindowPos(tooltipPos);
            if (ImGui::BeginTooltip())
            {
                ImGui::Image(icon->ID, imageSize);
                ImGui::EndTooltip();
            }

            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (content_browser.files[i].isDirectory)
            {
                content_browser.current_directory /= content_browser.files[i].path.filename();
                content_browser.current_filename = content_browser.files[i].filename;
                build_assets(content_browser.current_directory);
            } else if (content_browser.files[i].path.extension() == ".bin")
            {
                size_t lastindex = content_browser.files[i].filename.find_last_of(".");
                std::string rawname = content_browser.files[i].filename.substr(0, lastindex);
                load_scene(state, rawname.c_str());
                context->camera->position.x = 0;
                context->camera->position.y = 0;
            }
        }


        ImGui::TextWrapped(content_browser.files[i].filename.c_str());
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    ImGui::SetCursorPosY(parentContentSize.y + 40);
    ImGui::BeginChild("##cb_statusbar", ImVec2(0, 34), false);

    ImGui::Text("Thumbnail Size");
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGui::SliderInt("##Thumbnail Size", &content_browser.thumbnail_size, 64, 512);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text("Padding");
    ImGui::PushItemWidth(200);
    ImGui::SameLine();
    ImGui::SliderInt("##Padding", &content_browser.padding, 0, 32);
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::End();
}

void draw_console()
{
    ImGui::Begin("Console", nullptr);
    {
        static char filter_buffer[256] = "";
        ImGui::Checkbox("Auto Scroll", &editor.auto_scroll);
        ImGui::SameLine();
        ImGui::InputText("Filter", filter_buffer, IM_ARRAYSIZE(filter_buffer));
        ImGui::Separator();
    
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -5), ImGuiChildFlags_NavFlattened | 
                        ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
            // Track how many messages are actually shown (after filter)
            for (const auto& msg : game_logs.messages)
            {
                if (strlen(filter_buffer) == 0 || strstr(msg.c_str(), filter_buffer) != nullptr)
                {
                    ImGui::TextUnformatted(msg.c_str());
                    ++editor.displayed_count;
                }
            }
        
            editor.user_at_bottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f);
        
            if (editor.auto_scroll && (editor.user_at_bottom || editor.displayed_count > editor.prev_displayed_count))
            {
                ImGui::SetScrollHereY(1.0f);
            }
        
            editor.prev_displayed_count = editor.displayed_count;
        
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void draw_statusbar(int fps)
{
    ImGuiIO& io = ImGui::GetIO();
    // Set position to bottom-left, with a small offset for padding
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - ImGui::GetFrameHeightWithSpacing()), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, ImGui::GetFrameHeightWithSpacing()), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("##StatusBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        char buffer[256];
        snprintf(buffer, IM_ARRAYSIZE(buffer), "FPS: %d", fps);
        ImGui::Text(" ");
        ImGui::SameLine();

        float textWidth = ImGui::CalcTextSize(buffer).x;
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;

        float posX = ImGui::GetCursorPosX() + availableWidth - textWidth - itemSpacing;

        if (posX > ImGui::GetCursorPosX())
        {
            ImGui::SetCursorPosX(posX);
        }
        ImGui::Text(buffer);
    }
    ImGui::End();
}

void draw_popups(game_state *state, render_context *context)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (editor.save_modal_popup)
    {
        ImGui::OpenPopup("SaveScene");
        editor.save_modal_popup = false;
    }
    if (ImGui::BeginPopupModal("SaveScene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Save?");
        ImGui::Separator();

        
        ImGui::InputText("Filename", &editor.save_name_buffer);

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            printf("%s\n", editor.save_name_buffer.c_str());
            if (!editor.save_name_buffer.empty())
            {
                export_scene(state, editor.save_name_buffer.c_str());
                build_assets(content_browser.current_directory);
            }
            editor.save_name_buffer = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (editor.load_modal_popup)
    {
        ImGui::OpenPopup("LoadScene");
        editor.load_modal_popup = false;
    }
    if (ImGui::BeginPopupModal("LoadScene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Load?");
        ImGui::Separator();

        
        ImGui::InputText("Filename", &editor.save_name_buffer);

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            printf("%s\n", editor.save_name_buffer.c_str());
            if (!editor.save_name_buffer.empty())
            {
                if (load_scene(state, editor.save_name_buffer.c_str()))
                {
                    context->camera->position.x = 0;
                    context->camera->position.y = 0;
                    printf("Scene loaded");
                } else {
                    printf("Failed to laod scene: %s", editor.save_name_buffer.c_str());
                }
            }
            editor.save_name_buffer = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void show_editor(render_context *context, int fps, input_state *Input, game_memory *GM, Window *window)
{
    game_state *state = (game_state*)GM->storage;
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // MENU BAR
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load"))
            {
                editor.load_modal_popup = true;
            }
            if (ImGui::MenuItem("Save As.."))
            {
                editor.save_modal_popup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // DOCKSPACE WINDOW
    float statusBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + statusBarHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - statusBarHeight - 25));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags dockspaceFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDecoration;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,  ImVec2(0.0f, 0.0f));

    ImGui::Begin("##DockspaceWindow", nullptr, dockspaceFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    draw_hierarchy(state, context);

    draw_inspector(state, context);

    draw_scene(state, context, Input, fps);

    draw_browser(state, context, fps);

    draw_console();

    draw_statusbar(fps);

    ImGui::PopStyleVar(2);

    if (editor.firstTime)
    {
        editor.firstTime = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID right_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
        ImGuiID left_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, nullptr, &dockspace_id);
        ImGuiID bottom_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.35f, nullptr, &dockspace_id);
        ImGuiID console_right_node_id = ImGui::DockBuilderSplitNode(bottom_node_id, ImGuiDir_Right, 0.50f, nullptr, &bottom_node_id);
        ImGuiID scene_dock_id = ImGui::GetWindowDockID();
        ImGuiID scene_dock_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &scene_dock_left);

        ImGui::DockBuilderDockWindow("Scene Hierarchy", scene_dock_left);
        ImGui::DockBuilderDockWindow("Inspector", right_node_id);
        ImGui::DockBuilderDockWindow("Content Browser", bottom_node_id);
        ImGui::DockBuilderDockWindow("Console", console_right_node_id);
        ImGui::DockBuilderDockWindow("Scene", dockspace_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::End();

    draw_popups(state, context);

    ImGui::Render();
    glfwGetFramebufferSize(editor.window->window, &editor.window->width, &editor.window->height);
    glViewport(0, 0, editor.window->width, editor.window->height);
    glClearColor(editor.color.x * editor.color.w, editor.color.y * editor.color.w, editor.color.z * editor.color.w, editor.color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void close_editor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
