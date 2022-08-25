#include "EngineWidget.h"
#include "../Math/Vector3.h"
#include "../Math/Random.h"
#include "../Logging/Logging.h"

#include "Sphere.h"

using namespace SyntopiaCore::Math;
using namespace SyntopiaCore::Logging;

#include <QWheelEvent>

namespace SyntopiaCore {
	namespace GLEngine {


		EngineWidget::EngineWidget(QMainWindow* mainWindow, QWidget* parent) 
			: QGLWidget(parent), mainWindow(mainWindow) 
		{
			showDepth = false;
			disabled = false;
			updatePerspective();
			showCoordinateSystem = false;

			pendingRedraws = 0;
			requiredRedraws = 2; // for double buffering
			startTimer( 10 );
			oldPos = QPoint(0,0);

			// Engine settings
			scale = 0.4;
			minimumScale = 0.01;
			mouseSpeed = 1.0;
			mouseTranslationSpeed = 64.0;
			translation = Vector3f(0,0,-20);

			setMouseTracking(true);

			rotation = Matrix4f::Identity();
			pivot = Vector3f(0,0,0);
			backgroundColor = QColor(30,30,30);
			contextMenu = 0;

			rmbDragging = false;

			fastRotate = false;
			doingRotate = false;
			shaderProgram = 0;
			//setupFragmentShader();
		}

		QImage EngineWidget::getScreenShot() {
			if (!staticImage.isNull()) return staticImage;
			return grabFrameBuffer();
		}

		void EngineWidget::paintEvent(QPaintEvent * ev) {
			if (staticImage.isNull()) {
				QGLWidget::paintEvent(ev);
			} else {
				setAutoFillBackground(false);
				glDisable( GL_CULL_FACE );
				glDisable( GL_LIGHTING );
				glDisable( GL_DEPTH_TEST );
				glViewport(0,0,width(),height());
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(0,100,0,100);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				
				QPainter p;
				p.begin(this);
				p.drawImage(QPoint(0,0), staticImage);
				p.end();
				ev->accept();
				pendingRedraws = 0;
				
			}
		}


		void EngineWidget::setFastRotate(bool enabled) {
			fastRotate = enabled;
		}

		void EngineWidget::setImage(QImage im) {
			staticImage = im; 
			if (im.isNull()) {
				glEnable( GL_CULL_FACE );
				glEnable( GL_LIGHTING );
				glEnable( GL_DEPTH_TEST );
				updatePerspective();
				initializeGL();
			} else {
				update();
			}
			
		} 

		EngineWidget::~EngineWidget() {
		}

		void EngineWidget::mouseReleaseEvent(QMouseEvent* ev)  {
			doingRotate = false;

			if (ev->button() != Qt::RightButton) return;
			if (rmbDragging) { return; }
			if (contextMenu) contextMenu->exec(ev->globalPos());
		}



		void EngineWidget::contextMenuEvent(QContextMenuEvent* /*ev*/ ) {
			// Implementing this here gives trouble on Linux...
			// if (rmbDragging) { return; }
			// if (contextMenu) contextMenu->exec(ev->globalPos());	
		}


		void EngineWidget::reset() {
			translation = Vector3f(0,0,-20);
			rotation = Matrix4f::Identity();
			pivot = Vector3f(0,0,0);
			scale = 0.4f;
			settings = Settings();
			updatePerspective();
			//backgroundColor = QColor(0,0,0);
			requireRedraw();

			if (shaderProgram) {
				shaderProgram->release();
				setupFragmentShader();
			}
		}

		void EngineWidget::requireRedraw() {
			if (!staticImage.isNull()) setImage(QImage());
			pendingRedraws = requiredRedraws;
		}

		void vertexm(SyntopiaCore::Math::Vector3f v) { glVertex3f(v.x(), v.y(), v.z()); }

