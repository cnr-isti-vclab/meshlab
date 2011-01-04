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
 
 package org.libfreenect {
	 
	import org.libfreenect.libfreenect;
	import org.libfreenect.events.libfreenectSocketEvent;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.ProgressEvent;
	import flash.events.IOErrorEvent;
	
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	/**
	 * LibFreenectSocket class recieves Kinect data from the libfreenect driver.
	 */
	public class libfreenectSocket extends EventDispatcher
	{
		private static var _instance:libfreenectSocket;
		private var _packet_size:Number;
		private var _socket:Socket;
		private var _buffer:ByteArray;
		private var _port:Number;

		public function libfreenectSocket()
		{		
			_socket = new Socket();
			_buffer = new ByteArray();

			_socket.addEventListener(ProgressEvent.SOCKET_DATA, onSocketData);
			_socket.addEventListener(IOErrorEvent.IO_ERROR, onSocketError);
			_socket.addEventListener(Event.CONNECT, onSocketConnect);
		}
		
		public function connect(host:String = 'localhost', port:uint = 6003):void
		{
			_port = port;
			_packet_size = (_port == 6003) ? libfreenect.DATA_IN_SIZE : libfreenect.RAW_IMG_SIZE;
			if (!this.connected) 
				_socket.connect(host, port);
			else
				dispatchEvent(new libfreenectSocketEvent(libfreenectSocketEvent.ONCONNECT, null));
		}
		
		public function get connected():Boolean
		{
			return _socket.connected;
		}
		
		public function close():void
		{
			_socket.close();
		}
		
		public function sendData(data:ByteArray):int{
			if(data.length == libfreenect.DATA_OUT_SIZE){
				trace("sendData");
				_socket.writeBytes(data, 0, libfreenect.DATA_OUT_SIZE);
				_socket.flush();
				return libfreenect.SUCCESS;
			} else {
				throw new Error( 'Incorrect data size (' + data.length + '). Expected: ' + libfreenect.DATA_OUT_SIZE);
				return libfreenect.ERROR;
			}
		}
		
		private function onSocketData(event:ProgressEvent):void
		{
			if(_socket.bytesAvailable == 237){
				var _byte_arr:ByteArray = new ByteArray();
				_socket.readBytes(_byte_arr, 0, _socket.bytesAvailable);
				trace("policy_file : " + _byte_arr);
			}
			if(_socket.bytesAvailable > 0) {
				if(_socket.bytesAvailable >= _packet_size){
					_socket.readBytes(_buffer, 0, _packet_size);
					_buffer.endian = Endian.LITTLE_ENDIAN;
					_buffer.position = 0;
					dispatchEvent(new libfreenectSocketEvent(libfreenectSocketEvent.ONDATA, _buffer));
				}
			}
		}
		
		private function onSocketError(event:IOErrorEvent):void{
			dispatchEvent(new libfreenectSocketEvent(libfreenectSocketEvent.ONERROR, null));
		}
		
		private function onSocketConnect(event:Event):void{
			dispatchEvent(new libfreenectSocketEvent(libfreenectSocketEvent.ONCONNECT, null));
		}

		public function set instance(instance:libfreenectSocket):void 
		{
			throw new Error('libfreenectSocket.instance is read-only');
		}
		
		public static function get instance():libfreenectSocket 
		{
			if ( _instance == null )
			{
				_instance = new libfreenectSocket();
			}
			return _instance;
		}
	}
}