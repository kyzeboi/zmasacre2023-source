/*

     ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#include "main.h"

#include "segue_font.h"
#include "kyze_funcs.h"


float accent_color[4] = { 0.745f, 0.151f, 0.151f, 1.000f };

bool active = false;

ImFont* ico = nullptr;
ImFont* ico_combo = nullptr;
ImFont* ico_button = nullptr;
ImFont* default_segu = nullptr;
ImFont* bold_segu = nullptr;
ImFont* segu = nullptr;

int tab_count = 0, tabs = 0;
int backgroundcolor = 0;
void CMenu::AddImGuiSettings()
{
    ImGuiIO& io = ImGui::GetIO();

    default_segu = io.Fonts->AddFontFromMemoryTTF(&seguoe, sizeof seguoe, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());

    bold_segu = io.Fonts->AddFontFromMemoryTTF(&bold_segue, sizeof bold_segue, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 24, NULL, io.Fonts->GetGlyphRangesCyrillic());

    segu = io.Fonts->AddFontFromMemoryTTF(&seguoe, sizeof seguoe, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico_combo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 19, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico_button = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());
}


void CMenu::RenderMenu()
{
    if (!ÝmMenu->show)
        return;

    ImGui::SetNextWindowSize(ImVec2(970, 580));
    ImGui::SetWindowPos(ImVec2(1500, 500));

    DWORD window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

    ImGui::Begin("kyze", &ÝmMenu->show, window_flags);
    {
        ImGui::StyleColorsKyze();

   

        const auto& p = ImGui::GetWindowPos();
        const auto& pWindowDrawList = ImGui::GetWindowDrawList();


        pWindowDrawList->AddText(bold_segu, 50.f, ImVec2(300.f + p.x, 2.000f + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.70f), "ZMASACRE MULTIHACK");
        pWindowDrawList->AddText(segu, 30.f, ImVec2(40.000f + p.x, 515.000f + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.50f), FILEVERSION);

        ImGui::SetCursorPosY(80);

        if (ImGui::TabButton("N", "Aim", ImVec2(190, 40)) && tab_count != 0) {
            tab_count = 0;
            active = true;
        }

        if (ImGui::TabButton("O", "Misc", ImVec2(190, 40)) && tab_count != 1) {
            tab_count = 1;
            active = true;
        }

        if (ImGui::TabButton("I", "Visuals", ImVec2(190, 40)) && tab_count != 2) {
            tab_count = 2;
            active = true;
        }

        if (ImGui::TabButton("K", "Logger", ImVec2(190, 40)) && tab_count != 3) {
            tab_count = 3;
            active = true;
        }

        if (ImGui::TabButton("E", "Settings", ImVec2(190, 40)) && tab_count != 5) {
            tab_count = 5;
            active = true;
        }

        if (active) {
            { active = false; tabs = tab_count; };
        }


        static bool boolla = false;
        static int intle = 0;

        static int misc_tab = 1;
        switch (tabs)
        {

        case 0:
        {

            ImGui::SetCursorPos(ImVec2(203, 88));
            ImGui::BeginChild("Silent Aim", ImVec2(339, 253), true); { //253

                ImGui::Checkbox("Silent Aim", &silentaim);


                ImGui::Checkbox("Range Shot", &RangeShot);

                const char* body_types[]{ "Head", "Body", "Foot", "Random"};
                ImGui::Combo("Silent Aim Hit Region", &silent_body_type, body_types, IM_ARRAYSIZE(body_types));

                /*
                static int field_derecesi = 1;
                const char* field[]{ "%50" , "%100" };
                field_derece_ = (int)field_derecesi;

                ImGui::Combo("Silent Field Of View", &field_derecesi, field, IM_ARRAYSIZE(field));
                */

            }ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(203, 353));
            ImGui::BeginChild("Aimbot", ImVec2(339, 210), true); {

                if (ImGui::Checkbox("Aimbot", &autoaim)) FuncitonDevoloping();

                static int aimbot_type = 0;
                static const char* items[]{ "Head", "Body" };

                ImGui::Combo("Aimbot Hit Region", &aimbot_type, items, IM_ARRAYSIZE(items));


            }ImGui::EndChild();


            ImGui::SetCursorPos(ImVec2(555, 88));
            ImGui::BeginChild("Aim Filters", ImVec2(375, 475), true); {


                ImGui::Checkbox("Ignore Driver", &IgnoreDriver);
                ImGui::Checkbox("Ignore Onfoot", &IgnoreOnfoot);
                ImGui::Checkbox("Ignore My Color", &PlayerHaveMyColor);
                ImGui::Checkbox("Target Only Enemy", &OnlyEnemyFilter);

            }ImGui::EndChild();

            break;
        }
        case 1:
        {

            ImGui::SetCursorPos(ImVec2{ 250, 50 });
            if (ImGui::Renderingsubtab("PLAYER", misc_tab == 1)) misc_tab = 1;

            ImGui::SetCursorPos(ImVec2{ 350, 50 });
            if (ImGui::Renderingsubtab("VEHICLE", misc_tab == 2)) misc_tab = 2;

            ImGui::SetCursorPos(ImVec2{ 450, 50 });
            if (ImGui::Renderingsubtab("WEAPON", misc_tab == 3)) misc_tab = 3;

            ImGui::SetCursorPos(ImVec2{ 550, 50 });
            if (ImGui::Renderingsubtab("OTHERS", misc_tab == 4)) misc_tab = 4;



            if (misc_tab == 1)
            {
                ImGui::SetCursorPos(ImVec2(203, 92));
                ImGui::BeginChild("Esp", ImVec2(340, 200), true); { //253

                    static bool test_0 = false;
                    ImGui::Checkbox("Box", &box);
                    ImGui::Checkbox("Trace", &trace);

                    static const char* trace_items[]{ "Aimed", "All", };
                    ImGui::Combo("Trace Type", &trace_type, trace_items, IM_ARRAYSIZE(trace_items));

                }ImGui::EndChild();


                ImGui::SetCursorPos(ImVec2(203, 300));
                ImGui::BeginChild("Slapper", ImVec2(340, 260), true); { //253

                    static bool test_0 = false;
                    ImGui::Checkbox("Actor Slapper", &ActorSlapper);
                    ImGui::Checkbox("Vehicle Slapper", &CarSlapper);

                    static const char* items[]{ "Aimed", "All" };
                    ImGui::Combo("Slapper Distribution", &SlapperType, items, IM_ARRAYSIZE(items));

                }ImGui::EndChild();


                ImGui::SetCursorPos(ImVec2(570, 92));
                ImGui::BeginChild("Visual Cheats", ImVec2(355, 215), true); { //253
                    ImGui::SetScrollX(0);
                    ImGui::SetScrollY(0);
                    ImGui::Checkbox("AFK", &afk);
                    ImGui::Checkbox("Anti Stun", &antistun);
                    ImGui::Checkbox("Anti Car Surfing", &antics);
                    ImGui::Checkbox("Invisible[UP]", &ActorUPInv);
                }ImGui::EndChild();

                ImGui::SetCursorPos(ImVec2(570, 325));
                ImGui::BeginChild("BUTTON", ImVec2(355, 235), true); { //253

                    ImGui::SetScrollX(0);
                    ImGui::SetScrollY(0);

                    static int sec = 0;
                    ImGui::InputInt("GLOBAL INT:", &sec, 0, 999);

                    ImGui::SetCursorPos(ImVec2(15, 110));
                    if (ImGui::Button__("Set Skin", ImVec2(315, 30), ImGuiButtonFlags_None)) SetSkin(sec);
                    ImGui::SetCursorPos(ImVec2(15, 150));
                    if (ImGui::Button__("Set Health", ImVec2(315, 30), ImGuiButtonFlags_None)) SetHealth(sec);
                    ImGui::SetCursorPos(ImVec2(15, 190));
                    if (ImGui::Button__("Set Armor", ImVec2(315, 30), ImGuiButtonFlags_None)) SetArmour(sec);

                }ImGui::EndChild();
            }

            if (misc_tab == 2)
            {

                ImGui::SetCursorPos(ImVec2(203, 88));
                ImGui::BeginChild("Visual Settings", ImVec2(340, 475), true); { //253




                    static const char* car_colors_items[]{ "Red", "Green", "Black", "Blue", "Purple" };
                    ImGui::Combo("Car Color", &car_color, car_colors_items, IM_ARRAYSIZE(car_colors_items));

                    ImGui::SetCursorPos(ImVec2(15, 100));
                    if (ImGui::Button__("Set Car Color", ImVec2(315, 30), ImGuiButtonFlags_None)) Set_Car_Color(car_color);

                    ImGui::SetCursorPos(ImVec2(15, 160));
                    ImGui::InputInt("Car Health:", &car_health, 0, 999);

                    ImGui::SetCursorPos(ImVec2(15, 200));
                    if (ImGui::Button__("Set Car Health", ImVec2(315, 30), ImGuiButtonFlags_None)) Set_Car_Health(car_health);

                    ImGui::SetCursorPos(ImVec2(15, 240));
                    if (ImGui::Button__("Reset Vehicle Wiew", ImVec2(315, 30), ImGuiButtonFlags_None)) Reset_Car_Wiew();

                    ImGui::SetCursorPos(ImVec2(15, 280));
                    static const char* random_color_items[]{ "Basic", "Improved" };
                    ImGui::Combo("R|Car Color Type", &random_car_color_id, random_color_items, IM_ARRAYSIZE(random_color_items));

                    ImGui::SetCursorPos(ImVec2(15, 320));
                    if (ImGui::Button__("Random Car Color", ImVec2(315, 30), ImGuiButtonFlags_None)) Set_Random_Car_Color();




                }ImGui::EndChild();


                ImGui::SetCursorPos(ImVec2(555, 88));
                ImGui::BeginChild("Visual Cheats", ImVec2(375, 475), true); { //253



                    ImGui::Checkbox("Car Invisible[UP]", &VehiclePosInv);


                }ImGui::EndChild();

            }

            if (misc_tab == 3)
            {

                ImGui::SetCursorPos(ImVec2(203, 88));
                ImGui::BeginChild("Weapon Cheats", ImVec2(735, 475), true); { //253


                    ImGui::Checkbox("No Reload", &NoReload);
                    ImGui::Checkbox("Fake Bullet", &FakeBullet);


                }ImGui::EndChild();

            }

            if (misc_tab == 4)
            {
                ImGui::SetCursorPos(ImVec2(203, 88));
                ImGui::BeginChild("Another Cheats", ImVec2(735, 475), true); {


                    ImGui::Checkbox("Enemy Panel", &ShowEnemyPanel);

                }ImGui::EndChild();
            }

            break;
        }
        case 2:
        {
            ImGui::SetCursorPos(ImVec2(203, 88));
            ImGui::BeginChild("Friend", ImVec2(339, 253), true); { //253

                ImGui::SetScrollX(0);
                ImGui::SetScrollY(0);

                static int sec = 0;
                ImGui::InputInt("Player ID:", &sec, 0, 999);

                ImGui::SetCursorPos(ImVec2(15, 100));
                if (ImGui::Button__("Add Friend", ImVec2(300, 30), NULL)) add_friend(sec);

                ImGui::SetCursorPos(ImVec2(15, 140));
                if (ImGui::Button__("Delete Friend", ImVec2(300, 30), NULL)) delete_friend(sec);

                ImGui::SetCursorPos(ImVec2(15, 180));
                if (ImGui::Button__("Delete All Friends", ImVec2(300, 30), NULL)) delete_all_friend();



            }ImGui::EndChild();


            ImGui::SetCursorPos(ImVec2(555, 88));
            ImGui::BeginChild("Enemy", ImVec2(345, 253), true); {

                ImGui::SetScrollX(0);
                ImGui::SetScrollY(0);

                static int sec = 0;
                ImGui::InputInt("Player ID:", &sec, 0, 999);

                ImGui::SetCursorPos(ImVec2(15, 100));
                if (ImGui::Button__("Add Enemy", ImVec2(300, 30), NULL)) add_enemy(sec);

                ImGui::SetCursorPos(ImVec2(15, 140));
                if (ImGui::Button__("Delete Enemy", ImVec2(300, 30), NULL)) delete_enemy(sec);

                ImGui::SetCursorPos(ImVec2(15, 180));
                if (ImGui::Button__("Delete All Enemys", ImVec2(300, 30), NULL)) delete_all_enemys();

            }ImGui::EndChild();


            ImGui::SetCursorPos(ImVec2(203, 353));
            ImGui::BeginChild("Warp", ImVec2(339, 220), true); {

                static int sec = 0;
                ImGui::InputInt("Player ID:", &sec, 0, 999);

                ImGui::SetCursorPos(ImVec2(15, 100));
                if (ImGui::Button__("Warp To Player", ImVec2(300, 30), NULL)) warp(sec);


            }ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(555, 353));
            ImGui::BeginChild("Spectate", ImVec2(345, 220), true); {

                ImGui::SetScrollX(0);
                ImGui::SetScrollY(0);

                static int sec = 0;
                ImGui::InputInt("Player ID:", &sec, 0, 999);

                ImGui::SetCursorPos(ImVec2(15, 100));
                if (ImGui::Button__("Spectate", ImVec2(300, 30), NULL)) spec(sec);

                ImGui::SetCursorPos(ImVec2(15, 140));
                if (ImGui::Button__("End Spectate", ImVec2(300, 30), NULL)) end_spec();

            }ImGui::EndChild();

            break;
        }
        case 3:
        {
            ImGui::SetCursorPos(ImVec2(203, 88));
            ImGui::BeginChild("Logger", ImVec2(720, 470), true); { //253
                
                ImGui::SetCursorPos(ImVec2(20, 70));
                if (ImGui::Button("Temizle"))
                    logger->clear();

                ImGui::SetCursorPos(ImVec2(100, 70));
                if (ImGui::Button("Kopyala"))
                    ImGui::LogToClipboard();



                const auto buf{ logger->buffer.begin() };
                const auto bufEnd{ logger->buffer.end() };

                if (logger->filter.IsActive()) {
                    for (auto line_no{ 0 }; line_no < logger->lineOffsets.Size; ++line_no) {
                        const auto line_start{ buf + logger->lineOffsets[line_no] };
                        const auto line_end{ (line_no + 1 < logger->lineOffsets.Size) ? (buf + logger->lineOffsets[line_no + 1] - 1) : bufEnd };
                        if (logger->filter.PassFilter(line_start, line_end))
                            ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                else {
                    ImGuiListClipper clipper;
                    clipper.Begin(logger->lineOffsets.Size);
                    while (clipper.Step()) {
                        for (auto line_no{ clipper.DisplayStart }; line_no < clipper.DisplayEnd; ++line_no) {
                            const auto line_start{ buf + logger->lineOffsets[line_no] };
                            const auto line_end{ (line_no + 1 < logger->lineOffsets.Size) ? (buf + logger->lineOffsets[line_no + 1] - 1) : bufEnd };
                            ImGui::TextUnformatted(line_start, line_end);
                        }
                    }
                    clipper.End();
                }

                

            }ImGui::EndChild();


            break;
        }
        case 5:
        {
            ImGui::SetCursorPos(ImVec2(203, 88));

            ImGui::BeginChild("Discord RPC", ImVec2(339, 180), true); {

                ImGui::Checkbox("Discord RPC", &Discord_RPC);
                if (Discord_RPC)
                    ImGui::Text("Discord-RPC opened.");
                else
                    ImGui::Text("Discord-RPC not opened.");



            }ImGui::EndChild();


            ImGui::SetCursorPos(ImVec2(555, 88));
            ImGui::BeginChild("Background", ImVec2(339, 180), true); {

                ImGui::ColorEdit4("Widget Color", (float*)&accent_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);

                static const char* items[]{ "Dark", "Grey" };

                ImGui::Combo("Background Color", &backgroundcolor, items, IM_ARRAYSIZE(items));

            }ImGui::EndChild();



            break;
        }

        }
    }
    ImGui::End();
    


}

void CMenu::hkEndScene()
{

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ÝmMenu->RenderMenu();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}