		void EngineWidget::setupFragmentShader() {
			// As of now, these are not used...

			shaderProgram = new QGLShaderProgram(this);
			bool s = shaderProgram->addShaderFromSourceCode(QGLShader::Vertex,
				"varying vec2 coord;\n"
				"void main(void)\n"
				"{\n"
				"   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;;\n"
				"   coord = vec2(gl_Position.x,gl_Position.y);\n "
				"}");
			if (!s) WARNING("Could not create vertex shader: " + shaderProgram->log());
			s = shaderProgram->addShaderFromSourceCode(QGLShader::Fragment,
				"uniform mediump vec4 color;\n"
				"varying vec2 coord;\n"
				"void main(void)\n"
				"{\n"
				"   gl_FragColor =  vec4(1.0,coord.x,coord.y, 1.0);\n"
				"    if (coord.y<0.5)  gl_FragColor = vec4(1.0,1.0,1.0, 1.0); "
				"}");
			if (!s) WARNING("Could not create vertex shader: " + shaderProgram->log());

			s = shaderProgram->link();
			if (!s) WARNING("Could not link shaders: " + shaderProgram->log());

			s = shaderProgram->bind();
			if (!s) WARNING("Could not bind shaders: " + shaderProgram->log());

			INFO("Shader setup complete!");
		}

		double EngineWidget::getDepthAt(int x,int y) {
			RayInfo ri;
			Vector3f front = screenTo3D(x, y, 0);
			Vector3f back = screenTo3D(x, y, 1);
			ri.startPoint = front;
			ri.lineDirection = back - front;

			double dist = -1;
			Object3D* obj = 0;
			for (int i = 0; i < objects.count(); i++) {
				if (objects[i]->intersectsRay(&ri)) {
					if (ri.intersection<dist || dist ==-1) {
						dist = ri.intersection;
						obj = objects[i];
					}
				}
			}
			if (obj) {
				float d = -Vector3f::dot(obj->getCenter()-cameraPosition,
					(cameraTarget-cameraPosition).normalized());


				QString s = (QString("Object: %0 (%1,%2,%3). Viewport depth: %4. Camera plane depth: %5")
					.arg(obj->name()).arg(obj->getCenter().x()).arg(obj->getCenter().y()).arg(obj->getCenter().z())
					.arg(dist).arg(d));
				mainWindow->statusBar()->showMessage(s);
			} else {
				mainWindow->statusBar()->showMessage("");
			}
			return dist;
		}

		namespace {
			bool depthSorter( const Object3D *a, const Object3D *b )
			{
				return a->getDepth() < b->getDepth();
			}
		}


