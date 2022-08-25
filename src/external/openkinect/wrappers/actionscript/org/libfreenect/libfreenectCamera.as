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
	import org.libfreenect.events.libfreenectCameraEvent;
	
	import flash.display.Sprite;
	import flash.display.BitmapData;
	import flash.display.Bitmap;
	import flash.utils.ByteArray;
	
	import flash.events.EventDispatcher;
	import flash.events.Event;
	import flash.geom.Rectangle;
	
	public class libfreenectCamera extends EventDispatcher
	{
		private static var _singleton_lock:Boolean = false;
		private static var _instance:libfreenectCamera;
		private var canvas_rgb:BitmapData 
		private var canvas_depth:BitmapData 
		private var rgb_bmp:Bitmap;
		private var depth_bmp:Bitmap;
		private var socket_rgb:libfreenectSocket;
		private var socket_depth:libfreenectSocket;
		
		public function libfreenectCamera()
		{
			if ( !_singleton_lock ) throw new Error( 'Use libfreenectCamera.instance' );
		}
		
		private function onDepthReceived(event:libfreenectSocketEvent):void{
			event.data.position = 0;
			canvas_depth.lock();
			canvas_depth.setPixels(new Rectangle(0,0,640, 480), event.data);
			canvas_depth.unlock();
			event.data.clear();
		}
		private function onRGBReceived(event:libfreenectSocketEvent):void{
			event.data.position = 0;
			canvas_rgb.lock();
			canvas_rgb.setPixels(new Rectangle(0,0,640, 480), event.data);
			canvas_rgb.unlock();
		}
		
		public function initRGBStream(container:Sprite):void{
			socket_rgb = new libfreenectSocket();
			socket_rgb.addEventListener(libfreenectSocketEvent.ONDATA,onRGBReceived);
			if(!canvas_rgb) canvas_rgb = new BitmapData(640, 480, false, 0xFF000000);
			rgb_bmp = new Bitmap(canvas_rgb);
			container.addChild(rgb_bmp);
			socket_rgb.connect("localhost",6002);	
		}
		
		public function initDepthStream(container:Sprite):void{
			socket_depth = new libfreenectSocket();
			socket_depth.addEventListener(libfreenectSocketEvent.ONDATA,onDepthReceived);
			if(!canvas_depth) canvas_depth = new BitmapData(640, 480, false, 0xFF000000);
			depth_bmp = new Bitmap(canvas_depth);
			container.addChild(depth_bmp);
			socket_depth.connect("localhost",6001);
		}
		
		public function on():void 
		{
/*			var data:ByteArray = new ByteArray;
			data.writeByte(libfreenect.LIBFREENECT_CAMERA);
			data.writeByte(0);
			if(socket.sendData(data)){
				dispatchEvent(new libfreenectCameraEvent(libfreenectCameraEvent.LIBFREENECT_CAMERA_ON, null));
			} else {
				throw new Error('Data was not complete');
			}*/
		}
		
		public function off():void 
		{
/*			data.writeByte(libfreenect.LIBFREENECT_CAMERA);
			data.writeByte(1);
			if(socket.sendData(data)){
				dispatchEvent(new libfreenectCameraEvent(libfreenectCameraEvent.LIBFREENECT_CAMERA_OFF, null));
			} else {
				throw new Error('Data was not complete');
			}*/
		}
		
		public static function set instance(instance:libfreenectCamera):void 
		{
			throw new Error('libfreenectCamera.instance is read-only');
		}
		
		public static function get instance():libfreenectCamera 
		{
			if ( _instance == null )
			{
				_singleton_lock = true;
				_instance = new libfreenectCamera();
				_singleton_lock = false;
			}
			return _instance;
		}
	}
}