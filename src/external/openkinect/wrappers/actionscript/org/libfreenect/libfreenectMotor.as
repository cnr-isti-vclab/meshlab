/*
 * 
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 * 
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file 
 * for details.
 * 
 * This code is licensed to you under the terms of the Apache License, version 
 * 2.0, or, at your option, the terms of the GNU General Public License, 
 * version 2.0. See the APACHE20 and GPL20 files for the text of the licenses, 
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * If you redistribute this file in source form, modified or unmodified, 
 * you may:
 * 1) Leave this header intact and distribute it under the same terms, 
 * accompanying it with the APACHE20 and GPL20 files, or
 * 2) Delete the Apache 2.0 clause and accompany it with the GPL20 file, or
 * 3) Delete the GPL v2.0 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy 
 * of the CONTRIB file.
 * Binary distributions must follow the binary distribution requirements of 
 * either License.
 * 
 */
package org.libfreenect
{
	import org.libfreenect.libfreenect;
	import org.libfreenect.libfreenectData;
	import org.libfreenect.events.libfreenectDataEvent;
	import org.libfreenect.events.libfreenectMotorEvent;
	
	import flash.utils.ByteArray;
	import flash.events.EventDispatcher;
	
	public class libfreenectMotor extends EventDispatcher
	{
		private static var _singleton_lock:Boolean = false;
		private static var _instance:libfreenectMotor;
		
		private var _current_position:Number;
		public var _info:libfreenectData;

		public function libfreenectMotor()
		{
			if ( !_singleton_lock ) throw new Error( 'Use libfreenectMotor.instance' );
				
			_info = libfreenectData.instance;
			_info.addEventListener(libfreenectDataEvent.DATA_RECEIVED, onDataReceived);
		}
		
		private function onDataReceived(event:libfreenectDataEvent):void{
			var object:Object = event.data; // Acelerometer _infoo so far
		}
		
		public function set position(position:Number):void 
		{
			var data:ByteArray = new ByteArray;
			data.writeByte(libfreenect.MOTOR_ID);
			data.writeByte(1); //MOVE MOTOR
			data.writeInt(position);
			if(_info.sendData(data) == libfreenect.SUCCESS){
				dispatchEvent(new libfreenectMotorEvent(libfreenectMotorEvent.MOVED, position));
			} else {
				throw new Error('Data was not complete');
			}
			
		}
		
		public function get position():Number{
			return _current_position;
		}
		
		public static function set instance(instance:libfreenectMotor):void 
		{
			throw new Error('libfreenectMotor.instance is read-only');
		}
		
		public static function get instance():libfreenectMotor 
		{
			if ( _instance == null )
			{
				_singleton_lock = true;
				_instance = new libfreenectMotor();
				_singleton_lock = false;
			}
			return _instance;
		}
	}
}