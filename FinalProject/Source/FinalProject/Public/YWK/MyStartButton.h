// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "MyStartButton.generated.h"

UCLASS()
class FINALPROJECT_API AMyStartButton : public APSH_BlockActor
{
	GENERATED_BODY()
	
public:	
	AMyStartButton();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* ButtonMesh;  // ������ ������ ��ư �κ�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* BaseMesh;    // ��ư ��ħ�� �κ�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UBoxComponent* CollisionBox;       // �浹 ����

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	TSubclassOf<class URotationWidget> RotationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	TSubclassOf<class UMovewidget> MovewidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widgets")
	TSubclassOf<class UFlyWidget> FlyWidgetClass;

	// AMyStartButton.h
	UPROPERTY()
	URotationWidget* RotationWidgetInstance;

	UPROPERTY()
	UMovewidget* MovewidgetInstance;

	// Overlap Event Functions
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
                        const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                      class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button Animation")
	FVector OriginalScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button Animation")
	FVector PressedScale;

	bool bmodeChange = true;

	bool bIsOverlapping = false; // Ŭ���� ��� ����

};
