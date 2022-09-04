// Shoot Them up Game, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUCoreTypes.h"
#include "STUPlayerHUDWidget.generated.h"

class USTUWeaponComponent;
class USTUHealthComponent;
class USTUAIPerceptionComponent;
class USTUFindEnemyService;

UCLASS()
class SHOOTTHEMUP_API USTUPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool GetWeaponUIData(FWeaponUIData& UIData) const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool GetWeaponAmmoData(FAmmoData& AmmoData) const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool IsPlayerAlive() const;

    UFUNCTION(BlueprintCallable, Category = "UI")
    bool IsPlayerSpectating() const;    

    template <typename ComponentType>
    static ComponentType* GetComponent(AActor* PlayerPawn);

    bool static AreEnemies(AController* Controller1, AController* Controller2);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnTakeDamage();

    virtual void NativeOnInitialized() override;

private:
    void OnHealthChanged(float Health, float HealthDelta);
    void OnNewPawn(APawn* NewPawn);
};
