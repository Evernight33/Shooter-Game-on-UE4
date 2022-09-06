// Shoot Them up Game, All rights reserved.


#include "UI/STUGoToMenuUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "STUGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogGoToMenuUserWidget, All, All)

void USTUGoToMenuUserWidget::NativeOnInitialized() 
{
    Super::NativeOnInitialized();

    if (GoToMenuButton)
    {
        GoToMenuButton->OnClicked.AddDynamic(this, &USTUGoToMenuUserWidget::OnGoToMenu);
    }
}

void USTUGoToMenuUserWidget::OnGoToMenu()
{
    if (GetWorld())
    {
        const auto STUGameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
        if (STUGameInstance)
        {
            if (!STUGameInstance->GetMainMenuLevelName().IsNone())
            {
                UGameplayStatics::OpenLevel(this, STUGameInstance->GetMainMenuLevelName());
            }
            else
            {
                UE_LOG(LogGoToMenuUserWidget, Error, TEXT("Menu level name is NONE"));
            }
        }
    }
}