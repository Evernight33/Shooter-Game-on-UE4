// Shoot Them up Game, All rights reserved.


#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STUReloadAnimNotify.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our character can hold only %i weapon items"), WeaponNum);
    SpawnWeapons();
    InitAnimations();
    EquipWeapon(CurrentWeaponIndex);

}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;

    for (auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Weapons.Empty();
    Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (Character != nullptr && GetWorld() != nullptr)
    {
        for (auto OneWeaponData : WeaponData)
        {
           auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);

           if (Weapon == nullptr)
           {
               continue;
           }

           Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnEmptyClip);

           Weapon->SetOwner(Character);
           Weapons.Add(Weapon);

           AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
        }
    }
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName) 
{
    if (Weapon != nullptr && SceneComponent != nullptr)
    {
        FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
        Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
    }
}

void USTUWeaponComponent::StartFire() 
{
    if (CanFire())
    {
        CurrentWeapon->StartFire();
    } 
}

void USTUWeaponComponent::StopFire()
{
    if (CurrentWeapon != nullptr)
    {
        CurrentWeapon->StopFire();
    }
}

void USTUWeaponComponent::EquipWeapon(int32 WeaponIndex) 
{
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("InvalidWeaponIndex"));
        return;
    }

    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (CurrentWeapon != nullptr)
    {
        CurrentWeapon->Zoom(false);
        CurrentWeapon->StopFire();
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    if (Character != nullptr)
    {
        CurrentWeapon = Weapons[WeaponIndex];
        //CurrentReloadAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
        const auto CurrentWeaponData = WeaponData.FindByPredicate([&] (const FWeaponData& Data) { return Data.WeaponClass == CurrentWeapon->GetClass(); });
        CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
        EquipAnimInProgress = true;
        PlayAnimMontage(EquipAnimMontage);
    }
}

void USTUWeaponComponent::NextWeapon() 
{
    if (CanEquip() && ReloadAnimInProgress == false)
    {
        CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
        EquipWeapon(CurrentWeaponIndex);
    }   
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (CurrentWeapon != nullptr)
    {
        Character->PlayAnimMontage(Animation);
    }
}

void USTUWeaponComponent::InitAnimations() 
{
    if (EquipAnimMontage != nullptr)
    {
        USTUEquipFinishedAnimNotify* EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);

        if (EquipFinishedNotify != nullptr)
        {
            EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
        }
        else
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
            checkNoEntry();
        }

        for (auto SingleWeaponData : WeaponData)
        {
            USTUReloadFinishedAnimNotify* ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(SingleWeaponData.ReloadAnimMontage);

            if (ReloadFinishedNotify != nullptr)
            {
                ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
            }
            else
            {
                UE_LOG(LogWeaponComponent, Error, TEXT("Reload anim notify is forgotten to set"));
                checkNoEntry();
            }
        }       
    }
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (Character != nullptr && Character->GetMesh() == MeshComponent)
    {
        UE_LOG(LogWeaponComponent, Display, TEXT("Equip Finished"));
        EquipAnimInProgress = false;
    }   
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent) 
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (Character != nullptr && Character->GetMesh() == MeshComponent) 
    {
        UE_LOG(LogWeaponComponent, Display, TEXT("Reload Finished"));
        ReloadAnimInProgress = false;
    }
}

bool USTUWeaponComponent::CanFire() const 
{
    if (CurrentWeapon != nullptr && !EquipAnimInProgress && !ReloadAnimInProgress) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}

bool USTUWeaponComponent::CanEquip() const 
{
    return EquipAnimInProgress == false;
}

bool USTUWeaponComponent::CanReload() const
{
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::Reload() 
{
    ChangeClip();
}

void USTUWeaponComponent::OnEmptyClip(ASTUBaseWeapon* CurrentWeaponReloaded)
{
    if (CurrentWeaponReloaded != nullptr)
    {
        if (CurrentWeapon == CurrentWeaponReloaded)
        {
            ChangeClip();
        }
        else
        {
            for (const auto Weapon : Weapons)
            {
                if (Weapon == CurrentWeaponReloaded)
                {
                    Weapon->ChangeClip();
                }
            }
        }
    }
}

void USTUWeaponComponent::ChangeClip() 
{
    if (CanReload())
    {
        CurrentWeapon->StopFire();
        CurrentWeapon->ChangeClip();
        ReloadAnimInProgress = true;
        PlayAnimMontage(CurrentReloadAnimMontage);
    }
}

bool USTUWeaponComponent::IsFiring() const
{
    return CurrentWeapon && CurrentWeapon->IsFiring();
}

bool USTUWeaponComponent::GetWeaponUIData(FWeaponUIData& UIData) const 
{
    if (CurrentWeapon != nullptr) 
    {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }

    return false;
}

bool USTUWeaponComponent::GetWeaponAmmoData(FAmmoData& AmmoData) const 
{
    if (CurrentWeapon != nullptr)
    {
        AmmoData = CurrentWeapon->GetAmmoData();
        return true;
    }

    return false;
}

bool USTUWeaponComponent::TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount) 
{
    bool IsWeaponFound = false;

    for (const auto Weapon : Weapons)
    {
        if (Weapon != nullptr && Weapon->IsA(WeaponType))
        {
            return Weapon->TryToAddAmmo(ClipsAmount);
            IsWeaponFound = true;
        }
    }

    return false;
}

bool USTUWeaponComponent::NeedAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType)
{
    bool IsWeaponFound = false;

    for (const auto Weapon : Weapons)
    {
        if (Weapon != nullptr && Weapon->IsA(WeaponType))
        {
            return !Weapon->IsAmmoFull();
        }
    }

    return false;
}

bool USTUWeaponComponent::GetIsReloadAnimInProgress() const 
{
    return ReloadAnimInProgress;
}

void USTUWeaponComponent::Zoom(bool Enabled)
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Zoom(Enabled);
    }
}