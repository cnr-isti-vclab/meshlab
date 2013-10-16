#include "RuleSet.h"
#include "RuleRef.h"
#include "CustomRule.h"
#include "AmbiguousRule.h"
#include "PrimitiveRule.h"

#include <QStringList>
#include <QMap>
#include <typeinfo>

#include "../../SyntopiaCore/Exceptions/Exception.h"
#include "../../SyntopiaCore/Logging/Logging.h"
#include "../../SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Exceptions;
using namespace SyntopiaCore::Logging;
using namespace SyntopiaCore::Math;

namespace StructureSynth {
	namespace Model {	

		/// Constructor. Automatically adds built-in rules.
		RuleSet::RuleSet() {
			topLevelRule = new CustomRule("TopLevelRule");
			recurseDepth = false;
			defaultClass = new PrimitiveClass();

			/// Add instances of predefined built-in types.
			rules.append(new PrimitiveRule(PrimitiveRule::Box,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Sphere,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Cylinder,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Mesh,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Line,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Dot,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Grid,defaultClass));
			rules.append(new PrimitiveRule(PrimitiveRule::Template,defaultClass));
			rules.append(topLevelRule);

			
		};

		void RuleSet::setRulesMaxDepth(int maxDepth) {
			for (int i = 0; i < rules.size(); i++) {
				int md = rules[i]->getMaxDepth();
				//INFO(QString("Rule: %1, %2 -> %3").arg(rules[i]->getName()).arg(md).arg(maxDepth));
				if (md <= 0) rules[i]->setMaxDepth(maxDepth);
			}
		}


		/// Deletes rules
		RuleSet::~RuleSet() {
			for (int i = 0; i < rules.size(); i++) delete(rules[i]);
			//for (int i = 0; i < primitiveClasses.size(); i++) delete(primitiveClasses[i]);
			
		}

		void RuleSet::addRule(Rule* rule) { 
			// Check if the rule name is already used...
			QString name = rule->getName();
			
			for (int i = 0; i < rules.size(); i++) {

				if (rules[i]->getName() == name) {
					if (typeid(*rules[i]) == typeid(CustomRule)) {
						// A Custom rule already exists with the same name.
						// Now we must remove the existing rule, and create a new ambiguous rule hosting them both.
			
						Rule* r = rules[i];
						rules.removeAll(r);
						CustomRule* cr1 = dynamic_cast<CustomRule*>(r);
					
						AmbiguousRule* ar = new AmbiguousRule(name);
						ar->appendRule(cr1);

						CustomRule* cr2 = dynamic_cast<CustomRule*>(rule);
						if (!cr2) throw Exception("Trying to add non-custom rule to ambiguous rule: '%1'. "+name);
						ar->appendRule(cr2);

						rules.append(ar);

						return;
					} else if (typeid(*rules[i]) == typeid(PrimitiveRule)) {
						// A primitive rule already exists with the same name. This is not acceptable.
						throw Exception(QString("A primitive rule already exists with the name: '%1'. New definitions can not merged.").arg(name));
					} else if (typeid(*rules[i]) == typeid(AmbiguousRule)) {
						// A ambiguous rule already exists with the same name. We will add to it.
						AmbiguousRule* ar = dynamic_cast<AmbiguousRule*>(rules[i]);
						CustomRule* cr = dynamic_cast<CustomRule*>(rule);
						if (!cr) throw Exception("Trying to add non-custom rule to ambiguous rule: '%1'. "+name);
						ar->appendRule(cr);
						return;
					} else {
						WARNING("Unknown typeid");
					}
				}

			}

			rules.append(rule);
		}


