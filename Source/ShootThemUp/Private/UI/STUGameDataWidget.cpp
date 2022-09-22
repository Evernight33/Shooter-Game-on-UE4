// Shoot Them up Game, All rights reserved.


#include "UI/STUGameDataWidget.h"
#include "STUPlayerState.h"
#include "STUGameModeBase.h"

int32 USTUGameDataWidget::GetCurrentRoundNum() const
{
    return GetSTUGameModeBase() ? GetSTUGameModeBase()->GetCurrentRound() : NULL;
}

int32 USTUGameDataWidget::GetTotalRoundsNum() const 
{
    return GetSTUGameModeBase() ? GetSTUGameModeBase()->GetGameData().RoundsNum : NULL;
}

int32 USTUGameDataWidget::GetRoundSecondsRemaining() const 
{
    return GetSTUGameModeBase() ? GetSTUGameModeBase()->GetCurrentSecRemaining() : NULL;
}

ASTUGameModeBase* USTUGameDataWidget::GetSTUGameModeBase() const 
{
    return GetWorld() ? Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode()) : nullptr;
}

ASTUPlayerState* USTUGameDataWidget::GetSTUPlayerState() const 
{
    return GetOwningPlayerState() ? Cast<ASTUPlayerState>(GetOwningPlayerState()) : nullptr;
}