#pragma once

#include <QString>
#include <QVector>
#include <QTime>
#include <QStack>

namespace SyntopiaCore {
	namespace Logging {	
		/// Predefined logging levels
		enum LogLevel { NoneLevel, DebugLevel, TimingLevel, InfoLevel, WarningLevel, CriticalLevel, AllLevel };

		/// Abstract base class for all loggers
		class Logger {
		public:
			/// The destructors and constructors automatically add to the list of installed loggers.
			Logger() { 
				loggers.append(this); 
			}

			virtual ~Logger() { 
				// Remove from list of available loggers.
				for (int i = loggers.size()-1; i >= 0; i--) {
					if (loggers[i] == this) loggers.remove(i);
				}
			}

			/// This method all loggers must implement
			virtual void log(QString message, LogLevel priority) = 0;
			
			/// Log messages are sent to this list of loggers.
			static QVector<Logger*> loggers;
			static QStack<QTime> timeStack;
			static QStack<QString> timeStringStack;
		private:
			
		};


		void LOG(QString message, LogLevel priority);

		/// Useful aliases
		void Debug(QString text);
		void INFO(QString text);
		void TIME(QString text);
		void TIME(int repetitions = 0); // End time...
		void WARNING(QString text);
		void CRITICAL(QString text);
		
	}
}

