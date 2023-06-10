#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <jni.h>
#include <fstream>
#include <iostream>

#include "hack.h"
#include "log.h"
#include "game.h"
#include "utils.h"
#include "xdl.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_android.h"
#include "functions.h"
#include "Themes.h"
#include "Data/Fonts/Roboto-Regular.h"
#include "Draw/Include.h"
#include "Mono/MonoString.cpp"
#include "Unity/Quaternion.hpp"
#include "KittyMemory/MemoryPatch.h"
#include "Dobby/include/dobby.h"
#include "Chams.h"
#include "Includes/Utils.h"
#include "test.h"
#include "ByNameModding/Tools.h"
#include "ByNameModding/fake_dlfcn.h"
#include "ByNameModding/Il2Cpp.h"

int glHeight, glWidth;
static bool g_IsSetup = false;
static std::string g_IniFileName = "";
static utils::module_info g_TargetModule{};
std::map<std::string, uintptr_t> Methods;
uintptr_t g_il2cpp;

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

vector<void*> players;
int MAX_PLAYERS = 40;

void (*old_Player_OnDestroy)(...);
void Player_OnDestroy(void *player) {
    if (player != NULL) {
        old_Player_OnDestroy(player);
        players.clear();  
    }
}

bool playerFind(void *pl) {
    if (pl != NULL) {
        for (int i = 0; i < players.size(); i++) {
            if (pl == players[i]) return true;
        }
    }
    return false;
}

void *myPlayer;
void *Player;
void (*old_Player_update)(...);
void Player_update(void *player) {
    if (player != NULL) {
        
        
        bool isMine = *(bool *) ((uint64_t) player + 0x0);
        if (isMine) {
            myPlayer = player;
        }
        
        if (!playerFind(player)) players.push_back(player);
        if (players.size() > MAX_PLAYERS) {
            players.clear();
        }
    }
    old_Player_update(player);
}

typedef void Camera;
typedef void Transform;
typedef void Component;

Vector3 get_position(Transform *instance) {
    auto Transform_get_position = (Vector3 (*)(Transform *)) (Methods["Transform::get_position"]);
    return Transform_get_position(instance);
}

Transform *get_transform(void * Player) {
    auto Component_get_transform = (Transform *(*)(Component *)) (Methods["Component::get_transform"]);
    return Component_get_transform(Player);
}

static Camera *get_main() {
    auto Camera_get_main = (Camera *(*)()) (Methods["Camera::get_main"]);
    return Camera_get_main();
}

Vector3 WorldToScreenPoint(Vector3 pos) {
    auto main = get_main();
    if (main) {
        auto Camera_WorldToScreenPoint = (Vector3 (*)(Camera *, Vector3)) (Methods["Camera::WorldToScreenPoint"]);
        return Camera_WorldToScreenPoint(main);
    }
    return {0, 0, 0};
}

Vector3 PlayerPosition = WorldToScreenPoint(get_position(get_transform(Player)));

/*MonoString *(*PlayerName)(void *instance) {
    auto Transform_get_position = (Vector3 (*)(Transform *)) (Methods["Transform::get_position"]);
    return Transform_get_position(instance);
}*/

int GetPlayerHealth(void *player) {
    return *(float *) ((uint64_t) player + 0x110);
}

bool PlayerAlive(void *player) {
    return player != NULL && GetPlayerHealth(player) > 0;
}

static int tabb = 0;
static int itemChams = 4;
//-------------------//
void ImGui::HackWindow(bool* p_open){
   const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 250, main_viewport->WorkPos.y + 12), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(850, 680), ImGuiCond_FirstUseEver);
    
if (ImGui::Begin("0xHack", p_open)){
        
        if (ImGui::Button("Esp", ImVec2(200.f, 0.0f))) {                    
        tabb = 0;       
}   
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button("Player", ImVec2(200.f, 0.0f))) {
        tabb = 1;
}
        ImGui::SameLine(0.f, 2.f);        
        if (ImGui::Button("Weapon", ImVec2(200.f, 0.0f))) {
        tabb = 2;
}
        ImGui::SameLine(0.f, 2.f);
        if(ImGui::Button("Other", ImVec2(200.f, 0.0f))) {
        tabb = 3;
}       
                       
