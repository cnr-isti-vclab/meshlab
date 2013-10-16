#pragma once

#include "Tokenizer.h"
#include "../Model/Transformation.h"
#include "../Model/Rule.h"
#include "../Model/CustomRule.h"
#include "../Model/RuleSet.h"
#include "../Model/Action.h"

namespace StructureSynth {
	namespace Parser {	


		/// The' Eisenstein Engine' is a simple recursive descent parser,
		/// for parsing 'EisenScript'.
		class EisenParser {

		public:
			/// Constructor. 
			EisenParser(Tokenizer* tokenizer);

			/// Destructor, The tokenizer is not deleted.
			~EisenParser();

			/// Parses the input, and returns the corresponding ruleset.
			/// Throws a ParseError if any errors are encountered
			Model::RuleSet* parseRuleset();
			bool recurseDepthFirst() { return recurseDepth; }

		private:
			bool recurseDepth;
			void getSymbol();
			Model::Rule* rule();
			Model::RuleSet* ruleset();
			Model::Action action();
			Model::Action setAction();
			Model::Transformation transformationList();
			Model::Transformation transformation();
			void ruleModifierList(Model::CustomRule* customRule);
		
			bool accept(Symbol::SymbolType st);
			bool expect(Symbol::SymbolType st);
			Symbol symbol;

			Tokenizer* tokenizer;
		};

	}
}