		void EngineWidget::paintGL() {
			if (!staticImage.isNull()) {
				return;
			}
			
			static int count = 0;
			count++;

			if (pendingRedraws > 0) pendingRedraws--;

			if (disabled) {
				qglClearColor(backgroundColor);
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				return;
			}

			// Experimental shader stuff (not enabled)
			if (shaderProgram) {
				glDisable( GL_CULL_FACE );
				glDisable( GL_LIGHTING );
				glDisable( GL_DEPTH_TEST );
				glViewport(0,0,width(),height());
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(0,100,0,100);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				int colorLocation = shaderProgram->uniformLocation("color");
				QColor color(0, 255, 0, 255);
				shaderProgram->setUniformValue(colorLocation, color);
				glColor3d(1.0,1.0,0.0);
				glRectf(2,2,98,98); 
				INFO("Drawing...");
				return;
			}


			qglClearColor(backgroundColor);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// Calc camera position
			glTranslatef( translation.x(), translation.y(), translation.z() );
			glScalef( scale, scale, scale );
			Vector3f v(1,2,3);
			Vector3f v2 = rotation*v;
			glMultMatrixf(rotation.getArray());
			glTranslatef( -pivot.x(), -pivot.y(), -pivot.z() );

			glGetDoublev(GL_MODELVIEW_MATRIX, modelViewCache );
			glGetDoublev(GL_PROJECTION_MATRIX, projectionCache );
			glGetIntegerv(GL_VIEWPORT, viewPortCache);

			cameraPosition = screenTo3D(width()/2, height()/2, 0);
			cameraTarget = screenTo3D(width()/2, height()/2, 1);
			cameraUp = screenTo3D(width()/2, height()/2-height()/4, 0)-cameraPosition;
			cameraUp.normalize();

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			qglColor(getVisibleForegroundColor());

			
			if (showCoordinateSystem) {

				glPushMatrix();
			
				GLUquadric* g = gluNewQuadric();    
				gluQuadricDrawStyle(g, GLU_FILL);

				const float red[4] = { 1,0,0,1 };
				const float green[4] = { 0,1,0,1 };
				const float blue[4] = { 0,0,1,1 };
				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
				gluCylinder(g, 0.1,0.1,9,15,1); // z
				glTranslatef(0,0,9);
				gluCylinder(g, 0.3,0,1,15,1); // z-arrow
				glTranslatef(0,0,-9);
			
				glRotatef(90,0,1,0);
				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
				gluCylinder(g, 0.1,0.1,9,15,1); // z
				glTranslatef(0,0,9);
				gluCylinder(g, 0.3,0,1,15,1); // z-arrow
				glTranslatef(0,0,-9);
			
				glRotatef(-90,1,0,0);
				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
				gluCylinder(g, 0.1,0.1,9,15,1); // z
				glTranslatef(0,0,9);
				gluCylinder(g, 0.3,0,1,15,1); // z-arrow
				glTranslatef(0,0,-9);
			

				

				glPopMatrix();
			

			}

			if (QApplication::keyboardModifiers() == Qt::AltModifier || (doingRotate && fastRotate && ( objects.size()>1000))) {
				
				
				/*
				glDisable (GL_LIGHTING);
				glPointSize(3);
				glColor4f(1,1,1,1);
				glBegin(GL_POINTS);
				double stepX = 1.0/160.0;
				Math::RandomNumberGenerator rg;
				static int seeder = 0;
				//rg.setSeed(seeder++);
				for (double uz1 = 0; uz1 <=1; uz1+=stepX)
				{
					for (double uz2 = 0; uz2 <=1; uz2+=stepX)
					{
						double u1 = rg.getDouble(0,1);
						double u2 = rg.getDouble(0,1);
						u1 = uz1;
						u2 = uz2;
						double z = 1.0 - 2.0*u1;
						double r = r = sqrt(1.0-z*z);
						double phi = 2.0 * 3.1415926 * u2;
						double x = r * cos(phi);
						double y = r * sin(phi);
						//glVertex3d(x,y,z);
						glVertex3d(cos(2*3.1415*u1)*sqrt(u2),sin(2*3.1415*u1)*sqrt(u2),0);
					}
				}
				glEnd();
				glEnable (GL_LIGHTING);

				*/
				

				// Fast-draw
				int objs =  objects.size();
				int step = objs/5000;
				if (step < 1) step = 1;
				if (count >= step) count = 0;

				qglColor(getVisibleForegroundColor());
				for (int i = count; i < objects.size(); i+=step) {
					objects[i]->draw();
				}

				glEnable (GL_LIGHTING);
				requireRedraw();
			} else {

				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
				glEnable (GL_LIGHTING);
				glEnable(GL_CULL_FACE); 


				glMateriali( GL_FRONT, GL_SPECULAR, 30 );
				glMateriali(GL_FRONT, GL_SHININESS, 127);

				glDisable (GL_BLEND);

				QVector<Object3D*> transparentObjects;
				for (int i = 0; i < objects.size(); i++) {
					if (objects[i]->getColor()[3]==1.0) {
						objects[i]->draw();
					} else {
						transparentObjects.append(objects[i]);
						float d = -Vector3f::dot(objects[i]->getCenter()-cameraPosition,
							cameraTarget-cameraPosition);
						objects[i]->setDepth(d);
					}
				}

				glEnable (GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				//glDisable(GL_DEPTH_TEST); 
				//glEnable(GL_CULL_FACE); 
				qSort(transparentObjects.begin(), transparentObjects.end(), depthSorter);
				for (int i = 0; i < transparentObjects.size(); i++) {
					transparentObjects[i]->draw();
				}
			}
			renderText(10, 20, infoText);


		};

		void EngineWidget::resizeGL( int /* width */, int /* height */) {
			// When resizing the perspective must be recalculated
			requireRedraw();
			updatePerspective();
		};

		void EngineWidget::updatePerspective() {
			if (!staticImage.isNull()) return;
			if (height() == 0) return;

			GLfloat w = (float) width() / (float) height();
			infoText = QString("[%1x%2] Aspect=%3").arg(width()).arg(height()).arg((float)width()/height());
			textTimer = QTime::currentTime();
			//GLfloat h = 1.0;

			glViewport( 0, 0, width(), height() );
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			//settings.perspectiveAngle = 90;
			gluPerspective(settings.perspectiveAngle, w,  (float)settings.nearClipping, (float)settings.farClipping);
			//glOrtho( -w, w, -h, h, (float)0, (float) 60 );
		}

		void EngineWidget::timerEvent(QTimerEvent*) {
			static bool firstTime = true;
			if (firstTime) {
				firstTime = false;
				updatePerspective(); 
				requireRedraw();
				infoText = "";
			}

			// Check if we are displaying a message.
			if (infoText != "" && abs((int)(textTimer.msecsTo(QTime::currentTime()))>1000)) {
				infoText = "";
				requireRedraw();
			}

			if (pendingRedraws) updateGL();
		}


		void EngineWidget::initializeGL()
		{
			if (!staticImage.isNull()) return;
			requireRedraw();

			GLfloat pos[4] = {-55.0f, -25.0f, 20.0f, 1.0f };
			//GLfloat pos2[4] = {52.0f, 25.0f, 50.0f, 1.0f };
			GLfloat agreen[4] = {0.0f, 0.8f, 0.2f, 1.0f };

			float ambient = 0.4f;
			float diffuse = 0.6f;
			float specular = 0.1f;
			Vector3f color = Vector3f(1,1,1);
			GLfloat ambientLight[] = { ambient * color.x(),   ambient * color.y(),  ambient * color.z(), 1.0f };
			GLfloat diffuseLight[] = { diffuse * color.x(),   diffuse * color.y(),  diffuse * color.z(), 1.0f };
			GLfloat specularLight[] = {specular * color.x(),  specular * color.y(), specular* color.z(), 1.0f };
			glLightfv(0, GL_AMBIENT, ambientLight);
			glLightfv(0, GL_DIFFUSE, diffuseLight);
			glLightfv(0, GL_SPECULAR, specularLight);
			glLightfv(0, GL_POSITION, pos );
			glEnable( GL_LIGHT0 ); 



			glEnable( GL_CULL_FACE );
			glEnable( GL_LIGHTING );
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_NORMALIZE );
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, agreen );

			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		}


