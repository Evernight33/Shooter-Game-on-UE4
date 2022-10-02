// Shoot Them up Game, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUCoreTypes.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTUBaseWeapon();

    FOnClipEmptySignature OnClipEmpty;

    virtual void StartFire();
    virtual void StopFire();

    void ChangeClip();
    bool CanReload() const;
    bool IsFiring() const;
    
    FAmmoData GetAmmoData() const;
    FWeaponUIData GetUIData() const;

    bool TryToAddAmmo(int32 ClipsAmount);
    bool IsAmmoEmpty() const;
    bool IsAmmoFull() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName MuzzleSocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 1500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FWeaponUIData UIData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* MuzzleFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* FireSound;

	virtual void BeginPlay() override;

	virtual void MakeShot();

    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    APlayerController* GetPlayerController() const;
    FVector GetMuzzleWorldLocation() const;
	virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector TraceEnd);
    float DetermineAngle(FHitResult& HitResult, const FVector& TraceEnd, const FTransform& SocketTransform);

    void DecreaseAmmo();
    bool IsClipEmpty() const;
    void LogAmmo();

    UNiagaraComponent* SpawnMuzzleFX();

private:
    FTimerHandle ShotTimerHandle;
    FAmmoData CurrentAmmo;
    bool FireInProgress = false;
};
