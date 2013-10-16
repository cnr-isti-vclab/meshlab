#pragma once

#include <QString>
#include <QList>


namespace SyntopiaCore {
	namespace Misc {	

		/// For keeping track of versions
		/// Having a formalized version object, can
		/// be helpful when checking the internet for updates.
		class Version {
		public:
			/// Constructor.
			/// Settings revision or build to -1 indicates they are not in use.
			Version();
			Version(int major, int minor, int revision = -1, int build = -1, QString codename = "");

			/// Long human-readable version.
			QString toLongString() const;

			/// Returns a list of versions, newer than 'this' version,
			/// by reading an XML-file from the specified URL.
			///
			/// Todo: Implement
			QList<Version> GetNewVersions(QString url) const;

			/// Comparisons
			bool operator<(const Version &rhs);
			bool operator>(const Version &rhs);
			bool operator==(const Version &rhs);

		private:
			int major;
			int minor;
			int revision;
			int build;
			QString codename;
		};


	}
}


