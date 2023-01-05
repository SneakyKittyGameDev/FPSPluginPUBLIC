// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTTP/Public/Http.h"
#include "JsonObjectConverter.h"

#include "HttpHelper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAPIRespEvent, const FString&, JSONString);
const static FString API_URL = FString("https://YourURLHere.com/api/");

UCLASS()
class ULTIMATEFPSFRAMEWORK_API UHttpHelper : public UObject
{
	GENERATED_BODY()
protected:
	static FHttpModule* HTTP;
	static FAPIRespEvent CachedRespEvent;
	
	static void RESP_HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
public:
	static void API_PostRequest(FAPIRespEvent RespEvent, const FString& Handle, const FString& PostData);
	static void API_GetRequest(FAPIRespEvent RespEvent, const FString& Handle, const FString& GetData);
	
	template<typename OutStructType>
	static OutStructType DeSerializeStruct(const FString& JsonString)
	{
		OutStructType ReadyStruct;
		FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &ReadyStruct, 0, 0);
		return ReadyStruct;
	}
	template<typename OutStructType>
	static bool DeSerializeStruct(const FString& JsonString, OutStructType* OutStruct)
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, OutStruct, 0, 0);
	}
	
	template<typename OutStructType>
	static FString SerializeStruct(OutStructType* InStruct)
	{
		FString ReturnString = FString("");
		FJsonObjectConverter::UStructToJsonObjectString(*InStruct, ReturnString, 0, 0);
		return ReturnString;
	}
	template<typename InStructType>
	static void SerializeStruct(InStructType* InStruct, FString& OutString)
	{
		FJsonObjectConverter::UStructToJsonObjectString(*InStruct, OutString, 0, 0);
	}
	
	static FHttpModule* GetHttpModule();
};
