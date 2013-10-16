#pragma once

#include <QString>
#include <QList>

#include "../../SyntopiaCore/Exceptions/Exception.h"

namespace StructureSynth {
	namespace Parser {	

		class ParseError : public SyntopiaCore::Exceptions::Exception {
		public:
			ParseError(QString message, int position) :  Exception(message), position(position) {};
			int getPosition() { return position; }
		private:
			int position;
		};

		struct Symbol {
			enum SymbolType { Undefined, LeftBracket, RightBracket, MoreThan, End, Number, Multiply, UserString, Rule, Set, Operator } ;

			

			Symbol() :  floatValue(0), intValue(0),  isInteger(false),pos(-1), type(Undefined) { 	};
			Symbol(int pos, SymbolType s, QString original) : text(original),floatValue(0), intValue(0),isInteger(false), pos(pos),    type(s) { 	};

			
			/// yes, yes, it is a bloated representation. (I don't like unions...)
			QString text;         // The original text-string we parsed. Notice userstrings are converted to lower-case
			double floatValue;
			int    intValue;
			bool   isInteger;
			int pos;              // the position (char-index) of the original text parsed.
			SymbolType type;


			double getNumerical() {
				if (isInteger) return intValue;
				return floatValue;
			}
		};

		/// The Tokenizer divides an input stream into distinct symbols,
		/// for subsequent parsing.
		class Tokenizer {

		public:
			/// Constructor. 
			Tokenizer(QString input);

			/// Destructor
			~Tokenizer();

			/// Returns the next symbol
			Symbol getSymbol();

		private:
		
			QList<Symbol> symbols;
			int currentSymbol;
		};

	}
}

