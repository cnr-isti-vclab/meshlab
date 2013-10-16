#pragma once

#include <QString>
#include <QList>
#include <QVector>

#include "../../SyntopiaCore/Exceptions/Exception.h"

namespace StructureSynth {
	namespace Parser {	

		class GuiParameter {
		public:
			GuiParameter(QString name) : name(name) {};
			virtual QString getName() { return name; }
		protected:
			QString name;
		};

		class FloatParameter : public GuiParameter {
		public:
			FloatParameter(QString name, double from, double to, double defaultValue) :
					GuiParameter(name), from(from), to(to), defaultValue(defaultValue) {};
			
			double getFrom() { return from; }
			double getTo() { return to; }
			double getDefaultValue() { return defaultValue; }
		private:
			double from;
			double to;
			double defaultValue;
		};

		class IntParameter : public GuiParameter {
		public:
			IntParameter(QString name, int from, int to, int defaultValue) :
					GuiParameter(name), from(from), to(to), defaultValue(defaultValue) {};
			
			int getFrom() { return from; }
			int getTo() { return to; }
			int getDefaultValue() { return defaultValue; }
		private:
			int from;
			int to;
			int defaultValue;
		};

		/// The preprocessor is responsible for expanding '#define'
		///
		class Preprocessor {

		public:
			Preprocessor() {};
			
			// The preprocess replaces 'random[2,4]' statements with random numbers.
			// This requires a seed. Using the same seed as controls the EisenScript is probably the best idea her.
			QString Process(QString input, int seed = 0);
			QVector<GuiParameter*> getParameters() { return params; }
			
		private:
			QVector<GuiParameter*> params;
		};

	}
}

