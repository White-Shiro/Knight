#pragma once

class ObjectFinder {
public:
	template <typename T>
	FORCEINLINE static TObjectPtr<T> LoadAsset(const TCHAR* path) {
		 // Jason: this static only per Type <T>, so cannot help you to find different object with same type
		 // what I did is using macro and create a scope in place
		 // FYI https://github.com/Jasonchan35/Blacktown_ThirdPersonTemplate/blob/main/Source/ThirdPerson/MyLib/MyCDO.h
		static ConstructorHelpers::FObjectFinder<T> Finder = path;
		return Finder.Object;
	}
};
