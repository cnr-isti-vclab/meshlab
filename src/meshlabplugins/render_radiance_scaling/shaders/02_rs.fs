/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
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
#version 120
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D grad;  // (gx,gy,depth,1.0)
uniform sampler2D norm;  // (nx,ny,nz,depth)
uniform sampler2D colormap; // (r,g,b,1.0)
uniform float     sw;
uniform float     sh;
uniform float     enhancement;
uniform float     transition;
uniform bool      enabled;
uniform bool      invert;
uniform bool      doubleSide;
uniform int       display;
uniform bool      twoLS;
uniform sampler2D convexLS;
uniform sampler2D concavLS;

// **** GRADIENT DATA ****
// |A|B|C|
// |D|X|E|
// |F|G|H|
vec4 X,A,B,C,D,E,F,G,H;

// **** UTIL FUNCTIONS ****
void loadValues() {
  float xc = gl_TexCoord[0].s;
  float xm = gl_TexCoord[0].s-sw;
  float xp = gl_TexCoord[0].s+sw;

  float yc = gl_TexCoord[0].t;
  float ym = gl_TexCoord[0].t-sh;
  float yp = gl_TexCoord[0].t+sh;

  X = texture2D(grad,vec2(xc,yc));
  A = texture2D(grad,vec2(xm,yp));
  B = texture2D(grad,vec2(xc,yp));
  C = texture2D(grad,vec2(xp,yp));
  D = texture2D(grad,vec2(xm,yc));
  E = texture2D(grad,vec2(xp,yc));
  F = texture2D(grad,vec2(xm,ym));
  G = texture2D(grad,vec2(xc,ym));
  H = texture2D(grad,vec2(xp,ym));
}

vec3 hsvToRgb(in float h,in float s,in float v) {
  vec3 color;
  int hi = int(floor(h/60.0))%6;
  float f = h/60.0 - floor(h/60.0);
  float p = v*(1.0-s);
  float q = v*(1.0-f*s);
  float t = v*(1.0-(1.0-f)*s);

  if(hi==0) color = vec3(v,t,p);
  else if(hi==1) color = vec3(q,v,p);
  else if(hi==2) color = vec3(p,v,t);
  else if(hi==3) color = vec3(p,q,v);
  else if(hi==4) color = vec3(t,p,v);
  else color = vec3(v,p,q);

  return color;
}

float tanh(in float c, in float en) {
  float cmax = en*15.0;
  const float tanhmax = 3.11622;

  float x = ((c*cmax*1.0)/tanhmax);
  float e = exp(-2.0*x);
  float t = clamp((1.0-e)/(1.0+e),-1.0,1.0);

  return t;
}

// **** WARPING FUNCTION ****
float warp(in float impfunc,in float beta) {
  const float alpha = 0.1;
  float expbeta = exp(-beta);
  return (alpha*expbeta+impfunc*(1.0-alpha-alpha*expbeta)) / (alpha+impfunc*(expbeta-alpha-alpha*expbeta));
}

// **** WEIGHT FUNCTIONS ****
float silhouetteWeight(in float s) {
  const float ts = 0.07;
  const float t2 = 0.9+ts;
  const float t1 = t2-0.01;

  return smoothstep(t1,t2,max(1.0-s,0.9));
}

float weight() {
  return (silhouetteWeight(abs(A.z-X.z)) +
    silhouetteWeight(abs(B.z-X.z)) +
    silhouetteWeight(abs(C.z-X.z)) +
    silhouetteWeight(abs(D.z-X.z)) +
    silhouetteWeight(abs(E.z-X.z)) +
    silhouetteWeight(abs(F.z-X.z)) +
    silhouetteWeight(abs(G.z-X.z)) +
    silhouetteWeight(abs(H.z-X.z)))/8.0;
}

// **** CURVATURE FUNCTIONS ****
vec3 hessian() {
  float xx = E.x-D.x;
  float xy = E.y-D.y;
  float yx = B.x-G.x;
  float yy = B.y-G.y;

  return vec3(xx,yy,(xy+yx)/2.0);
}

