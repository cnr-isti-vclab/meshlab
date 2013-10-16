#pragma once

#include "Rendering/Renderer.h"
#include "State.h"

namespace StructureSynth {
	namespace Model {	

		
		class RuleRef; // forward decl.
		class Builder; // forward decl.

		/// (Abstract) Base class for rules.
		class Rule {
		public:
			/// Every rule must have a name.
			Rule(QString name) : name(name) { maxDepth = -1; };
			Rule() { maxDepth = -1; };

			virtual ~Rule() {};

			QString getName() const { return name; }

			/// When applied the rule will add new pending rules to the ExecutionStack for execution.
			/// Only PrimitiveRules will make use of the renderer.
			virtual void apply(Builder* builder) const = 0;

			/// Returns a list over rules that this rule references.
			virtual QList<RuleRef*> getRuleRefs() const { return QList<RuleRef*>(); }

			virtual void setMaxDepth(int maxDepth) { this->maxDepth = maxDepth; }
			virtual int getMaxDepth() const { return maxDepth; }
			
		protected:
			QString name;
			int maxDepth;
		};
	}
}

