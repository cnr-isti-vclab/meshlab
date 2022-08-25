#include "Preprocessor.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>

#include "../../SyntopiaCore/Exceptions/Exception.h"
#include "../../SyntopiaCore/Logging/Logging.h"
#include "../../SyntopiaCore/Math/Random.h"

using namespace SyntopiaCore::Exceptions;
using namespace SyntopiaCore::Logging;
using namespace SyntopiaCore::Math;


namespace StructureSynth {
	namespace Parser {	

		QString Preprocessor::Process(QString input, int seed) {
			RandomNumberGenerator rg;
			rg.setSeed(seed);

			QStringList in = input.split(QRegExp("\r\n|\r|\n"));

			QMap<QString, QString> substitutions;
			QRegExp ppCommand("^#"); // Look for #define varname value
			QRegExp defineCommand("^#define\\s([^\\s]+)\\s(.*)*$"); // Look for #define varname value
			QRegExp defineFloatCommand("^#define\\s([^\\s]+)\\s(.*)\\s\\(float:([^\\s]*)\\)$"); // Look for #define varname value 
			QRegExp defineIntCommand("^#define\\s([^\\s]+)\\s(.*)\\s\\(int:([^\\s]*)\\)$"); // Look for #define varname value 

			// Match a number: [-+]?[0-9]*\.?[0-9]+
			QRegExp randomNumber("random\\[([-+]?[0-9]*\\.?[0-9]+),([-+]?[0-9]*\\.?[0-9]+)\\]"); // random[-2.3,3.4]

			for (QStringList::iterator it = in.begin(); it != in.end(); ++it) {

				if (ppCommand.indexIn(*it) != -1) {
					// Preprocessor command

					if (defineFloatCommand.indexIn(*it) != -1) {
						//INFO(QString("Found ppC (%1)->(%2): ").arg(defineCommandWithGUI.cap(1)).arg(defineCommandWithGUI.cap(2)) + *it);
						if (defineFloatCommand.cap(2).contains(defineFloatCommand.cap(1))) {
							WARNING(QString("#define command is recursive - skipped: %1 -> %2")
								.arg(defineFloatCommand.cap(1))
								.arg(defineFloatCommand.cap(2)));
						}
						//substitutions[defineCommandWithGUI.cap(1)] = defineCommandWithGUI.cap(2);
						QString defaultValue = defineFloatCommand.cap(2);
						QString floatInterval = defineFloatCommand.cap(3);
						QStringList fi = floatInterval.split("-");
						if (fi.count() != 2) {
							WARNING("Could not understand #define gui command: " + floatInterval);
							continue;
						}
						bool succes = false;
						double d1 = fi[0].toDouble(&succes);
						bool succes2 = false;
						double d2 = fi[1].toDouble(&succes2);
						if (!succes || !succes2) {
							WARNING("Could not parse float interval in #define gui command: " + floatInterval);
							continue;
						}
						bool succes3 = false;
						double d3 = defineFloatCommand.cap(2).toDouble(&succes3);
						if (!succes3) {
							WARNING("Could not parse default argument in #define gui command: " + defineFloatCommand.cap(2));
							continue;
						}
						FloatParameter* fp= new FloatParameter(defineFloatCommand.cap(1), d1, d2, d3);
						params.append(fp);
						
					} else if (defineIntCommand.indexIn(*it) != -1) {
						INFO("INT");
						if (defineIntCommand.cap(2).contains(defineIntCommand.cap(1))) {
							WARNING(QString("#define command is recursive - skipped: %1 -> %2")
								.arg(defineIntCommand.cap(1))
								.arg(defineIntCommand.cap(2)));
						}
						QString defaultValue = defineIntCommand.cap(2);
						QString intInterval = defineIntCommand.cap(3);
						QStringList ii = intInterval.split("-");
						if (ii.count() != 2) {
							WARNING("Could not understand #define gui command: " + intInterval);
							continue;
						}
						bool succes = false;
						int i1 = ii[0].toInt(&succes);
						bool succes2 = false;
						int i2 = ii[1].toInt(&succes2);
						if (!succes || !succes2) {
							WARNING("Could not parse int interval in #define gui command: " + intInterval);
							continue;
						}
						bool succes3 = false;
						int i3 = defineIntCommand.cap(2).toInt(&succes3);
						if (!succes3) {
							WARNING("Could not parse default argument in #define gui command: " + defineIntCommand.cap(2));
							continue;
						}
						IntParameter* fp= new IntParameter(defineIntCommand.cap(1), i1, i2, i3);
						params.append(fp);
						
					} else if (defineCommand.indexIn(*it) != -1) {
						if (defineCommand.cap(2).contains(defineCommand.cap(1))) {
							WARNING(QString("#define command is recursive - skipped: %1 -> %2")
								.arg(defineCommand.cap(1))
								.arg(defineCommand.cap(2)));
						}
						substitutions[defineCommand.cap(1)] = defineCommand.cap(2);
					} else {
						WARNING("Could not understand preprocessor command: " + *it);
					}
				} else {
					// Non-preprocessor command
					// Check for substitutions.
					QMap<QString, QString>::const_iterator it2 = substitutions.constBegin();
					int subst = 0;
					while (it2 != substitutions.constEnd()) {
						if (subst>100) {
							WARNING("More than 100 recursive preprocessor substitutions... breaking.");
							break;
						}
						if ((*it).contains(it2.key())) {
							//INFO("Replacing: " + it2.key() + " with " + it2.value());
							(*it).replace(it2.key(), it2.value());

							it2 = substitutions.constBegin();
							subst++;
						} else {
							it2++;
						}
				    }
				}

				while (randomNumber.indexIn(*it) != -1) {
					double d1 = randomNumber.cap(1).toDouble();
					double d2 = randomNumber.cap(2).toDouble();
					double r = rg.getDouble(d1,d2);
					INFO(QString("Random number: %1 -> %2 ").arg(randomNumber.cap(0)).arg(r));
					(*it).replace(randomNumber.cap(0), QString::number(r));
				}
				
			}

			QStringList out = in;
			return out.join("\r\n");
		}
	}
}

