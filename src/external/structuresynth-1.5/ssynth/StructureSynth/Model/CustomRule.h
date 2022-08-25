#pragma once

#include "Rule.h"
#include "Action.h"

namespace StructureSynth {
	namespace Model {	

		/// A custom rule is a user defined rule.
		/// It consist of a number of actions, 
		/// and a weight that is used if the rule definition is ambiguous (see 'AmbiguousRule').
		class CustomRule : public Rule {
		public:
			CustomRule(QString name);
			virtual ~CustomRule();

			virtual void apply(Builder* builder) const;

			/// Returns a list over rules that this rule references.
			virtual QList<RuleRef*> getRuleRefs() const;

			void appendAction(Action a) { actions.append(a); }

			double getWeight() const { return weight; }
			void setWeight(double w) { weight = w; }

			void setRetirementRule(QString ruleName) { retirementRule = new RuleRef(ruleName); };

		private:
			QList<Action> actions;
			double weight;
			RuleRef* retirementRule;
		};

	}
}

