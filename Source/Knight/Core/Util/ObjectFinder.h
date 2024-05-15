#pragma once

class ObjectFinder {
public:
	template <typename T>
	FORCEINLINE static TObjectPtr<T> LoadAsset(const TCHAR* path) {
		static ConstructorHelpers::FObjectFinder<T> Finder = path;
		return Finder.Object;
	}
};