		void EngineWidget::wheelEvent(QWheelEvent* e) {
			e->accept();

			double interval = (double)e->delta() / 800.0;

			if ( scale <= mouseSpeed*interval ) return;
			scale -= mouseSpeed*interval;
			requireRedraw();
		}

		void EngineWidget::mouseMoveEvent( QMouseEvent *e ) {
			e->accept();

			if (showDepth) getDepthAt(e->pos().x(),e->pos().y());

			// store old position

			if (oldPos.x() == 0 && oldPos.y() == 0) {
				// first time
				oldPos = e->pos(); 
				return;
			}
			double dx = e->x() - oldPos.x();
			double dy = e->y() - oldPos.y();

			// normalize wrt screen size
			double rx = dx / width();
			double ry = dy / height();

			oldPos = e->pos();

			if ( (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier ) 
				|| e->buttons() == (Qt::LeftButton | Qt::RightButton )) 
			{
				doingRotate = true;

				// 1) dragging with left mouse button + shift down, or
				// 2) dragging with left and right mouse button down
				//
				// This results in zooming for vertical movement, and Z axis rotation for horizontal movement.

				scale += (ry * 4.0f);

				if (scale < minimumScale) scale = minimumScale;

				rotateWorldZ( rx );
				requireRedraw();

				if (e->buttons() == (Qt::LeftButton | Qt::RightButton ))  {
					rmbDragging = true;
				}
			} else if ( ( e->buttons() == Qt::RightButton ) 
				|| (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier ) 
				|| (e->buttons() == Qt::LeftButton && e->modifiers() == Qt::MetaModifier ) ) 
			{ 
				doingRotate = true;

				// 1) dragging with right mouse button, 
				// 2) dragging with left button + control button,
				// 3) dragging with left button + meta button (Mac)
				//
				// results in translation

				if (rx != 0 || ry != 0) {
					translateWorld( mouseSpeed*mouseTranslationSpeed* rx, - mouseSpeed*mouseTranslationSpeed*ry, 0 );
					requireRedraw();
					rmbDragging = true;
				} 
			} else if ( e->buttons() == Qt::LeftButton ) {
				doingRotate = true;

				// Dragging with left mouse button.
				// Results in rotation.

				rotateWorldXY( rx, ry );
				requireRedraw();

				rmbDragging = false;
			} else {
				rmbDragging = false;
			}
		}


