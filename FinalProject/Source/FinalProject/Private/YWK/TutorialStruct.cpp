// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/TutorialStruct.h"
#include "Engine/DataTable.h"

// void LoadTutorialData()
// {
//     // 데이터 테이블 로드
//     UDataTable* TutorialDataTable = LoadObject<UDataTable>(
//         nullptr,
//         TEXT("/Game/YWK/UI/UIforTutorial/Tutorial_Text/Tutorial_Conversation.Tutorial_Conversation")
//     );
// 
//     if (TutorialDataTable)
//     {
//         // 데이터 테이블에서 ID가 "1"인 행 찾기
//         static const FString ContextString(TEXT("Tutorial Context"));
//         FTutorialStruct* Row = TutorialDataTable->FindRow<FTutorialStruct>(
//             FName(TEXT("1")),
//             ContextString
//         );
// 
//         if (Row)
//         {
//             UE_LOG(LogTemp, Warning, TEXT("ID: %d, Speaker: %s, Text: %s, NextID: %d"),
//                 Row->ID,
//                 *Row->Speaker,
//                 *Row->Text,
//                 Row->NextID
//             );
//         }
//         else
//         {
//             UE_LOG(LogTemp, Error, TEXT("Row not found for ID: 1"));
//         }
//     }
//     else
//     {
//         UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable"));
//     }
// }
