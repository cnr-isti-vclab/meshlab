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
	import org.libfreenect.libfreenectSocket;
	import org.libfreenect.events.libfreenectSocketEvent;
	import org.libfreenect.events.libfreenectDataEvent;
	import flash.events.EventDispatcher;
	import flash.utils.ByteArray;
	
	public class libfreenectData extends EventDispatcher
	{
		private static var _singleton_lock:Boolean = false;
		private static var _instance:libfreenectData;
		
		private var socket:libfreenectSocket;

		public function libfreenectData()
		{
			if ( !_singleton_lock ) throw new Error( 'Use libfreenectData.instance' );
				
			socket = new libfreenectSocket();
			
			socket.addEventListener(libfreenectSocketEvent.ONDATA,onDataReceived);
			
			socket.connect("localhost", 6003);
		}
		
		private function onDataReceived(event:libfreenectSocketEvent):void{
			var data:ByteArray = event.data;
			var object:Object = new Object();
			object.ax = data.readShort();
			object.ay = data.readShort();
			object.az = data.readShort();
			object.dx = data.readDouble();
			object.dy = data.readDouble();
			object.dz = data.readDouble();
			dispatchEvent(new libfreenectDataEvent(libfreenectDataEvent.DATA_RECEIVED, object));
		}
		
		public function sendData(data:ByteArray):int{
			return socket.sendData(data);
		}
		
		private function onError(event:libfreenectSocketEvent):void {
			throw new Error('Socket IO Error');
		}

		public static function set instance(instance:libfreenectData):void 
		{
			throw new Error('libfreenectData.instance is read-only');
		}
		
		public static function get instance():libfreenectData 
		{
			if ( _instance == null )
			{
				_singleton_lock = true;
				_instance = new libfreenectData();
				_singleton_lock = false;
			}
			return _instance;
		}
	}
}