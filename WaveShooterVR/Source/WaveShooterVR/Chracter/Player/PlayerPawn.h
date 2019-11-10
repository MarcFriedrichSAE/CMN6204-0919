#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#pragma endregion

#pragma region project include
#include "PlayerPawn.generated.h"
#pragma endregion

#pragma region forward decleration
class UCapsuleComponent;
class UCameraComponent;
class ABullet;
#pragma endregion

UCLASS()
/// <summary>
/// player class
/// </summary>
class WAVESHOOTERVR_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	APlayerPawn();
#pragma endregion

#pragma region public override function
	/// <summary>
	/// update every frame
	/// </summary>
	/// <param name="DeltaTime">time since last frame</param>
	virtual void Tick(float DeltaTime) override;
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Required")
	/// <summary>
	/// bullet class to spawn
	/// </summary>
	TSubclassOf<ABullet> BulletClass;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// capsule collision component
	/// </summary>
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// spawn point of bullet
	/// </summary>
	USceneComponent* SpawnPoint = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// mesh of player
	/// </summary>
	USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// rotation origin of camera
	/// </summary>
	USceneComponent* CameraRoot = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// camera
	/// </summary>
	UCameraComponent* Camera = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	/// <summary>
	/// movement speed in cm per second
	/// </summary>
	float MovementSpeed = 450.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	/// <summary>
	/// rotation speed in degree per seconds
	/// </summary>
	float RotationSpeed = 180.0f;
#pragma endregion

#pragma region UFUNCTION
	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// move player
	/// </summary>
	/// <param name="ForwardBack">forward and back value</param>
	/// <param name="LeftRight">left and right value</param>
	void Move(float ForwardBack, float LeftRight);

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// rotate player and camera
	/// </summary>
	/// <param name="LeftRight">left and right value</param>
	/// <param name="UpDown">up and down value</param>
	void Rotate(float LeftRight, float UpDown);

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// shoot
	/// </summary>
	void Shoot();
#pragma endregion

protected:
#pragma region protected override function
	/// <summary>
	/// called at begin play
	/// </summary>
	virtual void BeginPlay() override;
#pragma endregion
};