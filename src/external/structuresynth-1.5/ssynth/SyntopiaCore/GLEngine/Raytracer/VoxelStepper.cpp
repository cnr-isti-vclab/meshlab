#include "VoxelStepper.h"



using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {

		VoxelStepper::VoxelStepper(Vector3f minPos, Vector3f maxPos, int steps) 
				:  steps(steps), minPos(minPos), maxPos(maxPos), size(( maxPos - minPos)/(double)steps) {
					currentT = 0;
					copy = false;
					grid = new QList<Object3D*>[steps*steps*steps];
					for (int i = 0; i < steps*steps*steps; i++) grid[i] = QList<Object3D*>();
			};

		VoxelStepper::~VoxelStepper() { if (!copy) delete[] grid; };

		void VoxelStepper::registerObject(Object3D* obj) {
				// Simple method - check all cells intersecting the objects bounding boxes.
				obj->prepareForRaytracing();
				Vector3f from;
				Vector3f to;
				obj->getBoundingBox(from,to);
				from = from - minPos;
				to = to - minPos;

				int xStart = floor(from.x()/size.x());
				int xEnd = ceil(to.x()/size.x());
				int yStart = floor(from.y()/size.y());
				int yEnd = ceil(to.y()/size.y());
				int zStart = floor(from.z()/size.z());
				int zEnd = ceil(to.z()/size.z());
				if (xStart < 0) xStart = 0;
				if (yStart < 0) yStart = 0;
				if (zStart < 0) zStart = 0;
				if (xEnd > (int)steps) xEnd = steps;
				if (yEnd > (int)steps) yEnd = steps;
				if (zEnd > (int)steps) zEnd = steps;

				for (unsigned int x = xStart; x < (unsigned int)xEnd; x++) {
					for (unsigned int y = yStart; y < (unsigned int)yEnd; y++) {
						for (unsigned int z = zStart; z < (unsigned int)zEnd; z++) {
							if (obj->intersectsAABB(minPos + Vector3f(size.x()*x,size.y()*y,size.z()*z),minPos + Vector3f(size.x()*(x+1),size.y()*(y+1),size.z()*(z+1)) )) {
								grid[x+y*steps+z*steps*steps].append(obj);
							}
						}
					}
				}

			}

		QList<Object3D*>* VoxelStepper::setupRay(Vector3f pos, Vector3f dir, double& maxT) {
				this->pos = pos; 
				this->dir = dir;

				currentT = 0;

				const Vector3f ro = pos - minPos;
				cx = floor(ro.x() / size.x());
				cy = floor(ro.y() / size.y());
				cz = floor(ro.z() / size.z());


				if ((cx < 0 || cx >= steps) ||
					(cy < 0 || cy >= steps) || 
					(cz < 0 || cz >= steps)) {						
						// we are outside grid.
						// advance ray to inside grid.
						bool found = false;
						double p;
						if (dir.x() > 0) {
							p = (minPos.x()-pos.x())/dir.x();
							cx = 0;
						} else {
							p = (maxPos.x()-pos.x())/dir.x();
							cx = steps-1;								
						}
						Vector3f v = pos + dir*p - minPos;								
						cy = floor(v.y() / size.y());
						cz = floor(v.z() / size.z());
						if ((cy >= 0 && cy < steps) && (cz >= 0 && cz < steps)) {
							found = true;
							pos = v+minPos;
						}

						if (!found) {
							if (dir.y() > 0) {
								p = (minPos.y()-pos.y())/dir.y();
								cy = 0;
							} else {
								p = (maxPos.y()-pos.y())/dir.y();
								cy = steps-1;
							}
							Vector3f v = pos + dir*p - minPos;									
							cx = floor(v.x() / size.x());
							cz = floor(v.z() / size.z());
							if ((cx >= 0 && cx < steps) && (cz >= 0 && cz < steps)) {
								pos = v+minPos;
								found = true;
							}
						}

						if (!found) {
							if (dir.z() > 0) {
								p = (minPos.z()-pos.z())/dir.z();
								cz = 0;
							} else {
								p = (maxPos.z()-pos.z())/dir.z();
								cz = steps-1;
							}
							Vector3f v = pos + dir*p - minPos;									
							cx = floor(v.x() / size.x());
							cy = floor(v.y() / size.y());
							if ((cy >= 0 && cy < steps) && (cx >= 0 && cx < steps)) {
								pos = v+minPos;
								found = true;
							}
						}

						currentT = p;

						// We do not intersect grid.
						if (!found) return false;
				}

				stepX = (dir.x() > 0) ? 1 : -1;
				stepY = (dir.y() > 0) ? 1 : -1;
				stepZ = (dir.z() > 0) ? 1 : -1;

				tDeltaX = stepX*size.x() / dir.x();
				tDeltaY = stepY*size.y() / dir.y();
				tDeltaZ = stepZ*size.z() / dir.z();

				Vector3f orv = pos- (minPos + Vector3f(size.x()*cx, size.y()*cy, size.z()*cz));
				tMaxX = stepX*orv.x()/dir.x();
				if (stepX>0) tMaxX = tDeltaX - tMaxX;
				tMaxY = stepY*orv.y()/dir.y();
				if (stepY>0) tMaxY = tDeltaY - tMaxY;
				tMaxZ = stepZ*orv.z()/dir.z();
				if (stepZ>0) tMaxZ = tDeltaZ - tMaxZ;

				// Now pos is advanced properly.
				// cx,cy,cz contains current cell.
				QList<Object3D*>* list = &grid[cx+cy*steps+cz*steps*steps];

				if (list && (list->count() == 0)) {
					list = advance(maxT);
				} else {
					maxT = currentT + minn(tMaxX, tMaxY, tMaxZ);					
				}

				return list;
			}

		

			QList<Object3D*>* VoxelStepper::advance(double& maxT) {
				QList<Object3D*>* list = 0;
				do {
					if(tMaxX < tMaxY) {
						if(tMaxX < tMaxZ) {
							cx += stepX;
							if (cx >= steps || cx < 0) return 0;
							tMaxX = tMaxX + tDeltaX;
						} else {
							cz += stepZ;
							if (cz >= steps || cz < 0) return 0;
							tMaxZ = tMaxZ + tDeltaZ;
						} 
					} else {
						if(tMaxY < tMaxZ) {
							cy += stepY;
							if (cy >= steps || cy < 0) return 0;
							tMaxY = tMaxY + tDeltaY;
						} else {
							cz += stepZ;
							if (cz >= steps || cz < 0) return 0;
							tMaxZ = tMaxZ + tDeltaZ;
						}
					}
					list = &grid[cx+cy*steps+cz*steps*steps];

					if (list && (list->count() == 0)) list = 0; // Continue until we find an non-empty list.
				} while(list == 0);

				maxT = currentT + minn(tMaxX, tMaxY, tMaxZ);
				return(list);
			}
	}
}

