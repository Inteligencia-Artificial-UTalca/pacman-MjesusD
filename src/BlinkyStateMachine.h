
#ifndef BLINKYSTATEMACHINE_H_
#define BLINKYSTATEMACHINE_H_

#include "FSM.h"
#include "Controller.h"


//////////////////////////////// TRANSITIONS //////////////////////////////////


class TimerTransition : public FSMTransition {

private:

	int counter;
	int limit;

	std::shared_ptr<FSMState> _next;

public:

	TimerTransition(
		std::shared_ptr<FSMState> next,
		int limit
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};

///////////////////////////////////////////////////////////////////////////////

class PowerPelletTransition : public FSMTransition {

private:

	std::shared_ptr<FSMState> _next;

public:

	PowerPelletTransition(
		std::shared_ptr<FSMState> next
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};

///////////////////////////////////////////////////////////////////////////////

class FrightenedEndTransition : public FSMTransition {

private:

	int counter;
	int limit;

	std::shared_ptr<FSMState> _next;

public:

	FrightenedEndTransition(
		std::shared_ptr<FSMState> next,
		int limit = 300
	);

	bool isValid(const GameState& gs) override;

	std::shared_ptr<FSMState>
	getNextState() override;
};


//////////////////////////////// ChaseState ///////////////////////////////////


class ChaseState : public FSMState {

public:

	ChaseState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~ChaseState();
};


/////////////////////////////// ScatterState //////////////////////////////////


class ScatterState : public FSMState {

public:

	ScatterState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~ScatterState();
};


//////////////////////////// FrightenedState /////////////////////////////////


class FrightenedState : public FSMState {

public:

	FrightenedState(std::shared_ptr<Character> _character);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	~FrightenedState();
};


/////////////////////////// NonFrightenedState ////////////////////////////////


class NonFrightenedState : public FSMState {

private:

	std::shared_ptr<FSMState> activeState;

	std::shared_ptr<FSMState> chaseState;

	std::shared_ptr<FSMState> scatterState;

public:

	NonFrightenedState(
		std::shared_ptr<Character> _character
	);

	Move onUpdate(const GameState& gs) override;

	void onEnter(const GameState& gs) override;

	void onExit(const GameState& gs) override;

	~NonFrightenedState();
};


/////////////////////////// BlinkyStateMachine ////////////////////////////////


class BlinkyStateMachine : public FiniteStateMachine {

public:

	BlinkyStateMachine(
		std::shared_ptr<Character> _character
	);

	Move update(const GameState& gs) override;

	~BlinkyStateMachine();
};

#endif /* BLINKYSTATEMACHINE_H_ */