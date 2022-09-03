// Shoot Them up Game, All rights reserved.


#include "Player/STUPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUPlayerState, All, All);

void ASTUPlayerState::LogInfo() const
{
    UE_LOG(LogSTUPlayerState, Display, TEXT("TeamID: %i, Kills:%i, Deaths: %i"), TeamId, KillsNum, DeathsNum);
}

int32 ASTUPlayerState::GetKillsNum() const 
{
    return KillsNum;
}

int32 ASTUPlayerState::GetDeathsNum() const 
{
    return DeathsNum;
}