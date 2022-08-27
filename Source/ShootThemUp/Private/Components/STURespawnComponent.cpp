// Shoot Them up Game, All rights reserved.

#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"
#include "UI/STUPlayerHUDWidget.h"

USTURespawnComponent::USTURespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTURespawnComponent::Respawn(int32 RespawnTime) 
{
    if (GetWorld())
    {
        RespawnCountDown = RespawnTime;
        GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &USTURespawnComponent::RespawnTimerUpdate, 1.0f, true);
    }
}

bool USTURespawnComponent::IsRespawnInProgress() const 
{
    return GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle);
}

void USTURespawnComponent::RespawnTimerUpdate() 
{
    if (--RespawnCountDown == 0)
    {
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

            const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->RespawnRequst(Cast<AController>(GetOwner()));
            }
        }
    }
}
