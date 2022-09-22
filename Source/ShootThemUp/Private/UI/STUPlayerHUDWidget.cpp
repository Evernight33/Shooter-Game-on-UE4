// Shoot Them up Game, All rights reserved.


#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/STURespawnComponent.h"
#include "AI/Services/STUFindEnemyService.h"
#include "Components/STUAIPerceptionComponent.h"
#include "Components/ProgressBar.h"
#include "Player/STUPlayerState.h"

float USTUPlayerHUDWidget::GetHealthPercent() const 
{
    float HealthPercent = 0.0;
    USTUHealthComponent* HealthComponent = GetComponent<USTUHealthComponent>(GetOwningPlayerPawn());

    GetComponent<USTURespawnComponent>(GetOwningPlayer()); // trash function

    if (HealthComponent != nullptr)
    {
        HealthPercent = HealthComponent->GetHealthPercent();
    }

    return HealthPercent;
}

bool USTUPlayerHUDWidget::GetWeaponUIData(FWeaponUIData& UIData) const 
{
    bool IsNotNull = false;
    USTUWeaponComponent* WeaponComponent = GetComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (WeaponComponent != nullptr)
    {
        WeaponComponent->GetWeaponUIData(UIData);
        IsNotNull = true;
    }

    return IsNotNull;
}

bool USTUPlayerHUDWidget::GetWeaponAmmoData(FAmmoData& AmmoData) const 
{
    bool IsNotNull = false;
    USTUWeaponComponent* WeaponComponent = GetComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    const auto TrashComp = GetComponent<USTUAIPerceptionComponent>(GetOwningPlayerPawn());

    if (WeaponComponent != nullptr)
    {
        WeaponComponent->GetWeaponAmmoData(AmmoData);
        IsNotNull = true;
    }

    return IsNotNull;
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    bool IsAlive = false;

    const USTUHealthComponent* HealthComponent = GetComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    IsAlive = HealthComponent && !HealthComponent->IsDead();

    return IsAlive;
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const 
{
    bool IsSpeactating = false;

    const APlayerController* Controller = GetOwningPlayer();
    IsSpeactating = Controller && Controller->GetStateName() == NAME_Spectating;

    return IsSpeactating;
}

template <typename ComponentType>
ComponentType* USTUPlayerHUDWidget::GetComponent(AActor* PlayerPawn)
{
    ComponentType* CompReturnVal = nullptr;

    if (PlayerPawn != nullptr)
    {
        UActorComponent* Component = PlayerPawn->GetComponentByClass(ComponentType::StaticClass());
        ComponentType* WeaponComponent = Cast<ComponentType>(Component);

        if (WeaponComponent != nullptr)
        {
            CompReturnVal = WeaponComponent;
        }
    }

    return CompReturnVal;
}

void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);
        OnNewPawn(GetOwningPlayerPawn());
    }
}

void USTUPlayerHUDWidget::OnHealthChanged(float Health, float HealthDelta)
{
    if (HealthDelta < 0.0f)
    {
        //OnTakeDamage();   -- Won't implement it in final version of the project
    }    

    UpdateHealthBar();
}

bool USTUPlayerHUDWidget::AreEnemies(AController* Controller1, AController* Controller2) 
{
    bool ReturnVal = false;
    if (Controller1 && Controller2 && Controller1 != Controller2)
    {
        const auto PlayerState1 = Cast<ASTUPlayerState>(Controller1->PlayerState);
        const auto PlayerState2 = Cast<ASTUPlayerState>(Controller2->PlayerState);
        ReturnVal = PlayerState1 && PlayerState2 && PlayerState1->GetTeamID() != PlayerState2->GetTeamID();
    }

    return ReturnVal;
}

int32 USTUPlayerHUDWidget::GetKillsNum() const
{
    const auto Controller = GetOwningPlayer();
    if (Controller)
    {
        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        return PlayerState ? PlayerState->GetKillsNum() : 0;
    }

    return 0;
}

FString USTUPlayerHUDWidget::FormatBullets(int32 BulletsCount) const 
{
    const int32 MaxLen = 3;
    const TCHAR PrefixSymbol = '0';

    auto BulletStr = FString::FromInt(BulletsCount);
    const auto SymbolNumToAdd = MaxLen - BulletStr.Len();

    if (SymbolNumToAdd > 0)
    {
        BulletStr = FString::ChrN(SymbolNumToAdd, PrefixSymbol).Append(BulletStr);
    }

    return BulletStr;
}

void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    USTUHealthComponent* HealthComponent = GetComponent<USTUHealthComponent>(NewPawn);

    if (HealthComponent != nullptr && !HealthComponent->OnHealthChanged.IsBoundToObject(this))
    {
        HealthComponent->OnHealthChanged.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChanged);
    }

    UpdateHealthBar();
}

void USTUPlayerHUDWidget::UpdateHealthBar()
{
    HealthProgressBar->SetFillColorAndOpacity(GetHealthPercent() > PercentColorThreshold ? GoodColor : BadColor);
}