		Vector3f EngineWidget::screenTo3D(int sx, int sy, int sz) {
			// 2D -> 3D conversion
			/*
			GLdouble modelView[16];
			GLdouble projection[16];
			GLint viewPort[16];

			glGetDoublev( GL_MODELVIEW_MATRIX, modelView );
			glGetDoublev( GL_PROJECTION_MATRIX, projection );
			glGetIntegerv( GL_VIEWPORT, viewPort);
			*/

			GLdouble x, y, z;
			float h = (float)height(); 
			gluUnProject(sx, h-sy, sz, modelViewCache, projectionCache, viewPortCache, &x, &y ,&z);
			return Vector3f(x,y,z);
		}

		void EngineWidget::rotateWorldXY(double x, double y) {
			double rotateSpeed = 5.0;

			Vector3f startPoint = screenTo3D(oldPos.x(), oldPos.y(),1);
			Vector3f xDir =       (screenTo3D(oldPos.x()+10, oldPos.y(),1) - startPoint).normalized() ;
			Vector3f yDir =       (screenTo3D(oldPos.x(), oldPos.y()+10,1) - startPoint).normalized() ;

			Matrix4f mx = Matrix4f::Rotation(xDir, y*rotateSpeed);
			Matrix4f my = Matrix4f::Rotation(yDir, -x*rotateSpeed);
			rotation = rotation*my * mx   ; 
			requireRedraw();
		}

		void EngineWidget::rotateWorldZ(double z) {
			double rotateSpeed = 5.0;

			Vector3f startPoint = screenTo3D(oldPos.x(), oldPos.y(), 0);
			Vector3f endPoint =   screenTo3D(oldPos.x(), oldPos.y(), 1);

			Matrix4f mz = Matrix4f::Rotation(startPoint-endPoint, z*rotateSpeed);
			rotation = rotation*mz  ; 
			requireRedraw();
		}

		void EngineWidget::translateWorld(double x, double y, double z) {
			translation = Vector3f(translation.x() + x, translation.y() + y, translation.z() + z);
		}


		void EngineWidget::clearWorld() {
			for (int i = 0; i < objects.size(); i++) delete(objects[i]);
			objects.clear();
		}

		void EngineWidget::addObject(Object3D* object) {
			objects.append(object);
		}

		SyntopiaCore::Math::Vector3f EngineWidget::getCameraPosition() {
			return cameraPosition;
		};

		SyntopiaCore::Math::Vector3f EngineWidget::getCameraUp() {
			return cameraUp;
		};

		SyntopiaCore::Math::Vector3f EngineWidget::getCameraTarget() {
			return cameraTarget;
		};

		double EngineWidget::getFOV() {
			double ar = width()/(double)height();
			return 29.0*ar; // Hack - this is not entirely accurate for large AR's.
		}

		QColor EngineWidget::getVisibleForegroundColor() {
			int r = backgroundColor.red() < 127 ? 255 : 0;
			int g = backgroundColor.green() < 127 ? 255 : 0;
			int b = backgroundColor.blue() < 127 ? 255 : 0;
			return QColor(r,g,b);
		}

		void EngineWidget::getBoundingBox(SyntopiaCore::Math::Vector3f& from, SyntopiaCore::Math::Vector3f& to) const {
			from = Vector3f(0,0,0);
			to = Vector3f(0,0,0);
			for (int i = 0; i < objects.count(); i++) {
				if (i == 0) { objects[i]->getBoundingBox(from,to); }
				else { objects[i]->expandBoundingBox(from,to);  }
			}
		}

	}
}

