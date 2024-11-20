#pragma once
#include "Knight/Core/Fmt/MyFmt.h"

#define KF_LOG_MESSAGE_FMT(message, ...)\
	do { \
		FString __formatedString = MY_FMT(message, ##__VA_ARGS__); \
		KfLog::Log(__formatedString); \
		KfLog::Message(__formatedString); \
	} while (false)\

// Old
#define UC_LOG(Format, ...)			UE_LOG(LogTemp, Display, TEXT(Format), ##__VA_ARGS__)

#define UC_MSG(Format, ...)			{\
if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__));\
}

#define UC_LOG_WARNING(Format, ...) UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__)

#define UC_LOG_ERROR(Format, ...)	UE_LOG(LogTemp, Error,	TEXT(Format), ##__VA_ARGS__)

#define UC_LOG_MSG(Format, ...)		{\
	UC_LOG(Format, ##__VA_ARGS__);\
	UC_MSG(Format, ##__VA_ARGS__);\
}

#define UC_LOG_WARNING_MSG(Format, ...)	{ \
	UC_LOG_WARNING	(Format, ##__VA_ARGS__) \
	UC_MSG			(Format, ##__VA_ARGS__) \
}

#define UC_LOG_ERROR_MSG(Format, ...)	{ \
	UC_LOG_ERROR	(Format, ##__VA_ARGS__) \
	UC_MSG			(Format, ##__VA_ARGS__) \
}

#define UC_LOG_MSG_CONDITIONAL(Condition, Format, ...)	{ \
if ((bool)(Condition)) UC_LOG_MSG(Format, ##__VA_ARGS__) \
}

class KfLog {
public:

	FORCEINLINE static void Log(const FString& message) {
		UE_LOG(LogTemp, Display, TEXT("%s"), *message);
	}

	FORCEINLINE static void Message(const FString& message) {
		if (!GEngine) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, message);
	}
};