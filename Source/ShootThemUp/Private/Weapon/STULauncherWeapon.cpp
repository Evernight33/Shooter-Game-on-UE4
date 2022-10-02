// Shoot Them up Game, All rights reserved.


#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void ASTULauncherWeapon::StartFire() 
{
    MakeShot();
}

void ASTULauncherWeapon::MakeShot() 
{
    // add StopFire and log in launcher and riffle
    if (GetWorld())
    {
        if (!IsAmmoEmpty())
        {
            FVector TraceStart, TraceEnd;
            GetTraceData(TraceStart, TraceEnd);

            FHitResult HitResult;
            MakeHit(HitResult, TraceStart, TraceEnd);

            const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
            const FVector Direction = (EndPoint - GetMuzzleWorldLocation());

            const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
            ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);

            if (Projectile != nullptr)
            {
                Projectile->SetShotDirection(Direction);
                Projectile->SetOwner(GetOwner());
                Projectile->FinishSpawning(SpawnTransform);
            }

            DecreaseAmmo();
            SpawnMuzzleFX();
            UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
        }
        else
        {
            UGameplayStatics::SpawnSoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
        }
    }
}