if (tabb == 0) {
ImGui::BeginChild("", ImVec2(780.0f,400.0f), true);
ImGui::Checkbox("Open Esp", &Vars::Esp::start);
ImGui::SameLine();
ImGui::Checkbox("ESP Line", &Vars::Esp::line);
ImGui::Checkbox("ESP Box", &Vars::Esp::box);
ImGui::SameLine();
ImGui::Checkbox("ESP Distance", &Vars::Esp::distance);
ImGui::Checkbox("ESP Health", &Vars::Esp::hp);
ImGui::SameLine();
ImGui::Checkbox("ESP Crosshair", &Vars::Esp::crosshair);
ImGui::Checkbox("ESP Name", &Vars::Esp::nickname);
ImGui::SliderFloat("Thickness", &Vars::Esp::ThicknessLine, 1.0f,30.0f);
ImGui::SliderFloat("Line X", &Vars::Esp::EnemyLineX,-100.0f, 100.0f);
ImGui::SliderFloat("Line Y", &Vars::Esp::EnemyLineY, -100.0f, 100.0f);
ImGui::EndChild();
}

else if (tabb == 1) {
ImGui::BeginChild("", ImVec2(780.0f,400.0f), true);
ImGui::Checkbox("Fly", &Vars::Player::func1);   
ImGui::SliderFloat("", &Vars::Player::Gravity, -50.0f, 50.0f);
}

else if (tabb == 2) {
ImGui::BeginChild("", ImVec2(780.0f,400.0f), true);
ImGui::Checkbox("Damage", &Vars::Weapon::func3);
ImGui::Text("InfinityAmmo:");
ImGui::Checkbox("1", &Vars::Weapon::func4);
ImGui::SameLine();
ImGui::Checkbox("2", &Vars::Weapon::func5);     
ImGui::Checkbox("FastShoot", &Vars::Weapon::func6);
}



