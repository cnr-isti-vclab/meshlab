#include "EisenParser.h"

#include "../../SyntopiaCore/Logging/Logging.h"
#include "../Model/CustomRule.h"

#include <QColor>

using namespace SyntopiaCore::Logging;
using namespace StructureSynth::Model;

namespace StructureSynth {
	namespace Parser {	
		
		/*
			EISENSCRIPT
			-----------

			EBNF:
			-----

			program = { set | rule } ;
			rule = 'RULE' ,  rule_name, { rule_modifier } , '{', { set | action } , '}' ;
			rule_modifier = { 'MAXDEPTH' integer | WEIGHT float }
			action = transformation rule_ref ;
			set = 'SET'  , var_name , string ;

			'rule_ref', 'var_name', and 'string' are text strings with a reasonable set of allowed characters.

			Example:
			--------

			//
			//	C++ style multiline comments are allowed.
			//  A pre-processor strips comments, and imports '#include's
			//
			#include "../basicstuff.es"
			set background #F00  // All 'SET' commands outside the scope of a rule are executed at startup.

			RULE core {
				r1 
			}

			RULE r1 5 {
			}

			RULE r2 1 {
			}
		*/


		EisenParser::EisenParser(Tokenizer* tokenizer) : tokenizer(tokenizer) {
			recurseDepth = false;
		};

		EisenParser::~EisenParser() {
		};


		void EisenParser::getSymbol() {
			symbol = tokenizer->getSymbol();
		};

		bool EisenParser::accept(Symbol::SymbolType st) {
			if (symbol.type == st) {
				getSymbol();
				return true;
			}
			return false;
		}

		void EisenParser::ruleModifierList(CustomRule* customRule) {
			while (symbol.type == Symbol::Operator) { 
				if (symbol.text == "weight") {
					getSymbol();
					double param = symbol.getNumerical();
					if (!accept(Symbol::Number)) {
						throw (ParseError("Rule modifier 'weight' expected numerical argument. Found: " + symbol.text, symbol.pos));
					}
					customRule->setWeight(param);

				} else if (symbol.text == "maxdepth") {
					getSymbol();
					int param = (int)symbol.getNumerical();
					if (!symbol.isInteger || !accept(Symbol::Number)) {
						throw (ParseError("Rule modifier 'maxdepth' expected integer argument. Found: " + symbol.text, symbol.pos));
					}
					customRule->setMaxDepth(param);

					if (symbol.type == Symbol::MoreThan) {
						getSymbol();
						QString ruleName = symbol.text;
						if (!accept(Symbol::UserString)) throw (ParseError("After maxdepth retirement operator a rule name is expected. Found: " + symbol.text, symbol.pos));
						customRule->setRetirementRule(ruleName);
					}
				} else {
					throw (ParseError("In rule modifier list: expected maxdepth or weight. Found: " + symbol.text, symbol.pos));
				}
			}

			if (!symbol.type == Symbol::LeftBracket) {
				throw (ParseError("After rule modifier list: expected a left bracket. Found: " + symbol.text, symbol.pos));
			}
		}

		Rule* EisenParser::rule() {
			// rule = 'RULE' ,  rule_name, '{', { set | action }  , '}' ;
		
			if (!accept(Symbol::Rule)) throw (ParseError("Unexpected: trying to parse Rule not starting with rule identifier. Found: " + symbol.text, symbol.pos));
			
			QString ruleName = symbol.text;
			if (!accept(Symbol::UserString)) throw (ParseError("After rule identifier a rule name is expected. Found: " + symbol.text, symbol.pos));
			CustomRule* customRule = new CustomRule(ruleName);

			if (symbol.type != Symbol::LeftBracket) {
				// This must be a rule_modifier list.
				ruleModifierList(customRule);
			}


			if (!accept(Symbol::LeftBracket)) throw (ParseError("After rule name a left bracket is expected. Found: " + symbol.text, symbol.pos));
				
			// TODO: implement rest of types:
			// Possible actions:
			//   SET something = something
			//   rulename
			//   { yaw 20 size 0.1 } rulename
			//   20 * { forward 10 } rulename
			while (symbol.type == Symbol::LeftBracket || symbol.type == Symbol::UserString ||
				symbol.type == Symbol::Number || symbol.type == Symbol::Set) {    
				
				if (symbol.type == Symbol::Set) {
					Action a = setAction(); 
					customRule->appendAction(a);
				} else {
					Action a = action(); 
					customRule->appendAction(a);
				}
			}

			if (!accept(Symbol::RightBracket)) throw (ParseError("A rule definition must end with a right bracket. Found: "+symbol.text, symbol.pos));

			return customRule;
		}

