#pragma once

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
