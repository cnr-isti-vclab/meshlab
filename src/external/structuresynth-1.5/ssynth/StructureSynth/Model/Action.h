#pragma once

#include "RuleRef.h"
#include "TransformationLoop.h"

namespace StructureSynth {
	namespace Model {	

		struct SetAction {
			QString key;
			QString value;
		};

		/// An actions is a number of loops, that is applied to a rule.
		///
		/// Rules with only one transformation, e.g.:
		///    { x 1 } R1
		/// are represented as a loop transformation with one repetitions.
		class Action {
		public:
			Action(Transformation t, QString ruleName);
			Action(QString ruleName);
			Action(QString key, QString value);
			Action() { rule = 0; set = 0; }
			
			void addTransformationLoop(TransformationLoop tl);
			void setRule(QString rule);

			~Action();

			/// If 'callingRule' != 0 the new states generated will be set with 
			/// a depth equal to 'ruleDepth'
			void apply(Builder* b, const Rule* callingRule, int ruleDepth) const;
			RuleRef* getRuleRef() const { return rule; }

		private:
			QList<TransformationLoop> loops;
			RuleRef* rule; // The rule that will be called after all transformations.
			SetAction* set;
		};

	}
}

