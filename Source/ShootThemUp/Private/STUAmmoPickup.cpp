// Shoot Them up Game, All rights reserved.


#include "STUAmmoPickup.h"
#include "Components/STUWeaponComponent.h"
#include "Components/STUHealthComponent.h"
#include "UI/STUPlayerHUDWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogAmmoPickup, All, All);

bool ASTUAmmoPickup::GivePickupTo(APawn* PlayerPawn) 
{
    bool ReturnVal = false;
    USTUHealthComponent* const HealthComponent = USTUPlayerHUDWidget::GetComponent<USTUHealthComponent>(PlayerPawn);

    if (HealthComponent && !HealthComponent->IsDead())
    {    
        USTUWeaponComponent* const WeaponComponent = USTUPlayerHUDWidget::GetComponent<USTUWeaponComponent>(PlayerPawn);

        if (WeaponComponent != nullptr)
        {
            ReturnVal = WeaponComponent->TryToAddAmmo(WeaponType, ClipsAmount);
        }
    }

    UE_LOG(LogAmmoPickup, Display, TEXT("Ammo was taken"));
    return ReturnVal;
}