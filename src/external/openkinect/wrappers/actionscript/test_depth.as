/*
 * 
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 * 
 * Copyright (c) 2010 Juan Carlos del Valle <jc.ekinox@gmail.com> (imekinox)
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

package 
{
	import flash.display.Sprite;
	import flash.display.MovieClip;
	import flash.display.BitmapData;
	import flash.display.Bitmap;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.text.TextField;

	import org.libfreenect.libfreenectCamera;
	import org.libfreenect.libfreenectData;
	import org.libfreenect.libfreenectMotor;
	import org.libfreenect.events.libfreenectDataEvent;
	import org.libfreenect.libfreenect;
	import flash.events.MouseEvent;
	
	[SWF(width='640', height='480', backgroundColor='#000000', frameRate='24')]
	
	public class test_depth extends Sprite
	{
		private var cam:libfreenectCamera;
		private var info:TextField;
					
		function test_depth():void
		{
			info = new TextField();
            info.x = 0;
			info.y = 0;
            info.width = 180;
			info.height = 155;
			info.textColor = 0x000000;
            info.text = "Drag to select some of this text.";
			
			var btn_up:MovieClip = new MovieClip();
			btn_up.graphics.beginFill(0x0000FF);
			btn_up.graphics.drawRect(0, 0, 50, 50);
			btn_up.graphics.endFill();
			btn_up.x = 640 - 50;
			btn_up.y = 480 - 110;
			
			var btn_down:MovieClip = new MovieClip();
			btn_down.graphics.beginFill(0x00FF00);
			btn_down.graphics.drawRect(0, 0, 50, 50);
			btn_down.graphics.endFill();
			btn_down.x = 640 - 50;
			btn_down.y = 480 - 50;
			
			var camera:libfreenectCamera = libfreenectCamera.instance;
			camera.initDepthStream(this);

			var inf:libfreenectData = libfreenectData.instance;
			inf.addEventListener(libfreenectDataEvent.DATA_RECEIVED, updateInfo);

			var mot:libfreenectMotor = libfreenectMotor.instance;

			btn_up.addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void{
				mot.position = 25;
			});

			btn_down.addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void{
				mot.position = 0;
			});
			
			addChild(info);
			addChild(btn_up);
			addChild(btn_down);
		}

		private function updateInfo(event:libfreenectDataEvent): void{
			var object:Object = event.data;
			info.text = "raw acceleration:\n\tax: "+object.ax+"\n\tay: "+object.ay+"\n\taz: "+object.az+"\n\nmks acceleration:\n\tdx: "+object.dx+"\n\tdy: "+object.dy+"\n\tdz: "+object.dz;
		}
	}
}