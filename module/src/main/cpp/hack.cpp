#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "hack.h"
#include "log.h"
#include "game.h"
#include "utils.h"
#include "xdl.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "MemoryPatch.h"
#include "functions.h"
#include "Themes.h"

static int g_GlHeight, g_GlWidth;
static bool g_IsSetup = false;
static std::string g_IniFileName = "";
static utils::module_info g_TargetModule{};

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

static int tabb = 0;
//-------------------//
void ImGui::ShowExampleGameWindows(bool* p_open){
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
ImGui::Checkbox("New", &Vars::Player::func1);	
ImGui::Text("G:");
ImGui::SliderFloat("", &Vars::Player::func2, -50.0f, 50.0f);
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
ImGui::Checkbox("Function", &Vars::Other::func7);
ImGui::SameLine();
ImGui::Checkbox("Function", &Vars::Other::func8);
ImGui::Checkbox("Function", &Vars::Other::func9);
ImGui::SameLine();
ImGui::Checkbox("Function", &Vars::Other::func10);
}	
   } 
   
      /*if (Vars::Esp::start) {		  
            std::string Allplayers;     
            Allplayers += "Near People: ";
            Allplayers += std::to_string((int32_t) players.size());
            DrawAddLine::DrawText2(80.0f, ImVec2(glWidth * 0.40f - 0.0f, glHeight * 0.0f + 85.0f), ImVec4(0, 1, 1, 1), Allplayers.c_str());             			
	    	for (int i = 0; i < players.size(); i++) {
                        
            void *Player;
            if (i < players.size()) 
            Player = players[i];
            if(Player != NULL && get_main() !=NULL) {             
                Vector3 PlayerPos = getPosition(Player);
                Vector3 MyPos = getPosition(myPlayer);             
                //Head
                Vector3 HeadPos = getPosition(Player);  
                Vector3 Head = Vector3(HeadPos.x, HeadPos.y + 0.72,HeadPos.z);                        
                //Bottom
                Vector3 BottomPos = getPosition(Player);
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
			
				if(Vars::Esp::nickname && PlayerAlive(Player)) {
				MonoString *isPlayerName = PlayerName(Player);			
				DrawAddLine::DrawText2(25.0f, ImVec2(playerRect.x + (boxWidth / 15.5), playerRect.y - 39), ImVec4(0, 1, 1, 1), isPlayerName->toChars()); 					
				}
	      	}
		}		
    }*/
   ImGui::End();
}
void SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = g_IniFileName.c_str();
    io.DisplaySize = ImVec2((float)g_GlWidth, (float)g_GlHeight);
    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::StyleColorsLight();
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);
    ImGui::GetStyle().ScaleAllSizes(3.0f);
    Theme::SetBlueOceanTheme();
}

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

    if (!g_IsSetup) {
      SetupImGui();
      g_IsSetup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();
    ImGui::ShowExampleGameWindows();
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void hack_start(const char *_game_data_dir) {
    LOGI("hack start | %s", _game_data_dir);
    do {
        sleep(1);
        g_TargetModule = utils::find_module(TargetLibName);
    } while (g_TargetModule.size <= 0);
    LOGI("%s: %p - %p",TargetLibName, g_TargetModule.start_address, g_TargetModule.end_address);

    // TODO: hooking/patching here
    
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
