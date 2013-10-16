#pragma once

#include "Rule.h"
#include "State.h"

namespace StructureSynth {
	namespace Model {	

		struct RuleState {
			RuleState() {};
			RuleState(Rule* rule, State state) : rule(rule), state(state) {};

			Rule* rule;
			State state;
		};

		/// The ExecutionStack keeps track of which operations to perform next.
		/// Rules are executed in generations:
		///  The rules on the stack are all executed in each generation,
		///  and each rule will add a number of new rules to the next generation of the stack.
		///  Only one level is recursion is followed at each generation.
		typedef QVector<RuleState> ExecutionStack;

		/*
		struct ExecutionStack {
			QList< RuleState > currentStack;
		};
		*/

		
	}
}

