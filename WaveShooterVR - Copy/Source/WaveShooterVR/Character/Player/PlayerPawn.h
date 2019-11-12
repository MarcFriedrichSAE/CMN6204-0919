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
class ATeleportPoint;
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
	/// shoot a bullet
	/// </summary>
	void Shoot();

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// show teleport locations
	/// </summary>
	void ShowTeleport();

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// teleport to location
	/// </summary>
	void Teleport();

	UFUNCTION(NetMulticast, Reliable)
	/// <summary>
	/// spawn player on client
	/// </summary>
	/// <param name="_location">location to spawn</param>
	void SpawnPlayer_Client(FVector _location);

	UFUNCTION(Server, Unreliable, WithValidation)
	/// <summary>
	/// move player on server
	/// </summary>
	/// <param name="_location">location to move to</param>
	void Move_Server(FVector _location);

	UFUNCTION(NetMulticast, Unreliable)
	/// <summary>
	/// move player on client
	/// </summary>
	/// <param name="_location"></param>
	void Move_Client(FVector _location);

	UFUNCTION(Server, Unreliable, WithValidation)
	/// <summary>
	/// rotate player on server
	/// </summary>
	/// <param name="_angle">angle to rotate to</param>
	void Rotate_Server(float _angle);

	UFUNCTION(NetMulticast, Unreliable)
	/// <summary>
	/// rotate player on client
	/// </summary>
	/// <param name="_angle">angle to rotate to</param>
	void Rotate_Client(float _angle);

	UFUNCTION(Server, Reliable, WithValidation)
	/// <summary>
	/// shoot bullet on server
	/// </summary>
	/// <param name="_location">spawn location</param>
	/// <param name="_rotation">spawn rotation</param>
	void Shoot_Server(FVector _location, FRotator _rotation);

	UFUNCTION(Server, Reliable, WithValidation)
	/// <summary>
	/// teleport player on server
	/// </summary>
	/// <param name="_pTeleport">teleport point to teleport to</param>
	/// <param name="_pCurrent">current active teleport point of player</param>
	void Teleport_Server(ATeleportPoint* _pTeleport, ATeleportPoint* _pCurrent);

	UFUNCTION(Client, Reliable)
	/// <summary>
	/// teleport player on client
	/// </summary>
	/// <param name="_pTeleport">teleport point to teleport to</param>
	void Teleport_Client(ATeleportPoint* _pTeleport);
#pragma endregion

protected:
#pragma region protected override function
	/// <summary>
	/// called at begin play
	/// </summary>
	virtual void BeginPlay() override;
#pragma endregion

private:
#pragma region private primitive variable
	/// <summary>
	/// show teleport locations active
	/// </summary>
	bool m_teleportActive = false;
#pragma endregion

#pragma region private variable
	/// <summary>
	/// all teleport locations
	/// </summary>
	TArray<ATeleportPoint*> m_teleports;
#pragma endregion

#pragma region private pointer
	/// <summary>
	/// teleport target reference
	/// </summary>
	ATeleportPoint* m_pTeleportTarget = nullptr;

	/// <summary>
	/// active teleport reference
	/// </summary>
	ATeleportPoint* m_pActiveTeleport = nullptr;
#pragma endregion
};