		double degreeToRad(double degrees) {
			return degrees*3.14159265/180.0;
		}

		Transformation EisenParser::transformation() {

			QString type = symbol.text;
			if (!accept(Symbol::Operator)) throw (ParseError("Transformation: Expected transformation identifier (e.g. 'x' or 'rx'). Found: " + symbol.text, symbol.pos));

			if (type == "x") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'X' (X-axis translation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createX(param);
			} else if (type == "y") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'Y' (Y-axis translation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createY(param);
			} else if (type == "z") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'Z' (Z-axis translation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createZ(param);
			} else if (type == "rx") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'RX' (X-axis rotation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createRX(degreeToRad(param));
			} else if (type == "ry") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'RY' (Y-axis rotation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createRY(degreeToRad(param));
			} else if (type == "rz") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'RZ' (Z-axis rotation): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createRZ(degreeToRad(param));
			} else if (type == "hue") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'hue': Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createHSV(param, 1,1,1);
			} else if (type == "sat") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'sat': Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createHSV(0, param,1,1);
			} else if (type == "brightness") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'brightness': Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createHSV(0, 1,param,1);
			} else if (type == "color") {
				QString param = symbol.text;
				if (!QColor(param).isValid() && param.toLower()!="random") throw (ParseError("Transformation 'color': Expected a valid color. Found: " + symbol.text, symbol.pos));
				getSymbol();
				return Transformation::createColor(param);
			} else if (type == "blend") {
				QString param = symbol.text;
				if (!QColor(param).isValid()) throw (ParseError("Transformation 'blend': Expected a valid color as first argument. Found: " + symbol.text, symbol.pos));
				getSymbol();
				double param2 = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'blend': Expected a numerical value as second argument. Found: " + symbol.text, symbol.pos));
				return Transformation::createBlend(param, param2);
			} else if (type == "alpha") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'alpha': Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createHSV(0, 1,1,param);
			} else if (type == "matrix") {
				QVector<double> ds;
				for (unsigned int i = 0; i < 9; i++) {
					double param = symbol.getNumerical();
					if (!accept(Symbol::Number)) throw (ParseError("Transformation 'matrix': Expected nine (9) parameters. Found: " + symbol.text, symbol.pos));
					ds.append(param);
				}
				return Transformation::createMatrix(ds);
			} else if (type == "s") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'S' (size): Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				
				if (symbol.type == Symbol::Number) {
					double param2 = symbol.getNumerical();
					getSymbol();
					double param3 = symbol.getNumerical();
					if (!accept(Symbol::Number)) throw (ParseError("Transformation 'S' (size): Expected third numerical parameter. Found: " + symbol.text, symbol.pos));
					return Transformation::createScale(param,param2,param3);
				}
				return Transformation::createScale(param,param,param);
			} else if (type == "reflect") {
				double param = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'reflect': Expected numerical parameter. Found: " + symbol.text, symbol.pos));
				
				double param2 = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'reflect': Expected second numerical parameter. Found: " + symbol.text, symbol.pos));
				double param3 = symbol.getNumerical();
				if (!accept(Symbol::Number)) throw (ParseError("Transformation 'reflect': Expected third numerical parameter. Found: " + symbol.text, symbol.pos));
				return Transformation::createPlaneReflection(SyntopiaCore::Math::Vector3f(param,param2,param3));
			
			} else if (type == "fx") {
				return Transformation::createScale(-1,1,1);
			} else if (type == "fy") {
				return Transformation::createScale(1,-1,1);
			} else if (type == "fz") {
				return Transformation::createScale(1,1,-1);
			} else {
				throw (ParseError("Unknown transformation type: " + type, symbol.pos));
			}
		}

		Transformation EisenParser::transformationList() {
			// A transformationlist is something like: 
			// { x 23 rx 23 }

			Transformation t;

			if (!accept(Symbol::LeftBracket)) throw (ParseError("Transformation List: Expected a left bracket. Found: " + symbol.text, symbol.pos));

			while (symbol.type == Symbol::Operator) {    
				t.append(transformation());
			}

			if (!accept(Symbol::RightBracket)) throw (ParseError("Transformation List: Expected a right bracket or an operator. Found: " + symbol.text, symbol.pos));

			return t;
		}

		Action EisenParser::action() {
			// There are 3 types of action statements:
			//  { rx 20 ry 30 rz 20 } rulename
			//  rulename
			//  20 * { x 10 } 10 * { y 10 } rulename
			
			if (symbol.type == Symbol::LeftBracket) {
				Transformation t = transformationList();
				QString ruleName = symbol.text.trimmed();
				if (!accept(Symbol::UserString)) throw (ParseError("Expected a rule name after the transformation list. Found: " + symbol.text, symbol.pos));
				return Action(t, ruleName);
			} else if (symbol.type == Symbol::UserString) {
				QString ruleName = symbol.text.trimmed();
				accept(Symbol::UserString);
				return Action(ruleName);
			} else if (symbol.type == Symbol::Number) {

				Action action;

				while (symbol.type == Symbol::Number) {
					// number of loops...
					if (!symbol.isInteger) throw (ParseError("Expected an integer count in the transformation loop. Found: " + symbol.text, symbol.pos));
					int count = symbol.intValue;
					getSymbol(); 

					// '*'
					if (!accept(Symbol::Multiply)) throw (ParseError("Expected a '*' after the transformation count. Found: " + symbol.text, symbol.pos));
					
					// transformation list
					Transformation t = transformationList();
					action.addTransformationLoop(TransformationLoop(count, t));
				}
				
				// Rule reference
				QString ruleName = symbol.text.trimmed();
				if (!accept(Symbol::UserString)) throw (ParseError("Expected a rule name or a new loop after the transformation list. Found: " + symbol.text, symbol.pos));
				action.setRule(ruleName);

				return action;

			} else {
				throw (ParseError("A rule action must start with either a number, a rule name or a left bracket. Found: "+symbol.text, symbol.pos));	
			}
		}

		Action EisenParser::setAction() {
				accept(Symbol::Set);
				
				QString key = symbol.text;
				if (symbol.type == Symbol::Operator && key == "maxdepth") {
					getSymbol();
				} else if (!accept(Symbol::UserString)) throw (ParseError("Expected a valid setting name. Found: " + symbol.text, symbol.pos));
				QString value = symbol.text; 
				getSymbol(); // We will accept everything here! 

				if (key == "recursion" && value == "depth") recurseDepth = true;
				
				return Action(key,value);
		}

		RuleSet* EisenParser::ruleset() {
			RuleSet*  rs = new RuleSet();
			getSymbol();
	
			while (symbol.type == Symbol::Rule || symbol.type == Symbol::Set
				    || symbol.type == Symbol::LeftBracket || symbol.type == Symbol::UserString || symbol.type == Symbol::Number) {    
				if (symbol.type == Symbol::Rule) {
					Rule* r = rule(); 
					rs->addRule(r);
				} else if (symbol.type == Symbol::Set) {
					Action a = setAction(); 
					rs->getTopLevelRule()->appendAction(a);
				} else {
					Action a = action(); 
					rs->getTopLevelRule()->appendAction(a);
				}
			}

			if (!accept(Symbol::End)) throw (ParseError("Unexpected symbol found. At this scope only RULE and SET statements are allowed. Found: " + symbol.text, symbol.pos));
			if (recurseDepth) rs->setRecurseDepthFirst(true);
			return rs;
		}


		RuleSet* EisenParser::parseRuleset() {
			return ruleset();
		}
	}
}

