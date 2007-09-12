#ifndef __GLSTATE_H__
#define __GLSTATE_H__

#include <QString>
#include <QDomElement>

class GlState
{
	public:
		QString name;
		int state;
		int value;
		bool valid;

		GlState( QString & _name, int _state, int _value ) {
			name = _name;
			state = _state;
			value = _value;
			valid = true;
		}

		GlState( QDomElement xml ) {
			bool ok1,ok2;
			name = xml.attribute("NAME");
			state = xml.attribute("STATE").toUInt( &ok1 );
			value = xml.attribute("VALUE").toUInt( &ok2 );
			
			valid = ( xml.tagName() == "RmState" && xml.attribute("API") == "OpenGL" && ok1 && ok2 );
		}


		bool isValid() { return valid; }
		int getValue() { return value; }
		int getState() { return state; }
		QString & getName() { return name; }
		
};

#endif

