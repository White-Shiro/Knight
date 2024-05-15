#pragma once

#include "CoreMinimal.h"
#include "Knight/Core/Common.h"

//#define LOG_STATE

template<typename T_PARAM>
class SimpleState;

template<typename T_PARAM>
class SimpleStateMachine {
	using StateType = SimpleState<T_PARAM>;
	TArray< TSharedPtr<StateType> > _states;
	int _currentStateIdx = -1;

	StateType* GetCurrentState() {
		return _states.IsValidIndex(_currentStateIdx) ? _states[_currentStateIdx].Get() : nullptr;
	}

public:

	FORCEINLINE int GetSize() const { return _states.Num(); }

	FORCEINLINE void Tick(float DeltaTime, const T_PARAM& param) {
		if(auto* current = GetCurrentState()) {
			current->OnStateTick(*this, param, DeltaTime);
		}
	}

	FORCEINLINE void ChangeState(int newStateIdx, const T_PARAM& param, bool canRepeat = false) {
		if(_currentStateIdx == newStateIdx && !canRepeat) return;

		if(auto* current = GetCurrentState()) {
			current->OnStateExit(*this, param);
		}

		_currentStateIdx = newStateIdx;
		auto* newState = GetCurrentState();

		if(!newState) {
			UC_LOG_MSG("Cannot Change to state : %d, Make sure State is Added To State Machine Explicitly", newStateIdx);
			return;
		}

		newState->OnStateEnter(*this, param);
#ifdef LOG_STATE
		UC_LOG_MSG("Changed to state: %d", _currentStateIdx);
#endif
	}

	FORCEINLINE void InitStatesSize(int size) {
		TSharedPtr<StateType> emptyPtr;
		_states.Init(emptyPtr, size);
	}

	template<typename T_STATE>
	FORCEINLINE void AddState(int stateIdx) {

		// Check if T_STATE is SUBCLASS OF StateType
		static_assert(TIsDerivedFrom<T_STATE, StateType>::IsDerived, "T_STATE must be subclass of StateType");

		if(!_states.IsValidIndex(stateIdx)) {
			UC_LOG_MSG("Add state: %s failed, Index Out Of Range : %d" , *T_STATE::StateNameString(), stateIdx);
			return;
		}

		if(_states[stateIdx].IsValid()) {
			UC_LOG_MSG("Add state: %s failed, %d index Already Exist" , *T_STATE::StateNameString(), stateIdx);
			return;
		}

#ifdef LOG_STATE
		UC_LOG_MSG("Adding state: %s", *T_STATE::StateNameString());
#endif

		TSharedPtr<T_STATE> statePtr = MakeShareable(new T_STATE());
		_states[stateIdx] = statePtr;
	}
};

template<typename T_PARAM>
class SimpleState {
private:
	using StateMachineType = SimpleStateMachine<T_PARAM>;

public:
	SimpleState() = default;
	virtual ~SimpleState() = default;
	virtual int GetStateIdx() const = 0;
	virtual void OnStateEnter(StateMachineType& fsm, const T_PARAM& param) = 0;
	virtual void OnStateTick(StateMachineType& fsm, const T_PARAM& param , float DeltaTime) = 0;
	virtual void OnStateExit(StateMachineType& fsm, const T_PARAM& param) = 0;
};