#include "Action.h"
#include "ExecutionStack.h"
#include "Builder.h"

#include "../../SyntopiaCore/Logging/Logging.h"

using namespace SyntopiaCore::Logging;

namespace StructureSynth {
	namespace Model {	
		void Action::apply(Builder* b, const Rule* callingRule, int ruleDepth) const {
			bool rememberPreviousMatrix = true; // at some point we might make this optional -> only needed for grid meshes...

			if (set != 0) {
				b->setCommand(set->key, set->value);
				return;
			}

			State s = b->getState();
			
			
			QList<int> counters;
			for (int i = 0; i < loops.size(); i++) counters.append(1);

			if (counters.size() == 0) {
				if (callingRule) {
					s.maxDepths[callingRule] = ruleDepth;
				}
				b->getNextStack().append(RuleState(rule->rule(), s));
				return;
			}

			bool done = false;
			while (!done) {

				// create state
				State s0 = s;
				if (rememberPreviousMatrix) {
					// Copy the old matrix...
					s0.setPreviousState(s.matrix, s.hsv, s.alpha);
				}
				for (int i = 0; i < counters.size(); i++) {
					for (int j = 0; j < counters[i]; j++) {
						s0 = loops[i].transformation.apply(s0, b->getColorPool());
					}
				}
				if (callingRule) {
					s0.maxDepths[callingRule] = ruleDepth;
				}
				b->getNextStack().append(RuleState(rule->rule(), s0));

				// increase lowest counter...
				counters[0]++;
				for (int i = 0; i < counters.size(); i++) {
					if (counters[i] > loops[i].repetitions) {
						if (i == counters.size()-1) {
							done = true;
						} else {
							counters[i] = 1;
							counters[i+1]++;
						}
					}
				}

			}
		}

		Action::Action(QString key, QString value) {
			set = new SetAction();
			set->key = key;
			set->value = value;
			rule = 0;
		}

		Action::~Action() {
			// TODO: Handle leaks (Actions are treated as value types, and hence rule,set ptrs are duped)
			//delete(rule);
			//delete(set);
		}


		void Action::addTransformationLoop(TransformationLoop tl) { 
			loops.append(tl); 
		}

		void Action::setRule(QString ruleName) {
			rule = new RuleRef(ruleName);
			set = 0;
		}

		Action::Action(Transformation t, QString ruleName) {
			loops.append(TransformationLoop(1, t));
			rule = new RuleRef(ruleName);
			set = 0;
		}

		Action::Action(QString ruleName) {
			rule = new RuleRef(ruleName);
			set = 0;
		}

	}
}

