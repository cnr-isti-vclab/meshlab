#include "AmbiguousRule.h"

#include "Builder.h"
#include "RandomStreams.h"

#include "../../SyntopiaCore/Logging/Logging.h"

using namespace SyntopiaCore::Logging;


namespace StructureSynth {
	namespace Model {	

		QList<RuleRef*> AmbiguousRule::getRuleRefs() const {
			QList<RuleRef*>  list;
			for (int i = 0; i < rules.size(); i++) {
				for (int j = 0; j < rules[i]->getRuleRefs().size(); j++) {
					list.append(rules[i]->getRuleRefs()[j]);
				}
			}
			return list;
		}

		void AmbiguousRule::apply(Builder* builder) const {
			// Calc sum of weigths
			double totalWeight = 0;
			for (int i = 0; i < rules.size(); i++) {
				totalWeight += rules[i]->getWeight();
			}


			double random = totalWeight*RandomStreams::Geometry()->getDouble();


			// Choose a random rule according to weights
			double accWeight = 0;
			for (int i = 0; i < rules.size(); i++) {
				accWeight += rules[i]->getWeight();
				if (random <= accWeight) {
					rules[i]->apply(builder);
					return;
				}
			}
			rules[rules.size()-1]->apply(builder);

			WARNING("Assertion failed: in AmbiguousRule::apply");

		};
	
	}
}

