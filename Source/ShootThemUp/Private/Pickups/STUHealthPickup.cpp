// Shoot Them up Game, All rights reserved.


#include "Pickups/STUHealthPickup.h"
#include "Components/STUHealthComponent.h"
#include "UI/STUPlayerHUDWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthPickup, All, All);

bool ASTUHealthPickup::GivePickupTo(APawn* PlayerPawn)
{
    bool IsHealthTaken = false;

    USTUHealthComponent* const HealthComponent = USTUPlayerHUDWidget::GetComponent<USTUHealthComponent>(PlayerPawn);

    if (HealthComponent != nullptr && !HealthComponent->IsDead())
    {
        float CurrentHealth = HealthComponent->GetHealth();

        if (CurrentHealth == HealthComponent->GetMaxHealth())
        {           
            UE_LOG(LogHealthPickup, Display, TEXT("Health is already full"));
        }
        else if (CurrentHealth + HealthAmount < HealthComponent->GetMaxHealth())
        {
            IsHealthTaken = true;
            HealthComponent->SetHealth(CurrentHealth + HealthAmount);
            UE_LOG(LogHealthPickup, Display, TEXT("Character was healed for %d HP, your current Health is: %d"), 
                FMath::FloorToInt(HealthAmount), 
                FMath::FloorToInt(HealthComponent->GetHealth()));
        }
        else
        {
            IsHealthTaken = true;
            HealthComponent->SetHealth(HealthComponent->GetMaxHealth());
            UE_LOG(LogHealthPickup, Display, TEXT("Now your HP is full"));
        }
    }
    
    return IsHealthTaken;
}