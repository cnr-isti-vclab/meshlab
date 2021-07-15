/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef _PUSHPULL_H
#define _PUSHPULL_H

typedef unsigned char Byte;

#include <QImage>
#include <QRgb>

namespace vcg
{
    /* pull push filling algorithm */

    static int mean4w(int p1,Byte w1,int p2,Byte w2,int p3,Byte w3,int p4,Byte w4)
    {
        int result =(p1*int(w1) + p2*int(w2)  +p3*int(w3) + p4*int(w4) )
        / ( int(w1)+int(w2)+int(w3)+int(w4)  ) ;
        return result;
    }

    static QRgb mean4Pixelw(QRgb p1,Byte w1,QRgb p2,Byte w2,QRgb p3,Byte w3,QRgb p4,Byte w4)
    {
        int r= mean4w(qRed(p1),w1,qRed(p2),w2,qRed(p3),w3,qRed(p4),w4);
        int g= mean4w(qGreen(p1),w1,qGreen(p2),w2,qGreen(p3),w3,qGreen(p4),w4);
        int b= mean4w(qBlue(p1),w1,qBlue(p2),w2,qBlue(p3),w3,qBlue(p4),w4);
        int a= mean4w(qAlpha(p1),w1,qAlpha(p2),w2,qAlpha(p3),w3,qAlpha(p4),w4);

        return qRgba(r,g,b,a);

    }

    // Genera una mipmap pesata
    static void PullPushMip( QImage & p, QImage & mip, QRgb  bkcolor )
    {
        assert(p.width()/2==mip.width());
        assert(p.height()/2==mip.height());
        Byte w1,w2,w3,w4;
        int x,y;
        for(y=0;y<mip.height();++y)
            for(x=0;x<mip.width();++x)
            {
                if(p.pixel(x*2  ,y*2  )==bkcolor) w1=0; else w1=255;
                if(p.pixel(x*2+1,y*2  )==bkcolor) w2=0; else w2=255;
                if(p.pixel(x*2  ,y*2+1)==bkcolor) w3=0; else w3=255;
                if(p.pixel(x*2+1,y*2+1)==bkcolor) w4=0; else w4=255;
                if(w1+w2+w3+w4>0        )
                    mip.setPixel(x, y, mean4Pixelw(
                                                   p.pixel(x*2  ,y*2  ),w1,
                                                   p.pixel(x*2+1,y*2  ),w2,
                                                   p.pixel(x*2  ,y*2+1),w3,
                                                   p.pixel(x*2+1,y*2+1),w4 ));
            }
    }

