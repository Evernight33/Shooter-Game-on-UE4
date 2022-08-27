// Shoot Them up Game, All rights reserved.


#include "UI/STUSpectatorWidget.h"
#include "UI/STUPlayerHUDWidget.h"
#include "Components/STURespawnComponent.h"

bool USTUSpectatorWidget::GetRespawnTime(int32& CountDownTime) const 
{
    bool ReturnVal = false;

    const auto RespawnComponent =  USTUPlayerHUDWidget::GetComponent<USTURespawnComponent>(GetOwningPlayer());
    if (RespawnComponent && RespawnComponent->IsRespawnInProgress())
    {
        CountDownTime = RespawnComponent->GetRespawnCountDown();
        ReturnVal = true;
    }

    return ReturnVal;
}