float curvature(in float w, in vec3 h, in float e) {
  float c = tanh(-(h.x+h.y)/2.0,e);
  return invert ? -c*max(w-0.5,0.0) : c*max(w-0.5,0.0);
}

// **** DESCRIPTOR FUNCTIONS ****
vec4 coloredDescriptor(in float c, in float s) {
  vec3 rgb;
  vec3 convMax = vec3(0.1,0.1,0.8);
  vec3 convMin = vec3(0.2,0.2,0.6);
  vec3 concMax = vec3(0.8,0.1,0.1);
  vec3 concMin = vec3(0.6,0.2,0.2);
  vec3 plane   = vec3(0.7,0.7,0.2);
  float t = 0.02;
  float a;

  if(c<-t) {
    a = (-c-t)/(1.0-t);
    rgb = mix(concMin,concMax,a);
  } else if(c>t) {
    a = (c-t)/(1.0-t);
    rgb = mix(convMin,convMax,a);
  } else if(c<0.0) {
    a = -c/t;
    rgb = mix(plane,concMin,a);
  } else {
    a = c/t;
    rgb = mix(plane,convMin,a);
  }

  if(s<1.0)
    rgb = vec3(0.2);

  return vec4(rgb,1.0);
}

vec4 greyDescriptor(in float c, in float s) {
  return vec4( vec3( (c*0.5+0.5)-(1.0-s) ), 1.0);
}

// **** LIT SPHERE FUNCTIONS ****
vec4 oneLitSphere(in vec3 n,in float c) {
  vec4 color = texture2D(convexLS,(n.xy*0.5)+vec2(0.5));

  return enabled ? color*warp(length(color),c) : color;
}

vec4 twoLitSphere(in vec3 n,in float w,in vec3 h,in float c) {
  const float eps = 0.2;

  vec2 coord = (n.xy*0.5)+vec2(0.5);
  vec4 cconv = texture2D(convexLS,coord);
  vec4 cconc = texture2D(concavLS,coord);
  vec4 color = mix(cconc,cconv,smoothstep(0.5-eps,0.5+eps,curvature(w,h,transition)*0.5+0.5));

  return vec4( (enabled ? color*warp(length(color),c) : color).xyz,1.0);
}

// **** LIGHTING COMPUTATION ****
void main(void) {
  vec3 n = texture2D(norm,gl_TexCoord[0].st).xyz;

  if(n==vec3(0.0)) {
//    gl_FragColor = vec4(1.0);
    discard;  // So we can see the original background, outside the mesh.
    return;
  }

  // data
  loadValues();
  float w = weight();
  vec3  h = hessian();
  float c = curvature(w,h,enhancement);
  vec3  l = normalize(gl_LightSource[0].position.xyz);
  //vec4  m = vec4(0.5,0.1,0.7,1.0);
  //vec4  m = vec4(n,1.0);
  vec4  m = vec4(texture2D(colormap,gl_TexCoord[0].st).xyz,1.0);
  //vec4  m = gl_FrontMaterial.diffuse;

  //Initialize the depth of the fragment with the just saved depth
  gl_FragDepth = texture2D(norm,gl_TexCoord[0].st).w;

  if(display==0)
  {
    // lambertian lighting
    float cosineTerm = doubleSide ? max(dot(n,l),-dot(n,l)) : max(dot(n,l),0.0);
    float warpedTerm = enabled ? cosineTerm*warp(cosineTerm,c) : cosineTerm;
    gl_FragColor = vec4(m.rgb*warpedTerm,1.0);
  }
  else if(display==1)
  {
    // using lit spheres
    if(twoLS) {
      gl_FragColor = twoLitSphere(n,w,h,c);
    } else {
      gl_FragColor = oneLitSphere(n,c);
    }
  }
  else if(display==2)
  {
    // colored descriptor
    gl_FragColor = coloredDescriptor(c,w);
  }
  else if(display==3)
  {
    // grey descriptor
    gl_FragColor = greyDescriptor(c,w);
  }
}
