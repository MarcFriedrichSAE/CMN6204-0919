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
class AStaticMeshActor;
class UMotionControllerComponent;
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
	/// camera
	/// </summary>
	UCameraComponent* Camera = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// left hand static mesh
	/// </summary>
	UStaticMeshComponent* LeftHand = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Player")
	/// <summary>
	/// right hand static mesh
	/// </summary>
	UStaticMeshComponent* RightHand = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	/// <summary>
	/// movement speed in cm per second
	/// </summary>
	float MovementSpeed = 450.0f;
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

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// set weapon switch target
	/// </summary>
	/// <param name="OtherComp">target static mesh component to set</param>
	void SetWeaponTarget(UStaticMeshComponent* OtherComp);

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// collide weapon with other actor
	/// </summary>
	/// <param name="OtherActor">collide actor</param>
	void CollideWeapon(AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Player")
	/// <summary>
	/// spawn cube
	/// </summary>
	/// <param name="Cube">cube subclass to spawn</param>
	/// <param name="Force">force to add</param>
	void SpawnCube(TSubclassOf<AStaticMeshActor> Cube, float Force);

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
	/// <param name="_location">location to move to</param>
	void Move_Client(FVector _location);

	UFUNCTION(Server, Unreliable, WithValidation)
	/// <summary>
	/// set tracking devices transform on server
	/// </summary>
	/// <param name="_locationCam">relative location of camera</param>
	/// <param name="_angle">relative yaw angle of camera</param>
	/// <param name="_locationLeft">relative location of left hand</param>
	/// <param name="_rotationLeft">relative rotation of left hand</param>
	/// <param name="_locationRight">relative location of right hand</param>
	/// <param name="_rotationRight">relative rotation of right hand</param>
	void SetTransforms_Server(FVector _locationCam, float _angle,
		FVector _locationLeft, FRotator _rotationLeft, FVector _locationRight, FRotator _rotationRight);

	UFUNCTION(NetMulticast, Unreliable)
	/// <summary>
	/// set tracking devices transform on client
	/// </summary>
	/// <param name="_locationCam">relative location of camera</param>
	/// <param name="_angle">relative yaw angle of camera</param>
	/// <param name="_locationLeft">relative location of left hand</param>
	/// <param name="_rotationLeft">relative rotation of left hand</param>
	/// <param name="_locationRight">relative location of right hand</param>
	/// <param name="_rotationRight">relative rotation of right hand</param>
	void SetTransforms_Client(FVector _locationCam, float _angle,
		FVector _locationLeft, FRotator _rotationLeft, FVector _locationRight, FRotator _rotationRight);

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

	/// <summary>
	/// range weapon active
	/// </summary>
	bool m_range = true;

	/// <summary>
	/// camera angle to rotate camera to
	/// </summary>
	float m_camAngleRotTo = 0.0f;
#pragma endregion

#pragma region private variable
	/// <summary>
	/// position to move actor to
	/// </summary>
	FVector m_actMoveTo = FVector();

	/// <summary>
	/// position to move camera to
	/// </summary>
	FVector m_camMoveTo = FVector();

	/// <summary>
	/// position to move left hand to
	/// </summary>
	FVector m_leftMoveTo = FVector();

	/// <summary>
	/// postition to move right hand to
	/// </summary>
	FVector m_rightMoveTo = FVector();

	/// <summary>
	/// angle to rotate left hand to
	/// </summary>
	FRotator m_leftRotTo = FRotator();

	/// <summary>
	/// angle to rotate right hand to
	/// </summary>
	FRotator m_rightRotTo = FRotator();

	/// <summary>
	/// right hand location to calculate throw velocity
	/// </summary>
	TArray<FVector> m_rightHandLocations;

	/// <summary>
	/// all teleport locations
	/// </summary>
	TArray<ATeleportPoint*> m_teleports;
#pragma endregion

#pragma region private pointer
	/// <summary>
	/// left motion controller
	/// </summary>
	UMotionControllerComponent* m_pLeftController = nullptr;

	/// <summary>
	/// right motion controller
	/// </summary>
	UMotionControllerComponent* m_pRightController = nullptr;

	/// <summary>
	/// weapon to switch target
	/// </summary>
	UStaticMeshComponent* m_pWeaponTarget = nullptr;

	/// <summary>
	/// teleport target reference
	/// </summary>
	ATeleportPoint* m_pTeleportTarget = nullptr;

	/// <summary>
	/// active teleport reference
	/// </summary>
	ATeleportPoint* m_pActiveTeleport = nullptr;
#pragma endregion

private:
#pragma region private primitive variable
	/// <summary>
	/// update frequency for network movement in seconds
	/// </summary>
	float m_netUpdateFrequency = 0.2f;

	/// <summary>
	/// time until update movement via network
	/// </summary>
	float m_netUpdateTimer = 0.0f;
#pragma endregion

#pragma region private function
	void SetCapsule();
#pragma endregion
};