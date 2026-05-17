
#ifndef SUESTATEMACHINE_H_
#define SUESTATEMACHINE_H_

#include "FSM.h"
#include "Controller.h"


//////////////////////////////// TRANSITIONS //////////////////////////////////


class SueTimerTransition : public FSMTransition {

private:

	int counter;
	int limit;

	std::shared_ptr<FSMState> _next;

public:

	SueTimerTransition(
		std::shared_ptr<FSMState> next,
		int limit
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};

///////////////////////////////////////////////////////////////////////////////

class SuePowerPelletTransition : public FSMTransition {

private:

	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> character;

public:

	SuePowerPelletTransition(
		std::shared_ptr<Character> character,

		std::shared_ptr<FSMState> next
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};

///////////////////////////////////////////////////////////////////////////////

class SueFrightenedEndTransition : public FSMTransition {

private:

	int counter;
	int limit;

	std::shared_ptr<FSMState> _next;

public:

	SueFrightenedEndTransition(
		std::shared_ptr<FSMState> next,
		int limit = 300
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};


//////////////////////////////// SueChaseState ///////////////////////////////////


class SueChaseState : public FSMState {

public:

	SueChaseState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~SueChaseState();
};


/////////////////////////////// SueScatterState //////////////////////////////////


class SueScatterState : public FSMState {

public:

	SueScatterState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~SueScatterState();
};


//////////////////////////// SueFrightenedState /////////////////////////////////


class SueFrightenedState : public FSMState {

public:

	SueFrightenedState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~SueFrightenedState();
};


/////////////////////////// SueNonFrightenedState ////////////////////////////////


class SueNonFrightenedState : public FSMState {

private:

	std::shared_ptr<FSMState> activeState;

	std::shared_ptr<FSMState> chaseState;

	std::shared_ptr<FSMState> scatterState;

public:

	SueNonFrightenedState(
		std::shared_ptr<Character> _character
	);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	void onExit(const GameState& gs) override;

	~SueNonFrightenedState();
};


/////////////////////////// SueStateMachine ////////////////////////////////


class SueStateMachine : public FiniteStateMachine {

public:

	SueStateMachine(
		std::shared_ptr<Character> _character
	);

	Move update(const GameState& gs) override;

	~SueStateMachine();
};

#endif /* SueStateMachine_H_ */