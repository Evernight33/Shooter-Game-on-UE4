// Shoot Them up Game, All rights reserved.


#include "STUGameInstance.h"
#include "Sound/STUSoundFuncLib.h"

void USTUGameInstance::ToggleVolume() 
{
    USTUSoundFuncLib::ToggleSoundClassVolume(MasterSoundClass);
}