		/// Resolve symbolic names into pointers
		QStringList RuleSet::resolveNames() {

			// build map
			QMap<QString, Rule*> map;
			for (int i = 0; i < rules.size(); i++) map[rules[i]->getName()] = rules[i];

			QStringList usedPrimitives;
			


			// resolve rules.
			for (int i = 0; i < rules.size(); i++) {

				QList<RuleRef*> refs = rules[i]->getRuleRefs();


				for (int j = 0; j < refs.size(); j++) {
					QString name = refs[j]->getReference();
					if (!map.contains(name)) {
						// We could not resolve the name.
						// Check if it has a class specifier.
						QStringList sl = name.split("::");
						if (sl.size() == 2) {
							QString baseName = sl[0];
							QString classID = sl[1];

							if (!map.contains(baseName)) {
								throw Exception(QString("Unable to resolve base rule name: %1 for rule %2").arg(baseName).arg(name));
							}
					
							// Now we have to create a new instance of this rule.
							Rule* r = map[baseName];

							if (typeid(*r) != typeid(PrimitiveRule)) {
								throw Exception(QString("Only primitive rules (box, sphere, ...) may have a class specifier: %1 is invalid").arg(name));
							}

							PrimitiveRule* pr = (PrimitiveRule*)r;
							PrimitiveRule* newRule = new PrimitiveRule(*pr);
							newRule->setClass(getPrimitiveClass(classID));
							
							map[name] = newRule;
							
							//INFO("Created new class for rule: " + name);
						} else {
							// The Polygons rules (i.e. Triangle[x,y,z]) are special rules, each created on the fly.
							QRegExp r("triangle\\[(.*)\\]");
							if (r.exactMatch(name)) {
								// Check the arguments.
								INFO("Found:" + r.cap(1));
								QVector<Vector3f> v;
								QStringList l = r.cap(1).split(";");
								if (l.size() != 3) {
									throw Exception(QString("Unable to parse Triangle definition - must be triangle(p1;p2;p3) - found : %1").arg(name));
								}

								for (unsigned int i = 0; i < 3; i++) {
									QStringList l2 = l[i].split(",");
									if (l2.size() != 3) {
											throw Exception(QString("Unable to parse Triangle definition - coordinates must be like '0.1,0.2,0.3' - found : %1").arg(l[i]));
									}
									bool ok = false;
									float f1 = l2[0].toFloat(&ok);
									if (!ok) throw Exception(QString("Unable to parse Triangle definition - error in first coordinate - found in : %1").arg(name));
									float f2 = l2[1].toFloat(&ok);
									if (!ok) throw Exception(QString("Unable to parse Triangle definition - error in second coordinate - found in : %1").arg(name));
									float f3 = l2[2].toFloat(&ok);
									if (!ok) throw Exception(QString("Unable to parse Triangle definition - error in third coordinate - found in : %1").arg(name));
									v.append(Vector3f(f1,f2,f3));
								}	


								map[name] = new TriangleRule(v[0], v[1], v[2], defaultClass);
							
							} else {
								throw Exception(QString("Unable to resolve rule: %1").arg(name));
							}
						}
					}
					if ( dynamic_cast<PrimitiveRule*>(map[name]) ) {
						if (!usedPrimitives.contains(name)) usedPrimitives.append(name);
					}
					refs[j]->setRef(map[name]);
				}

			}

			
			return usedPrimitives;

		}

		///
		QStringList RuleSet::getUnreferencedNames() {

			WARNING("RuleSet::getUnreferencedNames(): Not implemented yet!");
			return QStringList();

		};

		Rule* RuleSet::getStartRule() const {
			return topLevelRule;
		};

		/// For debug
		void  RuleSet::dumpInfo() const {
			int custom = 0;
			int ambi = 0;
			int primitive = 0;
			int rulesCount = 0;

			for (int i = 0; i < rules.size(); i++) {
				rulesCount++; 

				CustomRule* cr = dynamic_cast<CustomRule*>(rules[i]);
				if (cr) custom++;

				AmbiguousRule* ar = dynamic_cast<AmbiguousRule*>(rules[i]);
				if (ar) ambi++;

				PrimitiveRule* pr = dynamic_cast<PrimitiveRule*>(rules[i]);
				if (pr) primitive++;
			}

			/*
			Debug(QString("Loaded %1 user rules: %2 Custom Rules, %3 Ambiguous Rules")
				.arg(rulesCount-primitive).arg(custom).arg(ambi));
			Debug(QString("Loaded %1 built-in rules.").arg(primitive));
			*/
		}

		bool RuleSet::existsPrimitiveClass(QString classLabel) {
			for (int i = 0; i < primitiveClasses.count(); i++) {
				if (primitiveClasses[i]->name == classLabel) return true;
			}
			return false;
		}


		PrimitiveClass* RuleSet::getPrimitiveClass(QString classLabel) {
			for (int i = 0; i < primitiveClasses.count(); i++) {
				if (primitiveClasses[i]->name == classLabel) return primitiveClasses[i];
			}
			PrimitiveClass* p = new PrimitiveClass(*defaultClass);
			p->name = classLabel;
			//INFO("Created new primitiveClass: " + classLabel);
			primitiveClasses.append(p);
			return p;
		}


	}
}

