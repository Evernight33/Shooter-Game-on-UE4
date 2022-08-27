// Shoot Them up Game, All rights reserved.


#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Weapon/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All);

ASTURifleWeapon::ASTURifleWeapon() 
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTURifleWeapon::StartFire()
{
    UE_LOG(LogRifleWeapon, Display, TEXT("Fire!"));
   
    InitMuzzleFX();
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetMuzzleFXVisibility(false);

    UE_LOG(LogRifleWeapon, Display, TEXT("Fire stoped"));
}

void ASTURifleWeapon::BeginPlay() 
{
    Super::BeginPlay();

    check(WeaponFXComponent);
}

void ASTURifleWeapon::MakeShot()
{
    if (GetWorld() && !IsAmmoEmpty())
    {
        UE_LOG(LogRifleWeapon, Display, TEXT("Make Shot"));

        const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName);

        FVector TraceStart, TraceEnd;
        if (GetTraceData(TraceStart, TraceEnd))
        {
            FHitResult HitResult;
            MakeHit(HitResult, TraceStart, TraceEnd);
            FVector TraceFXEnd = TraceEnd;

            float Degrees = DetermineAngle(HitResult, TraceEnd, SocketTransform);

            if (HitResult.bBlockingHit && Degrees < 90)
            {
                TraceFXEnd = HitResult.ImpactPoint;
                // DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 20.0f, 0, 3.0f);
                // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 24, FColor::Red, false, 20.0f);
                MakeDamage(HitResult);
                WeaponFXComponent->PlayImpactFX(HitResult);
                SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
                // UE_LOG(LogBaseWeapon, Display, TEXT("Bone: %s"), *HitResult.BoneName.ToString());
            }
            else
            {
                // DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), TraceEnd, FColor::Red, false, 3.0f, 0, 3.0f);
                SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
            }

            DecreaseAmmo();
        }
        else
        {
            StopFire();
        }
    }
    else
    {
        StopFire();
    }
}

bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
    {
        return false;
    }

    TraceStart = ViewLocation;  // const FVector TraceStart = SocketTransform.GetLocation();
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    const FVector ShootDirection = FMath::VRandCone(
        ViewRotation.Vector(), HalfRad);  // const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    return true;
}

void ASTURifleWeapon::MakeDamage(FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();

    if (DamagedActor != nullptr)
    {
        DamagedActor->TakeDamage(DamageAmount, FDamageEvent{}, GetController(), this);
    }
}

void ASTURifleWeapon::InitMuzzleFX() 
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }

    SetMuzzleFXVisibility(true);
}

void ASTURifleWeapon::SetMuzzleFXVisibility(bool Visible) 
{
    if (MuzzleFXComponent != nullptr) 
    {
        MuzzleFXComponent->SetPaused(!Visible);
        MuzzleFXComponent->SetVisibility(Visible, true);
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd)
{
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);

    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
    }
}

AController* ASTURifleWeapon::GetController() const 
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}