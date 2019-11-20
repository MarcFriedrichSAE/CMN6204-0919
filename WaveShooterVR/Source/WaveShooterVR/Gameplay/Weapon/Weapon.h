#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#pragma endregion

#pragma region project include
#include "Weapon.generated.h"
#pragma endregion

UCLASS()
/// <summary>
/// weapon switch class
/// </summary>
class WAVESHOOTERVR_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	AWeapon();
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// range weapon static mesh component
	/// </summary>
	UStaticMeshComponent* RangeWeapon = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// melee weapon static mesh component
	/// </summary>
	UStaticMeshComponent* MeleeWeapon = nullptr;
#pragma endregion
};