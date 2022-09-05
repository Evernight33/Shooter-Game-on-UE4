// Shoot Them up Game, All rights reserved.


#include "Menu/UI/STUMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "STUGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUMenuWidget, All, All)

void USTUMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &USTUMenuWidget::OnStartGame);
    }
}

void USTUMenuWidget::OnStartGame() 
{
    if (GetWorld())
    {
        const auto STUGameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
        if (STUGameInstance)
        {
            if (!STUGameInstance->GetStartupLevelName().IsNone())
            {
                UGameplayStatics::OpenLevel(this, STUGameInstance->GetStartupLevelName());
            }
            else
            {
                UE_LOG(LogSTUMenuWidget, Error, TEXT("Level name is NONE"));
            }           
        }   
    }   
}