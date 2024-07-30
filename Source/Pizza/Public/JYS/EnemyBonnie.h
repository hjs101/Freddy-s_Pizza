// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JYS/AllEnemyCharacter.h"
#include "HJS/FreddyPlayer.h"
#include "EnemyBonnie.generated.h"

enum class EBonnieState : uint8
{
	Room0,
	Room1,
	Room2,
	Room3,

};

UCLASS()
class PIZZA_API AEnemyBonnie : public AAllEnemyCharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyBonnie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// EBonnieState State = EBonnieState::Room3;

	void Move(EBonnieState MoveState);

private:
	// ���� �迭�� ����
	UPROPERTY(EditAnywhere)
	FVector RoomPositions[4];


	// �̵� ����
	EBonnieState State;

	// 4.97�ʸ��� �̵� Ÿ�̸�
	FTimerHandle MoveTimerHandle;

	// �÷��̾� ��ü ����
	AFreddyPlayer* Player;
	
	void TickRoom0(const float& DeltaTime);
	void TickRoom1(const float& DeltaTime);
	void TickRoom2(const float& DeltaTime);
	void TickRoom3(const float& DeltaTime);

	// �̵� �� ���� ���� �̱�
	UFUNCTION()
	void AttemptMove();
	UFUNCTION()
	int32 GetRandomNumber();

	// �÷��̾� ���� Ȯ�� �Լ�
	bool ShouldMoveToRoom3();

	// 1���濡�� 3���� �̵� 
	// ��ǥ ��ġ
	FVector TargetLocation;
	// �̵� �ӵ�Z
	float MoveSpeed;
	// �̵� ������ ����
	bool bIsMovingToRoom3;
};