else if (tabb == 3) {
ImGui::BeginChild("", ImVec2(780.0f,400.0f), true);
  if (ImGui::Combo("Shaders", &itemChams, "_MainLightPosition\0_BaseMap\0_unity_SpecCube0\0_MainTex_ST\0")) {
                switch (itemChams) {
                case 0:
                    setShader("_MainLightPosition");
                    break;
                case 1:
                    setShader("_BaseMap");
                    break;
                case 2:
                  setShader("unity_SpecCube0");
                    break;
                case 3:
                    setShader("_MainTex_ST");
                    break;
                }
            }
if (ImGui::Checkbox("Default Chams", &Vars::Player::Chams1)) {
                if (Vars::Player::Chams1 == false) {
                    SetWallhack(false);
                } else {
                    SetWallhack(true);
                }
            }
            if (ImGui::Checkbox("Shading Chams", &Vars::Player::Chams2)) {
                if (Vars::Player::Chams2 == false) {
                    SetWallhackS(false);
                } else {
                    SetWallhackS(true);
                }
            }
            if (ImGui::Checkbox("Wireframe Chams", &Vars::Player::Chams3)) {
                if (Vars::Player::Chams3 == false) {
                    SetWallhackW(false);
                } else {
                    SetWallhackW(true);
                }
            }
            if (ImGui::Checkbox("Glow Chams", &Vars::Player::Chams4)) {
                if (Vars::Player::Chams4 == false) {
                    SetWallhackG(false);
                } else {
                    SetWallhackG(true);
                }
            }
            if (ImGui::Checkbox("Outline Chams", &Vars::Player::Chams5)) {
                if (Vars::Player::Chams5 == false) {
                    SetWallhackO(false);
                } else {
                    SetWallhackO(true);
                }
            }
            if (ImGui::Checkbox("Rainbow Chams", &Vars::Player::Chams7)) {
                if (Vars::Player::Chams7 == false) {
                    SetRainbow(false);
                } else {
                    SetRainbow(true);
                }
            }
            if (ImGui::Checkbox("SetRainbow", &Vars::Player::Chams8)) {
                if (Vars::Player::Chams8 == false) {
                    SetRainbow1(false);
                } else {
                    SetRainbow1(true);
                }
            }
            if (ImGui::SliderInt("Line Width", &Vars::Player::Color1, 0, 10)) {
                SetW(Vars::Player::Color1);
            }
            if (ImGui::SliderInt("Color Red", &Vars::Player::Color2, 0, 255)) {
                SetR(Vars::Player::Color2);
            }
            if (ImGui::SliderInt("Color Green", &Vars::Player::Color3, 0, 255)) {
                SetG(Vars::Player::Color3);
            }
            if (ImGui::SliderInt("Color Blue", &Vars::Player::Color4, 0, 255)) {
                SetB(Vars::Player::Color4);
            } 
}   
   } 
	
      if (Vars::Esp::start) {         
            std::string Allplayers;     
            Allplayers += "Near People: ";
            Allplayers += std::to_string((int32_t) players.size());
            DrawAddLine::DrawText2(80.0f, ImVec2(glWidth * 0.40f - 0.0f, glHeight * 0.0f + 85.0f), ImVec4(0, 1, 1, 1), Allplayers.c_str());                         
            for (int i = 0; i < players.size(); i++) {
                        
            void *Player;
            if (i < players.size()) 
            Player = players[i];
            if(Player != NULL && get_main() !=NULL) {             
                Vector3 PlayerPos = get_position(Player);
                Vector3 MyPos = get_position(myPlayer);             
                //Head
                Vector3 HeadPos = get_position(Player);  
                Vector3 Head = Vector3(HeadPos.x, HeadPos.y + 0.72,HeadPos.z);                        
                //Bottom
                Vector3 BottomPos = get_position(Player);
                Vector3 Bottom = Vector3(BottomPos.x, BottomPos.y - 1.2,BottomPos.z);
                                            
                auto HeadPosition = WorldToScreenPoint(get_main(), Head);
                auto BottomPosition = WorldToScreenPoint(get_main(), Bottom);
                
                if (HeadPosition.z < 1.f) continue;
                if (BottomPosition.z < 1.f) continue;
               
                if (Vars::Esp::line && PlayerAlive(Player)){
                    DrawAddLine::DrawLine(ImVec2(glWidth * 0.5f, glHeight * 0.14f),
                                          ImVec2(HeadPosition.x + Vars::Esp::EnemyLineX,
                                                 glHeight - HeadPosition.y + Vars::Esp::EnemyLineY),
                                          ImVec4(1.0,0.921569,0.0156863,1.0), Vars::Esp::ThicknessLine);
                }                         
        float boxHeight = abs(HeadPosition.y - BottomPosition.y) / 0.8f;
                float boxWidth = boxHeight * 0.60f;
                Rect playerRect(HeadPosition.x - (boxWidth / 2), (glHeight - HeadPosition.y - 5.0f), boxWidth, boxHeight);                   
                if (Vars::Esp::box && PlayerAlive(Player)){
                DrawAddLine::DrawBox(playerRect, ImVec4(0.00, 1.00, 1.00, 1.00));   
                }   
                if (Vars::Esp::crosshair ){
                DrawAddLine::DrawCrosshair(ImVec4(0, 1, 1 ,1), Vector2(glWidth / 2, glHeight / 2), 15.0f); 
                float AimFov = 320.0f;
                DrawAddLine::DrawCircle(ImVec4(1, 1, 0, 1), glWidth * 0.5f, glHeight * 0.5f, AimFov, false);
                }
                
                if (Vars::Esp::hp && PlayerAlive(Player)){
                DrawAddLine::DrawHorizontalHealthBar(Vector2(playerRect.x, playerRect.y - 12), boxWidth, 100.0f,GetPlayerHealth(Player), ImVec4(1, 0, 0, 1));
                std::string GetHp;      
                GetHp += "Hp: ";
                GetHp += std::to_string((int32_t) GetPlayerHealth(Player));        
                DrawAddLine::DrawText2(23.0f, ImVec2(playerRect.x + (playerRect.width / 0.78), playerRect.y - 20), ImVec4(0, 1, 1, 1), GetHp.c_str());  
                }
                
                if(Vars::Esp::distance && PlayerAlive(Player)){
                char extra[30];
                float DistanceTo = DrawAddLine::get_3D_Distance(MyPos.x, MyPos.y, MyPos.z, PlayerPos.x, PlayerPos.y, PlayerPos.z);              
                sprintf(extra, "%0.0f m", DistanceTo);                  
                DrawAddLine::DrawText2(23.0f, ImVec2(playerRect.x - (playerRect.width / 0.88), playerRect.y - 20), ImVec4(0, 1, 1, 1), extra);                      
                }
            
                /*if(Vars::Esp::nickname && PlayerAlive(Player)) {
                MonoString *isPlayerName = PlayerName(Player);          
                DrawAddLine::DrawText2(25.0f, ImVec2(playerRect.x + (boxWidth / 15.5), playerRect.y - 39), ImVec4(0, 1, 1, 1), isPlayerName->toChars());*/                    
            }
        }       
    }
   ImGui::End();
}

void SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);
    ImGui_ImplOpenGL3_Init("#version 100");      
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular,22,29.0f);
    ImGui::GetStyle().ScaleAllSizes(9.0f);
    Theme::SetBlueOceanTheme();
}

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!g_IsSetup) {
      SetupImGui();
      g_IsSetup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::HackWindow();
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (float)io.DisplaySize.x, (float)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void hack_start(const char *_game_data_dir) {
	
    while (!g_il2cpp) {
        g_il2cpp = Tools::GetBaseAddress("libil2cpp.so");
        sleep(1);
    }
	
    LOGI("hack start | %s", _game_data_dir);
    do {
        sleep(1);
        g_TargetModule = utils::find_module(TargetLibName);
    } while (g_TargetModule.size <= 0);
    LOGI("%s: %p - %p",TargetLibName, g_TargetModule.start_address, g_TargetModule.end_address);

    //TODO:hooking/patching here    
ProcMap il2cppMap;        
        do {
            il2cppMap = KittyMemory::getLibraryMap("libil2cpp.so");
            sleep(1);
        } while (!il2cppMap.isValid() && mlovinit());
        setShader("");
        LogShaders();
        Wallhack();

    Il2CppAttach();
         
    Methods["Transform::get_position"] = (uintptr_t) Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Transform", "get_position");   
    Methods["Camera::get_main"] = (uintptr_t) Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "get_main", 0);
    Methods["Camera::WorldToScreenPoint"] = (uintptr_t) Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Camera", "WorldToScreenPoint_Injected", 1);   
    Methods["Component::get_transform"] = (uintptr_t) Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Component", "get_transform");   
    Methods["Player::get_health"] = (uintptr_t) Il2CppGetMethodOffset("UnityEngine.CoreModule.dll", "UnityEngine", "Player", "get_health");
    
    Tools::Hook(Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "OtherPlayerController", "Update", 0), 
    (void *) Player_update, 
    (void **) &old_Player_update);  
    
    Tools::Hook(Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "OtherPlayerController", "OnDestroy", 0), 
    (void *) Player_OnDestroy, 
    (void **) &old_Player_OnDestroy);  
    
}

void hack_prepare(const char *_game_data_dir) {
    LOGI("hack thread: %d", gettid());
    int api_level = utils::get_android_api_level();
    LOGI("api level: %d", api_level);
    g_IniFileName = std::string(_game_data_dir) + "files/imgui.ini";
    sleep(5);

    void *sym_input = DobbySymbolResolver("/system/lib/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (NULL != sym_input){
        DobbyHook((void *)sym_input, (dobby_dummy_func_t) myInput, (dobby_dummy_func_t*)&origInput);
    }
    
    void *egl_handle = xdl_open("libEGL.so", 0);
    void *eglSwapBuffers = xdl_sym(egl_handle, "eglSwapBuffers", nullptr);
    if (NULL != eglSwapBuffers) {
        utils::hook((void*)eglSwapBuffers, (func_t)hook_eglSwapBuffers, (func_t*)&old_eglSwapBuffers);
    }
    xdl_close(egl_handle);

    hack_start(_game_data_dir);
}
