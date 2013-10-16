#pragma once

#include <QMutex>
#include <QWaitCondition>

namespace SyntopiaCore {
	namespace GLEngine {	

		class AtomicCounter {
		public:
			AtomicCounter() : current(0) { };
			
			int increase() {
				mutex.lock(); 
				int i = ++current; 
				mutex.unlock(); 
				wc.wakeAll(); 
				return i; 
			}

			void setValue(int value) {
				mutex.lock(); 
				current = value; 
				mutex.unlock(); 
				wc.wakeAll(); 
			}

			bool wait(unsigned long time = ULONG_MAX) { wcm.lock(); bool w = wc.wait(&wcm,time); wcm.unlock(); return w; }
			int value() { mutex.lock(); int i = current; mutex.unlock(); return i; }
		private:
			int current;
			QMutex mutex;
			QMutex wcm;
			QWaitCondition wc;
		};


	}
}

