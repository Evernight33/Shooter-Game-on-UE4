// Shoot Them up Game, All rights reserved.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon() : DefaultAmmo{15, 20, false}
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

    check(WeaponMesh);
    checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets couldn't be less than zero"));
    checkf(DefaultAmmo.Clips > 0, TEXT("Bullets couldn't be less than zero"));
    CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::StartFire() 
{
    FireInProgress = true;
}

void ASTUBaseWeapon::StopFire()
{
    FireInProgress = false;
}

bool ASTUBaseWeapon::IsFiring() const 
{
    return FireInProgress;
}

void ASTUBaseWeapon::MakeShot() 
{

}

APlayerController* ASTUBaseWeapon::GetPlayerController() const
{
    ACharacter* Player = Cast<ACharacter>(GetOwner());
    if (Player != nullptr)
    {
        APlayerController* Controller = Player->GetController<APlayerController>();
        if (Controller != nullptr)
        {
            return Controller;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const 
{
    const auto STUCharacter = Cast<ACharacter>(GetOwner());
    if (!STUCharacter)
    {
        return false;
    }

    if (STUCharacter->IsPlayerControlled())
    {
        const APlayerController* Controller = GetPlayerController();
        if (Controller != nullptr)
        {
            Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
            return true;
        }
        else 
        {
            return false;
        }
    }
    else
    {
        ViewLocation = GetMuzzleWorldLocation();
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
    }

   return true;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const 
{
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const 
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
    {
        return false;
    }

    TraceStart = ViewLocation;  // const FVector TraceStart = SocketTransform.GetLocation();
    const FVector ShootDirection = ViewRotation.Vector(); // const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    return true;
}

void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector TraceEnd) 
{
    if (GetWorld())
    {
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(GetOwner());
        CollisionParams.bReturnPhysicalMaterial = true;

        GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
    }   
}

float ASTUBaseWeapon::DetermineAngle(FHitResult& HitResult, const FVector& TraceEnd, const FTransform& SocketTransform)
{
    const FVector ActualTraceEnd = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector MuzzleDirection = (ActualTraceEnd - GetMuzzleWorldLocation()).GetSafeNormal();

    float DotProd = FVector::DotProduct(SocketTransform.GetRotation().GetForwardVector(), MuzzleDirection);
    float Angle = FMath::Acos(DotProd);
    float Degree = FMath::RadiansToDegrees(Angle);

    return Degree;
}

void ASTUBaseWeapon::DecreaseAmmo()
{
    if (CurrentAmmo.Bullets == 0)
    {
        UE_LOG(LogBaseWeapon, Warning, TEXT("No more bullets!"));
        return;
    }

    CurrentAmmo.Bullets--;
    //LogAmmo();

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        StopFire();
        OnClipEmpty.Broadcast(this);
    }
}

bool ASTUBaseWeapon::IsAmmoEmpty() const 
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

bool ASTUBaseWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

void ASTUBaseWeapon::ChangeClip() 
{
    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0)
        {
            UE_LOG(LogBaseWeapon, Warning, TEXT("Clip is empty!"));
            return;
        }
    }  

    CurrentAmmo.Clips--;
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
    //UE_LOG(LogBaseWeapon, Display, TEXT("------Change Clip-----"))
}

void ASTUBaseWeapon::LogAmmo() 
{
    FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + " / ";
    AmmoInfo += CurrentAmmo.Infinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}


FAmmoData ASTUBaseWeapon::GetAmmoData() const
{
    return CurrentAmmo;
}

FWeaponUIData ASTUBaseWeapon::GetUIData() const
{
    return UIData;
}

bool ASTUBaseWeapon::IsAmmoFull() const 
{
    return CurrentAmmo.Clips == DefaultAmmo.Clips && CurrentAmmo.Bullets == DefaultAmmo.Bullets;
}

bool ASTUBaseWeapon::TryToAddAmmo(int32 ClipsAmount) 
{
    bool IsAmmoAdded = false;

    if (!CurrentAmmo.Infinite && !IsAmmoFull() && ClipsAmount >= 0) 
    {
        IsAmmoAdded = true;

        if (IsAmmoEmpty())
        {
            CurrentAmmo.Clips = FMath::Clamp(ClipsAmount, 0, DefaultAmmo.Clips + 1);
            OnClipEmpty.Broadcast(this);
            UE_LOG(LogBaseWeapon, Display, TEXT("Ammo was empty"));
        }
        else if (CurrentAmmo.Clips < DefaultAmmo.Clips)
        {
            const auto NextClipsAmount = CurrentAmmo.Clips + ClipsAmount;
            if (DefaultAmmo.Clips - NextClipsAmount >= 0)
            {
                CurrentAmmo.Clips = NextClipsAmount;
                UE_LOG(LogBaseWeapon, Display, TEXT("Clips were added"));
            }
            else
            {
                CurrentAmmo.Clips = DefaultAmmo.Clips;
                CurrentAmmo.Bullets = DefaultAmmo.Bullets;
                UE_LOG(LogBaseWeapon, Display, TEXT("Ammo is full now"));
            }
        }
        else
        {
            CurrentAmmo.Bullets = DefaultAmmo.Bullets;
            UE_LOG(LogBaseWeapon, Display, TEXT("Bullets were added"));
        }
    }

    return IsAmmoAdded;
}

UNiagaraComponent* ASTUBaseWeapon::SpawnMuzzleFX() 
{
    UNiagaraComponent* ReturnComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
    (
        MuzzleFX, //
        WeaponMesh, //
        MuzzleSocketName, //
        FVector::ZeroVector, //
        FRotator::ZeroRotator, //
        EAttachLocation::SnapToTarget, true
    );

    return ReturnComponent;
}