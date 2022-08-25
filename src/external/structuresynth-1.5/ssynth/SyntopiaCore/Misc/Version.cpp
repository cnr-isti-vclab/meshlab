#include "Version.h"

#include "../Logging/Logging.h"

using namespace SyntopiaCore::Logging;

namespace SyntopiaCore {
	namespace Misc {	

		Version::Version() : revision(0), build(0), codename("") {	
			this->major = 0;
			this->minor = 0;
		}

		Version::Version(int major, int minor, int revision, int build, QString codename)  :  revision(revision), build(build), codename(codename) {
			this->major = major;
			this->minor = minor;
		}

		QList<Version> Version::GetNewVersions(QString /*url*/) const {
			return QList<Version>();
		}

		QString Version::toLongString() const {
			QString s = QString("%1.%2").arg(major).arg(minor);
			if (revision >= 0) s+= QString(".%3").arg(revision);
			if (build >= 0) s+= QString(".%4").arg(build);

			if (!codename.isEmpty()) s+= " " + codename;

			return s;
		}

		bool Version::operator<(const Version &rhs) {
			if (major == rhs.major) {
				if (minor == rhs.minor) {
					if (revision == rhs.revision) {
						return (build < rhs.build);
					} else {
						return (revision < rhs.revision);
					}
				} else {
					return (minor < rhs.minor);
				}
			} else {
				return (major < rhs.major);
			}

		}

		bool Version::operator>(const Version &rhs) {
			if ( (*this) == rhs ) return false;
			return !((*this)<rhs);
		}

		bool Version::operator==(const Version &rhs) {
			return ((major == rhs.major) && (minor == rhs.minor) && (revision == rhs.revision) && (build == rhs.build));
		}


	}
}

