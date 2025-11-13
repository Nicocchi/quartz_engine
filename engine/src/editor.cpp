#include "game.hpp"

static bool opt_fullscreen = true;
static bool opt_padding = false;
static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

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
}

bool open = true;

ImVec4 Vec4ToImVec4(const Vector4& v) {
    return ImVec4(v.x, v.y, v.z, v.w);
}

Vector4 ImVec4ToVec4(const ImVec4& v) {
    return Vector4{v.x, v.y, v.z, v.w };
}

void show_editor(render_context *context, int fps, game_memory *GM)
{
    game_state *state = (game_state*)GM->storage;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Scene Hierarchy");

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    static int selection_mask = (1 << 2);
    int node_clicked = -1;
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags flag_selected = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags flag_unselected = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

    for (int i = 0; i < state->entities.size(); i++)
    {
        bool isSelected = state->selected_entity == i;
        if (isSelected)
        {
            node_flags = flag_selected;
        }
        else
        {
            node_flags = flag_unselected;
        }

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)&state->entities[i], node_flags, state->entities[i].name, i);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            state->selected_entity = i;
        }

        if (node_open)
        {
            ImGui::TreePop();
        }
        
    }
    
    ImGui::End();

    ImGui::Begin("Inspector");
    if (state->entities.size() > 0)
    {
        ImGui::Text(state->entities[state->selected_entity].name);

        ImVec2 available_size = ImGui::GetContentRegionAvail();
        float w_width = available_size.x / 2.5;
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Position");
            ImGui::PushItemWidth(w_width);
            ImGui::DragFloat("##Entity X", &state->entities[state->selected_entity].transform.position.x, 1.0f, 0.0f, 0.0f, "X: %.02f");
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            ImGui::PushItemWidth(w_width);
            ImGui::DragFloat("##Entity Y", &state->entities[state->selected_entity].transform.position.y, 1.0f, 0.0f, 0.0f, "Y: %.02f");
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            ImGui::PushItemWidth(w_width - 8.0f);
            ImGui::PushID("ResetEntityPos");
            if (ImGui::Button("Reset"))
            {

            }
            ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::Text("Rotation");
            ImGui::PushItemWidth(available_size.x / 2.5);
            ImGui::DragFloat("##Entity Rotation", &state->entities[state->selected_entity].transform.rotation, 0.01f, -4.0f, 4.0f, "Rotation: %.02f");
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::PushItemWidth(w_width - 8.0f);
            ImGui::PushID("ResetEntityRotation");
            if (ImGui::Button("Reset"))
            {

            }
            ImGui::PopItemWidth();
            ImGui::PopID();
            // ImGui::Text("Scale");
            // ImGui::PushItemWidth(w_width);
            // ImGui::DragFloat("##Entity Scale X", &state->entities[state->selected_entity].transform.scale.x, 0.01f, 0.0f, 0.0f, "X: %.02f");
            // ImGui::PopItemWidth();
            // ImGui::SameLine();
            // ImGui::PushItemWidth(w_width - 8.0f);
            // ImGui::DragFloat("##Entity Scale Y", &state->entities[state->selected_entity].transform.scale.y, 0.01f, 0.0f, 0.0f, "Y: %.02f");
            // ImGui::PopItemWidth();

            ImGui::Text("Size");
            ImGui::PushItemWidth(w_width);
            ImGui::DragFloat("##Entity Size X", &state->entities[state->selected_entity].transform.size.x, 1.0f, 0.0f, 0.0f, "X: %.02f");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(w_width);
            ImGui::DragFloat("##Entity Size Y", &state->entities[state->selected_entity].transform.size.y, 1.0f, 0.0f, 0.0f, "Y: %.02f");
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            ImGui::PushItemWidth(w_width - 8.0f);
            ImGui::PushID("ResetEntitySize");
            if (ImGui::Button("Reset"))
            {

            }
            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {

            ImVec4 color = Vec4ToImVec4(state->entities[state->selected_entity].sprite.color);

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
            ImGui::SameLine();
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
                    state->entities[state->selected_entity].sprite.color = ImVec4ToVec4(color);
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
                        
                        // entity_color = ImVec4ToVec4(ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w))
                        color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!
                        state->entities[state->selected_entity].sprite.color = ImVec4ToVec4(color);
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
        }
    }


    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::AlignTextToFramePadding();
    bool enable_animated_sprite = true;

    bool as_open = ImGui::CollapsingHeader("##Animated Sprite", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
    ImGui::SameLine();
    ImGui::Checkbox("##EnableAnimatedSprite", &enable_animated_sprite);
    ImGui::SameLine();
    ImGui::Text("Animated Sprite");
    if (as_open)
    {
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        ImGui::Text("Current Frame: %d", state->entities[state->selected_entity].animated_sprite.frame_index);
            //ImGui::SameLine();
            //ImGui::PushItemWidth(available_size.x);
            //ImGui::DragInt("##Entity FrameIndex", &state->entities[state->selected_entity].animated_sprite.frame_index, 1.0f, 0.0f, state->entities[state->selected_entity].animated_sprite.max_frames - 1);

            ImGui::Text("Frame Duration");
            ImGui::PushItemWidth(available_size.x / 2.5f);
            ImGui::DragFloat("##Entity Frame Duration", &state->entities[state->selected_entity].animated_sprite.frame_duration, 0.01f, 0.0f, 1.0f, "%.02f");
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
            ImGui::DragInt("##Entity Start Frame", &state->entities[state->selected_entity].animated_sprite.start_frame, 1.0f, 0.0f, state->entities[state->selected_entity].animated_sprite.max_frames);
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
            ImGui::DragInt("##Entity End Frame", &state->entities[state->selected_entity].animated_sprite.end_frame, 0.01f, 0.0f, state->entities[state->selected_entity].animated_sprite.max_frames);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(available_size.x / 3.5f);
            ImGui::PushID("ResetAnimatedSpriteEnd");
            if (ImGui::Button("Reset"))
            {

            }
            ImGui::PopItemWidth();
            ImGui::PopID();
    }
    

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
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
    
    ImGui::End();

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

    ImGui::End();

    ImGui::Begin("Project");
    ImGui::Text("Project");
    ImGui::End();

    if (editor.firstTime)
    {
        editor.firstTime = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID right_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
        ImGuiID left_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, nullptr, &dockspace_id);
        ImGuiID bottom_node_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.35f, nullptr, &dockspace_id);
        ImGui::DockBuilderDockWindow("Scene Hierarchy", left_node_id);
        ImGui::DockBuilderDockWindow("Inspector", right_node_id);
        ImGui::DockBuilderDockWindow("Project", bottom_node_id);
        ImGui::DockBuilderDockWindow("Scene", dockspace_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    
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
