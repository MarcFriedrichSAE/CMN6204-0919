#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h" 
#pragma endregion

#pragma region project include
#include "BaseCharacter.generated.h" 
#pragma endregion

#pragma region forward decleration
class UCapsuleComponent;
class ABullet;
#pragma endregion

UCLASS()
/// <summary>
/// base character class
/// </summary>
class WAVESHOOTERVR_API ABaseCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	ABaseCharacter();
#pragma endregion

#pragma region public override function
	/// <summary>
	/// called after actor was destroyed
	/// </summary>
	virtual void Destroyed() override
	{
		// parent actor was destroyed
		Super::Destroyed();
	}
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Required")
	/// <summary>
	/// bullet class to spawn
	/// </summary>
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "BaseCharacter")
	/// <summary>
	/// movement speed in cm per second
	/// </summary>
	float MovementSpeed = 300.0f;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "BaseCharacter")
	/// <summary>
	/// capsule collision component
	/// </summary>
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "BaseCharacter")
	/// <summary>
	/// mesh of player
	/// </summary>
	USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "BaseCharacter")
	// <summary>
	/// spawn point of bullet
	/// </summary>
	USceneComponent* SpawnPoint = nullptr;

	UPROPERTY(ReplicatedUsing = UpdateHealth_RepNotify)
	/// <summary>
	/// health
	/// </summary>
	float Health = 100.0f;
#pragma endregion

#pragma region UFUNCTION
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
	/// <summary>
	/// update health
	/// </summary>
	/// <param name="HealthValue">health</param>
	void UpdateHealth(float HealthValue);

	UFUNCTION()
	/// <summary>
	/// health rep notify
	/// </summary>
	void UpdateHealth_RepNotify();
#pragma endregion

#pragma region public function
	/// <summary>
	/// take damage
	/// </summary>
	/// <param name="_damage">damage</param>
	void TakeCharacterDamage(float _damage);
#pragma endregion
};