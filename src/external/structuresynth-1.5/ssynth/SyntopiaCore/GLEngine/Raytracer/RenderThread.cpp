#include "RenderThread.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {


		RenderThread::RenderThread() {
			backgroundColor = Vector3f(0,0,0);
			aoSamples = 1;
			aaSamples = 8;
			useShadows = true;
			copy = false;
			lightPos = Vector3f(0,0,0);

			dofCenter = 0;
			dofFalloff = 0;
			sampler = 0;
			terminated = false;
			//filter = new GaussianFilter(0.75,1);
			//filter = new TriangleFilter(1);
			filter = new BoxFilter();
			maxDepth = 2;
		}


		RenderThread::~RenderThread() {
			delete accelerator;
			delete sampler;
			if (!copy) delete (filter);
		}


		RenderThread::RenderThread(const RenderThread& other) : QThread() {
			rayIDs = other.rayIDs;
			terminated = false;

			frontStart = other.frontStart;
			frontX = other.frontX;
			frontY = other.frontY;
			backStart = other.backStart;
			backX = other.backX;
			backY = other.backY;

			lightPos = other.lightPos;
			backgroundColor = other.backgroundColor;
			accelerator = new VoxelStepper(*other.accelerator);
			accelerator->setCopy(true);

			aoSamples = other.aoSamples;
			maxDepth = other.maxDepth;
			width = other.width;
			height = other.height;
			useShadows = other.useShadows;
			w = other.w;
			h = other.h;

			task = other.task;
			nextUnit = other.nextUnit;
			completedUnits = other.completedUnits;
			maxUnits = other.maxUnits;

			aaSamples = other.aaSamples;

			dofCenter = other.dofCenter;
			dofFalloff = other.dofFalloff;

			copy = true;

			rayID = 0;
			pixels = 0;
			checks = 0;
			rg.randomizeUniformCounter(); // to avoid coherence between threads
			sampler = other.sampler->clone(&rg);
			progressiveOutput = other.progressiveOutput;
			filter = other.filter;
		};



		double RenderThread::getAOStrength(Object3D* object, Vector3f objectNormal, Vector3f objectIntersection) {

			if (aoSamples == 0 || object==0) return 1.0;
			double tests = 0;
			double hits = 0;
			for (int ix = 0; ix < aoSamples*aoSamples; ix++) {

				Vector3f random = sampler->getAODirection(rayNumber*aoSamples*aoSamples+ix);
				if (Vector3f::dot(random, objectNormal)<0) random = random*-1.0; // Only check away from surface.
				random.normalize();

				double maxT = 0;
				QList<Object3D*>* list = accelerator->setupRay(objectIntersection,random, maxT);
				RayInfo ri;
				ri.startPoint = objectIntersection;
				ri.lineDirection = random;
				bool occluded = false;
				while (list != 0 && !occluded) { 
					// check objects
					for (int i = 0; i < list->size(); i++) {
						if (list->at(i) == object) continue; // self-shadow? 							
						occluded = list->at(i)->intersectsRay(&ri);
						if (ri.intersection < 1E-5) occluded = false;
						if (occluded) break;								
					}
					if (!occluded) list = accelerator->advance(maxT); 
				}
				double weight = 1.0; // Vector3f::dot(random, objectNormal);
				if (occluded) hits+=weight;
				tests += weight;			
			}
			return 1-hits/tests;
		}

		void RenderThread::raytrace(int newUnit) {
			int x = newUnit-1;
			float fx = x/(float)(w);
			float xs = (1.0/w);
			float ys = (1.0/h);
			Vector3f* colors = new Vector3f[h];

			for (int y = 0; y < h; y++) {	
				float weightSum = 0.0;
				sampler->prepareSamples(aaSamples,aoSamples);
				float fy = y/(float)(h);
				Vector3f color(0,0,0);
				for (int i = 0; i < aaSamples*aaSamples; i++) {
					rayNumber = i;
					Vector3f ls = sampler->getAASample(rayNumber);
					float weight = 1.0;
					weightSum += weight;
					color = color + weight*rayCastPixel(fx+ls.x()*xs,fy+ls.y()*ys);		
				}
				colors[y] = color / weightSum;
			}

			progressiveOutput->addColumn(x,colors);
			delete[] colors;
			
		};

		void RenderThread::raytraceProgressive(int newUnit) {
			// rayNumber = (newUnit-1);
			RandomNumberGenerator rg;
			rg.setSeed(0);
				
			double* weights = new double[w*h];
			Vector3f* colors = new Vector3f[w*h];
			for (int i = 0; i < w*h; i++) { weights[i] = 0; colors[i] = Vector3f(0,0,0); }
			float xs = (1.0/w);
			float ys = (1.0/h);
			sampler->prepareSamples(aaSamples,aoSamples);
			int extent = filter->getExtent();
			
			for (int y = 0; y < h; y++) {	
						
				int yFrom = y - extent; if (yFrom<0) yFrom=0;
				int yTo = y + extent; if (yTo>=h) yTo=h-1;
			
				float fy = y*ys;
				if (terminated) break;
				for (int x = 0; x < w; x++) {	
					rayNumber = ((newUnit-1) + rg.getInt(maxUnits)) % maxUnits;
				
					int xFrom = x - extent; if (xFrom<0) xFrom=0;
					int xTo = x + extent; if (xTo>=w) xTo=w-1;
			
					float fx = x*xs;
					Vector3f ls = sampler->getAASample(rayNumber);
					Vector3f sample = rayCastPixel(fx+ls.x()*xs,fy+ls.y()*ys);

					for (int xf = xFrom; xf<= xTo; xf++) {
						for (int yf = yFrom; yf<= yTo; yf++) {
							float dx = (xf - x)+ls.x();
							float dy = (yf - y)+ls.y();
							float wi = filter->getWeight(dx*dx,dy*dy);
							colors[xf+yf*w] = colors[xf+yf*w] + wi*sample;
							weights[xf+yf*w] += wi;
						}
					}
				}
			}

			progressiveOutput->addIteration(colors, weights);
			delete[] weights;
			delete[] colors;
		};

		void RenderThread::run() {
			int newUnit = nextUnit->increase();
			while (newUnit <= maxUnits) {
				// do work here...
				switch (task) {
					case Raytrace: raytrace(newUnit); break;
					case RaytraceProgressive: raytraceProgressive(newUnit); break;
					default: throw(1);
				}
				completedUnits->increase();
				newUnit = nextUnit->increase();
			}
		}


		void RenderThread::setCounters(AtomicCounter* nextUnit, AtomicCounter* completedUnits, int maxUnits)
		{
			this->nextUnit = nextUnit;
			this->completedUnits = completedUnits;
			this->maxUnits = maxUnits;
		}

		void RenderThread::alloc(int w, int h) {
			this->w = w;
			this->h = h;
			rayID = 0;
			pixels = 0;
			checks = 0;
		}

		Vector3f RenderThread::rayCastPixel(float x, float y) {

			Vector3f startPoint = frontStart + frontX*x + frontY*y;
			Vector3f endPoint  =   backStart  + backX*x  + backY*y;

			if (dofCenter == 0) {
				Vector3f direction = endPoint - startPoint;
				return rayCast(startPoint, direction, 0);
			} else {
				Vector3f centerPoint =(endPoint-startPoint)* dofCenter+ startPoint;
				// --- Uniform Disc Sampling
				Vector3f displace = sampler->getLensSample(rayNumber)*dofFalloff;
				Vector3f newStartPoint = frontStart + frontX*(x+displace.x())+ frontY*(y+displace.y());	
				Vector3f direction = (centerPoint - newStartPoint)*(1/dofCenter);
				return rayCast(newStartPoint, direction, 0);
			}
		}

		Vector3f RenderThread::rayCast(Vector3f startPoint, Vector3f direction, Object3D* excludeThis, int level) {
			if (level>maxDepth) return Vector3f(backgroundColor.x(),backgroundColor.y(),backgroundColor.z());
			rayID++;
			pixels++;

			double lengthToClosest = -1;
			Vector3f foundNormal;
			GLfloat foundColor[4];
			for (int i = 0; i < 4; i++) foundColor[i] = 0;

			Object3D* bestObj = 0;
			double maxT = 0;
			RayInfo ri;
			QList<Object3D*>* list = accelerator->setupRay(startPoint, direction, maxT);
			ri.startPoint = startPoint;
			ri.lineDirection = direction;			

			while (list != 0) { 
				// check objects
				for (int i = 0; i < list->count(); i++) {
					checks++;

					if (list->at(i) == excludeThis) continue;
					// Check if we already tested this...
					if (rayIDs[list->at(i)->getObjectID()] == rayID) continue;

					bool found = list->at(i)->intersectsRay(&ri);
					rayIDs[list->at(i)->getObjectID()]= rayID;
					if (!found) continue;
					if ((ri.intersection<1E-7)) continue;

					if ((ri.intersection>0) && ((ri.intersection <= lengthToClosest) || (lengthToClosest == -1))) {
						// We hit something and it was closer to us than the object before...
						foundNormal = ri.normal;
						for (int ix = 0; ix < 4; ix++) foundColor[ix] = ri.color[ix];							
						lengthToClosest = ri.intersection;
						bestObj = list->at(i);
					}
				}

				if (bestObj != 0 && lengthToClosest < maxT) break;
				list = accelerator->advance(maxT); 
			}
			// Now we can calculate the lightning.
			if (lengthToClosest>0) {	
				// iPoint is the intersection point in 3D.
				Vector3f iPoint = startPoint + direction*lengthToClosest;
				Vector3f lightDirection = (lightPos-iPoint);
				double light = 0;

				// This is a Phong lightning model, see e.g. (http://ai.autonomy.net.au/wiki/Graphics/Illumination)
				// -- Diffuse light 
				double diffuse = bestObj->getPrimitiveClass()->diffuse*(Vector3f::dot(foundNormal, (lightDirection).normalized()));				
				if (diffuse<0) diffuse = 0;
				light += diffuse;

				// -- Specular light
				double spec = 0;
				double dot = Vector3f::dot(foundNormal, lightDirection);
				if (dot<0.1) {
				} else {
					Vector3f reflected = foundNormal*dot*2 - lightDirection;
					reflected.normalize();
					spec = -(Vector3f::dot(reflected, (direction).normalized()));		
					if (spec< 0.1) {
						spec = 0;
					} else {
						spec = bestObj->getPrimitiveClass()->specular*pow(spec,50);
						if (spec<0) spec = 0;
					}
				}
				light += spec;

				// -- Ambient light
				double aoStrength = 1.0;
				// We will only check for AO at first intersection...
				if (level == 0) aoStrength = getAOStrength(bestObj, foundNormal, iPoint);

				double ambient = bestObj->getPrimitiveClass()->ambient*aoStrength;
				light += ambient; 

				// -- calculate shadow...
				// TODO: Calculate shadow in transperant media
				bool inShadow = false;
				if (useShadows) {
					double maxT = 0;
					QList<Object3D*>* list = accelerator->setupRay(iPoint,(lightPos-iPoint), maxT);
					ri.startPoint = iPoint;
					ri.lineDirection = lightPos-iPoint;

					while (list != 0 && !inShadow) { 
						// check objects
						for (int i = 0; i < list->size(); i++) {
							if (list->at(i) == bestObj) continue; // self-shadow? (probably not neccesary, since the specular light will be negative)							
							inShadow = list->at(i)->intersectsRay(&ri);
							if (ri.intersection < 1E-5 || ri.intersection > 1) inShadow = false;
							if (ri.color[3]<1) inShadow=false;
							if (inShadow) break;								
						}

						if (!inShadow) list = accelerator->advance(maxT); 
					}
				}

				if (useShadows && inShadow) light=ambient; // drop-shadow strength (only ambient light...)
				if (light < 0) light = 0;

				foundColor[0] = foundColor[0]*light;
				foundColor[1] = foundColor[1]*light;
				foundColor[2] = foundColor[2]*light;

				if (foundColor[3] < 1) {
					Vector3f color = rayCast(iPoint, direction, bestObj, level+1); 
					foundColor[0] = foundColor[0]*(foundColor[3]) + color.x()*(1-foundColor[3]);
					foundColor[1] = foundColor[1]*(foundColor[3]) + color.y()*(1-foundColor[3]);
					foundColor[2] = foundColor[2]*(foundColor[3]) + color.z()*(1-foundColor[3]);
				}

				double reflection = bestObj->getPrimitiveClass()->reflection;

				if (reflection > 0) {
					Vector3f nDir = foundNormal*(-2)*Vector3f::dot(foundNormal, direction)/foundNormal.sqrLength() + direction;

					//Vector3f v = rg.getUniform3D();
					//if (Vector3f::dot(v,nDir)<0) v = -v;
					//nDir = v+nDir;

					Vector3f color = rayCast(iPoint, nDir, bestObj, level+1);
					foundColor[0] = foundColor[0]*(1-reflection) + color.x()*reflection;
					foundColor[1] = foundColor[1]*(1-reflection) + color.y()*reflection;
					foundColor[2] = foundColor[2]*(1-reflection) + color.z()*reflection;

				}

				color =  Vector3f(foundColor[0],foundColor[1],foundColor[2]);
				return color;
			} else {
				color = Vector3f(backgroundColor.x(),backgroundColor.y(),backgroundColor.z());
				return color;
			}
		}

		void RenderThread::setObjects(int count) {
			rayIDs = QVector<int>(count, -1);
		}			

	}
}

