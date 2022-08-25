#include "ColorPool.h"

#include "../../SyntopiaCore/Logging/Logging.h"
#include "../../SyntopiaCore/Exceptions/Exception.h"
#include "Builder.h"
#include "RandomStreams.h"


#include <QFile>
#include <QFileInfo>


using namespace SyntopiaCore::Logging;
using namespace SyntopiaCore::Exceptions;

namespace StructureSynth {
	namespace Model {	

		ColorPool::ColorPool(QString initString) {
			initString = initString.toLower();
			picture = 0;

			if (initString == "randomhue") {
				type = RandomHue;
			} else if (initString == "greyscale" || initString == "grayscale" ) {
				type = GreyScale;
			} else if (initString == "randomrgb") {
				type = RandomRGB;
			}  else if (initString.startsWith("image:")) {
				initString = initString.remove("image:");
				type = Picture;

				if (!QFile::exists(initString)) {
					throw Exception(QString("Could not open file: %1").arg(QFileInfo(initString).absoluteFilePath()));			
				}

				picture = new QImage(initString);
				if (picture->isNull()) {
						throw Exception(QString("Could not parse image file: %1").arg(QFileInfo(initString).absoluteFilePath()));				
				}

			}  else if (initString.startsWith("list:")) {
				initString = initString.remove("list:");
				QStringList l = initString.split(",");
				for (int i = 0; i < l.count(); i++) {
					QColor c(l[i]);
					if (!c.isValid()) {
							throw Exception(QString("Could not parse color in colorlist: %1").arg(initString));			
					}
					colorList.append(c);
				}
				type = ColorList;
			} else {
				throw Exception(QString("Could not understand the color pool: %1. Try: RandomHue, RandomRGB, GrayScale, Image:test.png, List:#234,Red,Blue").arg(initString));
			}
		}

		ColorPool::~ColorPool() {
			delete picture;
		}

		QColor ColorPool::drawColor() {
			if (type == RandomHue) {
				return QColor::fromHsv(RandomStreams::Color()->getInt(359),255,255);
			} else if (type == GreyScale) {
				int r = RandomStreams::Color()->getInt(255);
				return QColor(r,r,r).toHsv();
			} else if (type == RandomRGB) {
				// We can only pull one random number, so we must use a few tricks to get three ints
				int r = RandomStreams::Color()->getInt(255);
				int g = RandomStreams::Color()->getInt(255);
				int b = RandomStreams::Color()->getInt(255); 
				return QColor(r,g,b).toHsv();
			} else if (type == Picture) {
				int x = RandomStreams::Color()->getInt(picture->width()-1);
				int y = RandomStreams::Color()->getInt(picture->height()-1);
				QRgb rgb = picture->pixel(x,y);
				return QColor(rgb).toHsv();
			} else if (type == ColorList) {
				int id = RandomStreams::Color()->getInt(colorList.count()-1);
				return colorList[id];
			} 
			return QColor();
		}
	}
}

