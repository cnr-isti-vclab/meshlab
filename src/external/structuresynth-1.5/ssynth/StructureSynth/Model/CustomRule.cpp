#include "CustomRule.h"

#include "../../SyntopiaCore/Logging/Logging.h"
#include "Builder.h"

using namespace SyntopiaCore::Logging;

namespace StructureSynth {
	namespace Model {	

		CustomRule::CustomRule(QString name) : Rule(name) {
			weight = 1.0;
			retirementRule = 0;
		}

		CustomRule::~CustomRule() {
			//delete (retirementRule);
		}

		void CustomRule::apply(Builder* b) const {

			int newDepth = -1;
			/// If there is a maxdepth set for this object check it.
			if (getMaxDepth() != -1) {
				if (!b->getState().maxDepths.contains(this)) {
					/// We will add a new maxdepth for this rule to the state.
					newDepth = getMaxDepth()-1;
					
				} else {
					int depth = b->getState().maxDepths[this];
					if (depth <= 0) {
						/// This rule is retired.
						if (retirementRule) {
							b->getState().maxDepths[this] = maxDepth;
							retirementRule->rule()->apply(b);
							
						} 
						return;
					} else {
						/// Decrease depth.
						newDepth = depth-1;
					}
				}
			}

			/// Apply all actions.
			for (int i = 0; i < actions.size(); i++) {
				if (getMaxDepth() != -1) {
					actions[i].apply(b, this, newDepth);
				} else {
					actions[i].apply(b, 0 ,0);
				}
			}
		}

		QList<RuleRef*> CustomRule::getRuleRefs() const {
			QList<RuleRef*>  list;
			for (int i = 0; i < actions.size(); i++) {
				RuleRef* a = actions[i].getRuleRef();
				if (a) list.append(a);
			}
			if (retirementRule) list.append(retirementRule);

			return list;
		}
	

	}
}