    // interpola a partire da una mipmap
    static void PullPushFill( QImage & p, QImage & mip, QRgb  bkg )
    {
        assert(p.width()/2==mip.width());
        assert(p.height()/2==mip.height());
        //      byte w1,w2,w3,w4;
        int x,y;
        for(y=0;y<mip.height();++y)
            for(x=0;x<mip.width();++x)
            {
                if(p.pixel(x*2 ,y*2)==bkg)
                    p.setPixel(x*2 ,y*2 ,mean4Pixelw( mip.pixel(x  ,y  ) ,  Byte(144),
                                                     (x>0 ? mip.pixel(x-1,y  ) : bkg),  (x>0 ? Byte( 48) : 0),
                                                     (y>0 ? mip.pixel(x  ,y-1) : bkg),  (y>0 ? Byte( 48) : 0),
                                                     ((x>0 && y>0 )? mip.pixel(x-1,y-1) : bkg), ((x>0 && y>0 )? Byte( 16) : 0)));
                if(p.pixel(x*2+1,y*2  )==bkg)
                    p.setPixel(x*2+1,y*2  ,mean4Pixelw(mip.pixel(x  ,y  ) ,Byte(144),
                                                       (x<mip.width()-1 ? mip.pixel(x+1,y) : bkg),  (x<mip.width()-1 ? Byte( 48) : 0),
                                                       (y>0  ? mip.pixel(x  ,y-1) : bkg),  (y>0  ? Byte( 48) : 0),
                                                       ((x<mip.width()-1 && y>0) ? mip.pixel(x+1,y-1) : bkg), ((x<mip.width()-1 && y>0) ? Byte( 16) : 0)));
                if(p.pixel(x*2  ,y*2+1)==bkg)
                    p.setPixel(x*2  ,y*2+1, mean4Pixelw( mip.pixel(x  ,y  ), Byte(144),
                                                        (x>0 ? mip.pixel(x-1,y  ) : bkg),  (x>0 ? Byte( 48) : 0),
                                                        (y<mip.height()-1  ? mip.pixel(x  ,y+1) : bkg),  (y<mip.height()-1  ? Byte( 48) : 0),
                                                        ((x>0 && y<mip.height()-1) ? mip.pixel(x-1,y+1) : bkg), ((x>0 && y<mip.height()-1 )? Byte( 16) : 0)));
                if(p.pixel(x*2+1,y*2+1)==bkg)
                    p.setPixel(x*2+1,y*2+1, mean4Pixelw(mip.pixel(x  ,y  ), Byte(144),
                                                        (x<mip.width()-1 ? mip.pixel(x+1,y  ) : bkg), (x<mip.width()-1 ? Byte( 48) : 0),
                                                        (y<mip.height()-1  ? mip.pixel(x  ,y+1) : bkg), ( y<mip.height()-1  ? Byte( 48) : 0),
                                                        ((x<mip.width()-1  && y<mip.height()-1) ? mip.pixel(x+1,y+1) : bkg), ((x<mip.width()-1  && y<mip.height()-1) ? Byte( 16) : 0)));

            }

        // avoid background bleeding on non power-of-two images

        if ((p.width() % 2) != 0) {
            for (y = 0; y < p.height(); ++y) {
                for (x = 2 * mip.width(); x < p.width(); ++x) {
                    if (x > 0 && p.pixel(x, y) == bkg)
                        p.setPixel(x, y, p.pixel(x-1, y));
                }
            }
        }

        if ((p.height() % 2) != 0) {
            for (y = 2 * mip.height(); y < p.height(); ++y) {
                for (x = 0; x < p.width(); ++x) {
                    if (y > 0 && p.pixel(x, y) == bkg)
                        p.setPixel(x, y, p.pixel(x, y-1));
                }
            }
        }


    }


    static void PullPush( QImage & p, QRgb  bkcolor )
    {
        int i=0;
        std::vector<QImage> mip(16);
        int div=2;
        int miplev=0;

        // pull phase create the mipmap
        while(1){
            mip[miplev]= QImage(p.width()/div,p.height()/div,p.format());
            mip[miplev].fill(bkcolor);
            div*=2;
            if(miplev>0) PullPushMip(mip[miplev-1],mip[miplev],bkcolor);
            else PullPushMip(p,mip[miplev],bkcolor);
            //if(mip[miplev].width()<=4 || mip[miplev].height()<=4) break;
            if(mip[miplev].width()<=1 || mip[miplev].height()<=1) break;
            ++miplev;
        }
        miplev++;
#ifdef _PUSHPULL_DEBUG
        for(int k=0;k<miplev;k++)               {
            char buf[100];sprintf(buf,"mip%02i.png",k);
            mip[k].Save(buf);
        }
#endif
        // push phase: refill
        for(i=miplev-1;i>=0;--i){
            if(i>0) PullPushFill(mip[i-1],mip[i],bkcolor);
            else PullPushFill(p,mip[i],bkcolor);
        }

#ifdef _PUSHPULL_DEBUG
        for(k=0;k<miplev;k++)           {
            char buf[100];sprintf(buf,"mipfill%02i.png",k);
            mip[k].Save(buf);
        }
#endif
    }
}       // End namespace
#endif
