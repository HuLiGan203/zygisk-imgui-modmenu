Quaternion lookRotation;
Vector3 AimPlayerPos;
bool IsFiring;
void *closestEnemy2 = nullptr;
void (*Player_update)(void *player);
void _Player_update(void *player) {
   if (player != NULL) {
        Player_update(player); 
        if (IsLocal(player)) {
            myPlayer = player;
        }
        if (myPlayer) {
            if (GetPlayerTeam(myPlayer) != GetPlayerTeam(player)) {
                enemyPlayer = player;
      
                if (handspos) {
                    *(Vector3 *)((uint64_t) get_arms(myPlayer) + 0x94) = Vector3(X1/150, Y1/150, Z1/150);
                    *(bool *)((uint64_t) get_arms(myPlayer) + 0x44) = false;
                }   
            }
        }
        if (enemyPlayer) {
            if (telekill) {
                Vector3 enemyLocation = GetPlayerLocation(enemyPlayer);

                set_position(get_transform(myPlayer),
                                 Vector3(enemyLocation.x, enemyLocation.y,
                                 enemyLocation.z - 1));
            }
        if (enemyPlayer) {
            if (masskill) {
                 Vector3 PlayerLocation = GetPlayerLocation(myPlayer);
                 set_position(get_transform(enemyPlayer),
                                 Vector3(PlayerLocation.x, PlayerLocation.y,
                                 PlayerLocation.z + 1));
            }
        }
        if(enemyPlayer){
            if (aim) {
                void* aimingdata = *(void **)((uint64_t) get_aim(myPlayer) + 0x5C);
                if (aimingdata) {
                    Vector3 angle = ToEulerRad(PlayerLook);
                    if (angle.x >= 275.0f)
                        angle.x -= 360.0f;
                    if (angle.x <= -275.0f)
                        angle.x += 360.0f;
                    float distance = Vector3::Distance(GetPlayerLocation(myPlayer), GetPlayerLocation(enemyPlayer));
                    if (usedist) {
                        if (distance < aimdist) {
                           
                            *(Vector3 *)((uint64_t)aimingdata + 0x10) = angle;
                            *(Vector3 *)((uint64_t)aimingdata + 0x1C) = angle;
                        } else {
                             enemyPlayer = NULL;
                             return;
                        }   
                        if(aim){
                            PlayerLook = Quaternion::LookRotation((GetPlayerLocation(closestEnemy2) + Vector3(0, -0.40f, 0)) - GetPlayerLocation(myPlayer), Vector3(0, 1, 0));
                        }  
                     Player_update(player);
                     }
                 }
             }
         }
     }
}

