namespace Vars {
     namespace Esp {
	  bool start = false;
	  bool line = false;
	  bool box = false;	
	  bool distance = false;
	  bool hp = false;
	  bool crosshair = false;
	  bool nickname = false;
	  float ThicknessLine = 2.0f;
      float EnemyLineX = 0.0f;
      float EnemyLineY = 0.0f;
} 
	 namespace Player {
	 bool func1 = false;
	 float Gravity = 0.0f;
         void (*_gravity)(void *instance);
         void gravity(void *instance){
         if(Gravity){
         *(float *)((uint64_t)
         instance + 0xc8) = Gravity;  
       }
        return _gravity(instance);
    }

	  bool Chams1 = false;
      bool Chams2 = false;
      bool Chams3 = false;
      bool Chams4 = false;
      bool Chams5 = false;
      bool Chams6 = false;
      bool Chams7 = false;
      bool Chams8 = false;

      int Color1 = 0;
      int Color2 = 0;
      int Color3 = 0;
      int Color4 = 0;
	
}
     namespace Weapon {
	 bool func3 = false;
	 bool func4 = false;
	 bool func5 = false;
	 bool func6 = false;
}
     namespace Other {
	 bool func7 = false;
	 bool func8 = false;
	 bool func9 = false;
	 bool func10 = false;
}

}
