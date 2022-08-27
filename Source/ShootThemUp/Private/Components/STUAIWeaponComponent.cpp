// Shoot Them up Game, All rights reserved.


#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire() 
{
    if (CanFire())
    {
        if (CurrentWeapon->IsAmmoEmpty())
        {
            NextWeapon();
        }
        else
        {
            CurrentWeapon->StartFire();
        }
    }
}

void USTUAIWeaponComponent::NextWeapon() 
{
    if (CanEquip())
    {
        int32 NextIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

        while (NextIndex != CurrentWeaponIndex)
        {
            if (!Weapons[NextIndex]->IsAmmoEmpty())
            {
                break;
            }
            else
            {
                NextIndex = (NextIndex + 1) % Weapons.Num();
            }
        }

        if (CurrentWeaponIndex != NextIndex)
        {
            CurrentWeaponIndex = NextIndex;
            EquipWeapon(CurrentWeaponIndex);
        }
    }
}