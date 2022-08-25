#pragma once

#include <QString>
#include <QList>


namespace SyntopiaCore {
	namespace Misc {	

		/// Small class for parsing values from simple text strings.
		class MiniParser {
		public:
			MiniParser( QString value, QChar separator = ',');

			MiniParser& getInt(int& val);				
			MiniParser& getBool(bool& val);
			MiniParser& getDouble(double& val);
			MiniParser& getFloat(float& val);
		private:
			QChar separator;
			QString original;
			QString value;
			int paramCount ;
		};

	}
}


