#include "Chica.h"
#include "LocationState.h"
#include "../Public/HJS/FreddyPlayer.h"
#include "AILevel.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
AChica::AChica()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = ELocationState::IDLE;

}

// Called when the game starts or when spawned
void AChica::BeginPlay()
{
	Super::BeginPlay();
	
	// UAILevel ������Ʈ ��������
	AILevelComp = FindComponentByClass<UAILevel>();
}

// Called every frame
void AChica::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// ��� ���� Ȯ�� (AlLevel �� �ִ� SetLevel())
	AILevelComp->SetLevel(this);
}

void AChica::SetUpLocation(ELocationState State, float DeltaTime, FName Tag)
{
	switch (State)
	{
	case ELocationState::IDLE:	Idle(DeltaTime, Tag);
		break;
	case ELocationState::MOVE:	Move(Tag);
		break;
	case ELocationState::ATTACK:	Attack();
		break;
	case ELocationState::CUPCAKE:	Cupcake();
		break;
	default:
		break;
	}
}

// Called to bind functionality to input
void AChica::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ��Ʈ�� Ű �Է� ����
	PlayerInputComponent->BindAction("MoveToRoom1", IE_Pressed, this, &AChica::OnCtrlKeyPressed);

}

FName AChica::GetCurrentLocationTag()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->ActorHasTag("Room1"))
		{
			return FName("Room1");
		}
		else if (Actor->ActorHasTag("Room3"))
		{
			return FName("Room3");
		}
		else if (Actor->ActorHasTag("Room4"))
		{
			return FName("Room4");
		}
		else if (Actor->ActorHasTag("Room6"))
		{
			return FName("Room6");
		}
		else if (Actor->ActorHasTag("Room8"))
		{
			return FName("Room8");
		}
	}

	return FName("");
}

void AChica::Idle(float DeltaTime, FName Tag)
{
	// ���� ��ġ == room1 || room3 || room4 || room6 || room8 ����
	
	// 4.98�ʸ��� AILevel�� �ִ� RandomMove() ȣ�� && Move�� ��������
	CurrentTime += DeltaTime;
	if (CurrentTime > MovableTime)
	{
		AILevelComp->RandomMove(this, DeltaTime, Tag); // RandomMove�ȿ� �������� ����

		CurrentTime = 0.f;
	}
}

void AChica::Move(FName Tag) // ������ �Ѱ� ������ 1,3,4�θ� �̵�
{
	FName CurrentTag = GetCurrentLocationTag();

	// room1 || room4 -> room3 ����
	if (CurrentTag == "Room1" || CurrentTag == "Room4")
	{
		MoveToTaggedLocation(FName("Room3"));
	}
	// room3 -> room1 || room4 || room6 ����
	else if (CurrentTag == "Room3")
	{
		TArray<FName> RoomTags = { FName("Room1"), FName("Room4"), FName("Room6") };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		MoveToTaggedLocation(RoomTags[RandomIndex]);
	}
	// room6 -> room3 || room8 ����
	else if (CurrentTag == "Room6")
	{
		TArray<FName> RoomTags = { FName("Room3"), FName("Room8") };
		int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

		MoveToTaggedLocation(RoomTags[RandomIndex]);

		// �߼Ҹ�

		//	����, ������ ON -> room1�� �̵� (�����̵�X)
		if(bIsFlashlightOn) 
		{
			// ������ ON && Door: Close -> room8�� �̵�
			if (!bIsDoorOpen)
			{
				MoveToTaggedLocation(FName("Room8"));
			}
			MoveToTaggedLocation(FName("Room1"));
		}
	}
	// room8 -> room6 ����
	else if (CurrentTag == "Room8")  // <<<<<(���� Door�� ���� �� ��ġ���� �޾ƿ;� ��)
	{
		Cast<AFreddyPlayer>
		//�� �÷��̾� ��ġ == Door && ������ ON : ����������(����) 
		if (bIsDoorOpen && bIsFlashlightOn)
		{
			CurrentState = ELocationState::ATTACK;
		}
		//�� �÷��̾� ��ġ == Door && bCLOSE == true (���� �ð����� CLOSE �� Ȯ�������� 1,3,4 �� �̵�)
		if (!bIsDoorOpen)
		{	
			for (float cnt = 0.f; cnt < 6.f; cnt++)
			{
				if (cnt > MovableTime)
				{
					TArray<FName> RoomTags = { FName("Room1"), FName("Room3"), FName("Room4") };
					int32 RandomIndex = FMath::RandRange(0, RoomTags.Num() - 1);

					MoveToTaggedLocation(RoomTags[RandomIndex]);
				}
			}
		}
		
		//�� �÷��̾� ��ġ��Door �� ��, ���� �ð� �Ŀ� ������ũ ����������(����) �� GAME OVER
		if (bIsDoorOpen)
		{
			CurrentState = ELocationState::CUPCAKE;
		}
		
		MoveToTaggedLocation(FName("Room6"));
	}
}

void AChica::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack !"));
	// ���������� anim ���

	// ���� ����
	
}

void AChica::Cupcake()
{
	UE_LOG(LogTemp, Warning, TEXT("CupCake Attack !"));
	// ������ũ ���������� anim ���

	// ���� ����
}

void AChica::MoveToTaggedLocation(FName Tag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	if (FoundActors.Num() > 0)
	{
		AActor* TargetActor = FoundActors[0];
		FVector TargetLocation = TargetActor->GetActorLocation();

		UE_LOG(LogTemp, Warning, TEXT("Moving to Location: %s"), *Tag.ToString());

		GetController()->StopMovement();

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(TargetLocation);
			MoveRequest.SetAcceptanceRadius(5.0f); // ��ǥ ��ġ�� �����ϴ� ���� ����

			FNavPathSharedPtr NavPath;
			AIController->MoveTo(MoveRequest, &NavPath);
		}
	}
}

//void AChica::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//	Super::EndPlay(EndPlayReason);
//
//	GetWorld()->GetTimerManager().ClearTimer(Timer);
//}

// ������ ON �Լ�
void AChica::OnCtrlKeyPressed()
{
	bIsFlashlightOn = !bIsFlashlightOn; // ������ ���� ���
}

