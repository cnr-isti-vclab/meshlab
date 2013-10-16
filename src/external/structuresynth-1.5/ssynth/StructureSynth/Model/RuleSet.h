#pragma once

#include "Rule.h"
#include "PrimitiveClass.h"
#include "CustomRule.h"

#include "../../SyntopiaCore/GLEngine/Object3D.h"

namespace StructureSynth {
	namespace Model {	

		using namespace SyntopiaCore::GLEngine;

		/// Container for all rules.
		class RuleSet {
		public:
			/// Constructor. Automatically adds built-in rules.
			RuleSet();

			/// Deletes rules
			~RuleSet();

			/// Added rules belong to the RuleSet and will be deleted by the RuleSet destructor.
			void addRule(Rule* rule);

			/// Resolve symbolic names into pointers
			/// Returns a list of the primitives used
			QStringList resolveNames();

			/// TODO: Implement
			QStringList getUnreferencedNames();

			Rule* getStartRule() const ;

			CustomRule* getTopLevelRule() const { return topLevelRule; }

			/// For debug
			void dumpInfo() const;

			void setRecurseDepthFirst(bool value) { recurseDepth = value; };
			bool recurseDepthFirst() { return recurseDepth; }
			void setRulesMaxDepth(int maxDepth);

			/// Returns the PrimitiveClass with this name. Constructs a new one if missing.
			PrimitiveClass* getPrimitiveClass(QString classLabel);
			bool existsPrimitiveClass(QString classLabel);
			PrimitiveClass* getDefaultClass() { return defaultClass; }

		private:
			QList<Rule*> rules;
			QVector<PrimitiveClass*> primitiveClasses;
			PrimitiveClass* defaultClass;
			CustomRule* topLevelRule;
			bool recurseDepth;
		};

	}
}

