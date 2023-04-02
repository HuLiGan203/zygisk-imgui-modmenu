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
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "hack.h"
#include "game.h"
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Dobby/dobby.h"
//#include "Includes/Macros.h"
#include "xdl.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_android.h"
#include "functions.h"
#include "Themes.h"
#include "Data/Fonts/Roboto-Regular.h"
#include "Draw/Include.h"
#include "Mono/MonoString.cpp"
#include "Unity/Quaternion.hpp"
#include "KittyMemory/MemoryPatch.h"
#include "And64InlineHook/And64InlineHook.hpp"
#include "Substrate/SubstrateHook.h"
#include "Substrate/CydiaSubstrate.h"

static int glHeight, glWidth;
static bool g_IsSetup = false;
uintptr_t address;

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

/*struct My_Patches {
MemoryPatch Sh, bypass1, bypass2, bypass3;
} hexPatches;*/

void *(*get_main)();

Vector3 (*WorldToScreenPoint)(void *instance, Vector3);
Vector3 (*Transform_get_position)(void *instance);
Vector3 (*get_forward)(void *instance);
void (*set_position)(void *instance, Vector3);
void (*get_position)(void *instance, Vector3);
void *(*get_transform)(void *instance);
MonoString *(*PlayerName)(void *instance);
Vector3 getPosition(void *player){
    return Transform_get_position(get_transform(player));
}

int GetPlayerHealth(void *player) {
    return *(float *) ((uint64_t) player + 0x110);
} 

bool PlayerAlive(void *player) {
    return player != NULL && GetPlayerHealth(player) > 0;
}
bool IsPlayerDead(void *player) {
    return player == NULL && GetPlayerHealth(player) <= 0;
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
ImGui::Text("Fly:");
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
ImGui::Checkbox("Function", &Vars::Other::func7);
ImGui::SameLine();
ImGui::Checkbox("Function", &Vars::Other::func8);
ImGui::Checkbox("Function", &Vars::Other::func9);
ImGui::SameLine();
ImGui::Checkbox("Function", &Vars::Other::func10);
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
    ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
    ImGui::NewFrame();
    ImGui::ShowExampleGameWindows();
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}


//--------MsHook-------//
void *hack_thread(void *) {
	
    WorldToScreenPoint = (Vector3(*)(void*, Vector3)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c2b1f4);//Camera WorldToScreenPoint(Vector3 position)
    Transform_get_position = (Vector3 (*)(void*)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c49688);//Transform get_position
    get_forward = (Vector3 (*)(void*)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c4a080);//Transform get_forward
    get_position = (void (*)(void *, Vector3)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c496e8);//Transform get_position_Injected
    set_position = (void (*)(void *, Vector3)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c49790);//Transform set_position_Injected
    get_transform = (void *(*)(void*)) 
                getAbsoluteAddress("libil2cpp.so", 0x1c2d518);//Component get_transform
    get_main = (void*(*)()) 
                getAbsoluteAddress("libil2cpp.so", 0x1c2b4d8);//Camera get_main  
    PlayerName = (MonoString *(*)(void *))
                getAbsoluteAddress("libil2cpp.so", 0x23270fc);//Player name
	/*match = (void*(*)()))
	            getAbsoluteAddress("libil2cpp.so", 0xA61004);//Stop Esp*/
				
    A64HookFunction((void *)getAbsoluteAddress("libil2cpp.so", 0x2b8e10c),
                                            (void *) &Player_update,
                                            (void **) &old_Player_update);
    A64HookFunction((void *)getAbsoluteAddress("libil2cpp.so", 0x27aa960), 
		                            (void *) &Vars::Player::gravity, 
		                            (void **) &Vars::Player::_gravity);
	
 #if defined(aarch64)
 //64-bit   
 //32-bit
	
     /*hexPatches.bypass1 = MemoryPatch::createWithHex("libil2cpp.so", 0x7bd, "00 00 00 00");
     hexPatches.bypass2 = MemoryPatch::createWithHex("libil2cpp.so", 0x7bc, "00 00 00 00");
     hexPatches.bypass3 = MemoryPatch::createWithHex("libil2cpp.so", 0x7be, "00 00 00 00");
    	
     hexPatches.bypass1.Modify();Anti detect 
     hexPatches.bypass2.Modify();
     hexPatches.bypass3.Modify();Anti detect 
	
    	
 01 00 A0 E3 1E FF 2F E1 = 1/True
 00 00 A0 E3 1E FF 2F E1 = 0/False
 FA 04 44 E3 1E FF 2F E1 = FLOAT/2000
 c6 02 44 e3 1e ff 2f e1 = FLOAT/99
 20 02 44 E3 1E FF 2F E1 = FLOAT/40
 C8 02 44 E3 1E FF 2F E1 = FLOAT/100
 37 00 A0 E3 1E FF 2F E1 = 50 INT
 FF 00 A0 E3 1E FF 2F E1 = 255 INT
 E7 03 00 E3 1E FF 2F E1 = 999 INT
 DC 0F 0F E3 1E FF 2F E1 = 65500/INT
 */
#else 
	 
    #endif
    pthread_exit(nullptr);
    return nullptr;
}

void *imgui_go(void *) {
    address = findLibrary("libil2cpp.so");
    auto addr = (uintptr_t)dlsym(RTLD_NEXT, "eglSwapBuffers");
    DobbyHook((void *)addr, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
    pthread_exit(nullptr);
    return nullptr;  
}

void *bugly_thread(void *) {
    do {
        sleep(99999);
    } while (!isLibraryLoaded("libanogs.so"));
    pthread_exit(nullptr);
    return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, NULL, imgui_go, NULL);
	pthread_t bugly;
    pthread_create(&bugly, NULL, bugly_thread, NULL);
    pthread_t hacks;
    pthread_create(&hacks, NULL, hack_thread, NULL);
}

