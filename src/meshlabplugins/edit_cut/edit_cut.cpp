#include "edit_cut.h"

#include <wrap/gl/pick.h>
#include <wrap/gl/addons.h>
#include <wrap/qt/device_to_logical.h>
#include <meshlab/glarea.h>
#include <common/GLExtensionsManager.h>
#include <vcg/complex/algorithms/update/selection.h>

#include <QImage>
#include <QPainter>

#include <cstdio>
#include <windows.h>

static const char* LOG_PATH = "C:\\Projects\\cut_debug.log";

static void cutLog(const char *msg)
{
	FILE *fp = fopen(LOG_PATH, "a");
	if (fp) {
		fprintf(fp, "%s\n", msg);
		fflush(fp);
		fclose(fp);
	}
	OutputDebugStringA("[edit_cut] ");
	OutputDebugStringA(msg);
	OutputDebugStringA("\n");
}

static void cutLogQ(const QString &msg)
{
	cutLog(msg.toUtf8().constData());
}

using namespace std;
using namespace vcg;

// ---- SplitTab from vcglib refine.h ----
// Index is bitmask: bit0 = edge01 split, bit1 = edge12 split, bit2 = edge20 split
// Vertices: 0,1,2 = original; 3 = mid01, 4 = mid12, 5 = mid20
struct SplitEntry {
	int TriNum;
	int TV[4][3];
};

static const SplitEntry SplitTab[8] = {
	/* 0 0 0 */ {1, {{0,1,2},{0,0,0},{0,0,0},{0,0,0}} },
	/* 0 0 1 */ {2, {{0,3,2},{3,1,2},{0,0,0},{0,0,0}} },
	/* 0 1 0 */ {2, {{0,1,4},{0,4,2},{0,0,0},{0,0,0}} },
	/* 0 1 1 */ {3, {{3,1,4},{0,3,2},{4,2,3},{0,0,0}} },
	/* 1 0 0 */ {2, {{0,1,5},{5,1,2},{0,0,0},{0,0,0}} },
	/* 1 0 1 */ {3, {{0,3,5},{3,1,5},{2,5,1},{0,0,0}} },
	/* 1 1 0 */ {3, {{2,5,4},{0,1,5},{4,5,1},{0,0,0}} },
	/* 1 1 1 */ {4, {{3,4,5},{0,3,5},{3,1,4},{5,4,2}} },
};

// ---- 2D segment-segment intersection returning lambda along first segment ----
static inline bool SegSegIntersect(
	const Point2f &p0, const Point2f &p1,
	const Point2f &p2, const Point2f &p3,
	float &outLambda0)
{
	float a = (p1 - p0)[0];
	float b = (p2 - p3)[0];
	float c = (p1 - p0)[1];
	float d = (p2 - p3)[1];
	float e = (p2 - p0)[0];
	float f = (p2 - p0)[1];

	float det = a * d - b * c;
	if (fabs(det) < 1e-8f)
		return false;

	float lambda0 = (d * e - b * f) / det;
	float lambda1 = (-c * e + a * f) / det;

	if (lambda0 < 0.0f || lambda0 > 1.0f || lambda1 < 0.0f || lambda1 > 1.0f)
		return false;

	outLambda0 = lambda0;
	return true;
}

// ========================================================================

EditCutPlugin::EditCutPlugin()
{
	memset(viewpSize, 0, sizeof(viewpSize));
	memset(mvMatrix_f, 0, sizeof(mvMatrix_f));
	memset(prMatrix_f, 0, sizeof(prMatrix_f));
	memset(SelViewport, 0, sizeof(SelViewport));
	SelMatrix.setIdentity();
}

const QString EditCutPlugin::info()
{
	return QString("Draw a closed polyline to cut through mesh triangles and delete the inside region.");
}

void EditCutPlugin::suggestedRenderingData(MeshModel & /*m*/, MLRenderingData &dt)
{
	MLPerViewGLOptions opts;
	dt.get(opts);
	opts._sel_enabled = true;
	opts._face_sel = true;
	opts._vertex_sel = true;
	dt.set(opts);
}

bool EditCutPlugin::startEdit(MeshModel & /*m*/, GLArea *gla, MLSceneGLSharedDataContext * /*cont*/)
{
	// Clear log file on tool activation
	FILE *fp = fopen(LOG_PATH, "w");
	if (fp) { fprintf(fp, "=== edit_cut plugin v3 loaded ===\n"); fflush(fp); fclose(fp); }

	if (gla == NULL)
		return false;
	if (!GLExtensionsManager::initializeGLextensions_notThrowing())
		return false;

	gla->setCursor(Qt::CrossCursor);
	cutPolyLine.clear();
	cutLog("startEdit OK");
	return true;
}

void EditCutPlugin::endEdit(MeshModel & /*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext * /*cont*/)
{
	cutPolyLine.clear();
}

// ---- Mouse Events ----

void EditCutPlugin::mousePressEvent(QMouseEvent *event, MeshModel &m, GLArea *gla)
{
	cutPolyLine.push_back(QTLogicalToOpenGL(gla, event->pos()));
	gla->update();
}

void EditCutPlugin::mouseMoveEvent(QMouseEvent *event, MeshModel & /*m*/, GLArea *gla)
{
	if (!cutPolyLine.empty())
		cutPolyLine.back() = QTLogicalToOpenGL(gla, event->pos());
	gla->update();
}

void EditCutPlugin::mouseReleaseEvent(QMouseEvent *event, MeshModel & /*m*/, GLArea *gla)
{
	if (!cutPolyLine.empty())
		cutPolyLine.back() = QTLogicalToOpenGL(gla, event->pos());
}

void EditCutPlugin::keyPressEvent(QKeyEvent *, MeshModel &, GLArea *)
{
}

void EditCutPlugin::keyReleaseEvent(QKeyEvent *e, MeshModel &m, GLArea *gla)
{
	switch (e->key()) {
		case Qt::Key_C: // Clear polyline
			cutLog("Key C - clearing polyline");
			cutPolyLine.clear();
			gla->update();
			e->accept();
			break;

		case Qt::Key_Backspace: // Remove last polyline point
			if (!cutPolyLine.empty()) {
				cutPolyLine.pop_back();
				gla->update();
			}
			e->accept();
			break;

		case Qt::Key_Q: // Cut + add to selection (like edit_select Q)
			if (cutPolyLine.size() >= 3) {
				cutLog("Key Q - cut and add to selection");
				executeCut(m, gla);
			}
			e->accept();
			break;

		case Qt::Key_W: // Subtract from selection (like edit_select W)
			if (cutPolyLine.size() >= 3) {
				cutLog("Key W - subtract from selection");
				// Select faces inside polyline, then CLEAR their selection
				{
					vector<Point3m> projVec;
					GLPickTri<CMeshO>::FillProjectedVector(m.cm, projVec, this->SelMatrix, this->SelViewport);

					QImage bufQImg(viewpSize[2], viewpSize[3], QImage::Format_RGB32);
					bufQImg.fill(Qt::white);
					QPainter bufQPainter(&bufQImg);
					vector<QPointF> qpoints;
					for (size_t i = 0; i < cutPolyLine.size(); ++i)
						qpoints.push_back(QPointF(cutPolyLine[i][0], cutPolyLine[i][1]));
					bufQPainter.setBrush(QBrush(Qt::black));
					bufQPainter.drawPolygon(&qpoints[0], (int)qpoints.size(), Qt::WindingFill);
					bufQPainter.end();
					QRgb blk = QColor(Qt::black).rgb();

					for (size_t fi = 0; fi < m.cm.face.size(); ++fi) {
						if (m.cm.face[fi].IsD() || !m.cm.face[fi].IsS()) continue;
						int vi0 = tri::Index(m.cm, m.cm.face[fi].V(0));
						int vi1 = tri::Index(m.cm, m.cm.face[fi].V(1));
						int vi2 = tri::Index(m.cm, m.cm.face[fi].V(2));
						if (vi0 < 0 || vi0 >= (int)projVec.size() ||
						    vi1 < 0 || vi1 >= (int)projVec.size() ||
						    vi2 < 0 || vi2 >= (int)projVec.size()) continue;
						if (projVec[vi0][2] <= -1.0 || projVec[vi0][2] >= 1.0) continue;
						if (projVec[vi1][2] <= -1.0 || projVec[vi1][2] >= 1.0) continue;
						if (projVec[vi2][2] <= -1.0 || projVec[vi2][2] >= 1.0) continue;
						float cx = (float)(projVec[vi0][0] + projVec[vi1][0] + projVec[vi2][0]) / 3.0f;
						float cy = (float)(projVec[vi0][1] + projVec[vi1][1] + projVec[vi2][1]) / 3.0f;
						int px = (int)cx;
						int py = (int)cy;
						if (px <= 0 || px >= viewpSize[2] || py <= 0 || py >= viewpSize[3]) continue;
						if (bufQImg.pixel(px, py) == blk)
							m.cm.face[fi].ClearS();
					}
					// Polyline stays visible
					gla->updateSelection(m.id(), false, true);
					gla->update();
				}
			}
			e->accept();
			break;

		case Qt::Key_D: // Deselect all (like edit_select D)
			cutLog("Key D - deselect all");
			tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
			gla->updateSelection(m.id(), false, true);
			gla->update();
			e->accept();
			break;

		case Qt::Key_A: // Select all faces
			cutLog("Key A - select all");
			tri::UpdateSelection<CMeshO>::FaceAll(m.cm);
			gla->updateSelection(m.id(), false, true);
			gla->update();
			e->accept();
			break;

		case Qt::Key_I: // Invert selection
			cutLog("Key I - invert selection");
			tri::UpdateSelection<CMeshO>::FaceInvert(m.cm);
			gla->updateSelection(m.id(), false, true);
			gla->update();
			e->accept();
			break;

		case Qt::Key_Escape: // Clear polyline only (D=deselect, Delete=delete selected)
			cutLog("Key ESC - clearing polyline");
			cutPolyLine.clear();
			gla->update();
			e->accept();
			break;

		default:
			break;
	}
}

// ---- Decorate ----

void EditCutPlugin::decorate(MeshModel &m, GLArea *gla)
{
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
	GLPickTri<CMeshO>::glGetMatrixAndViewport(this->SelMatrix, this->SelViewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
	glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
	glGetIntegerv(GL_VIEWPORT, viewpSize);
	glPopMatrix();

	DrawXORPolyLine(gla);

	QString line2;
	if (cutPolyLine.size() < 3)
		line2 = "Need at least 3 points";
	else
		line2 = "<b>Q=cut and select</b>, W=subtract";

	this->realTimeLog("Lasso Cut", m.shortName(),
		"Click to add points — C=clear, BACKSPACE=undo, ESC=cancel<br>"
		"%s<br>"
		"D=deselect all, A=select all, I=invert — <b>DELETE=delete selected</b>",
		line2.toStdString().c_str());
}

// ---- DrawXORPolyLine ----

void EditCutPlugin::DrawXORPolyLine(GLArea *gla)
{
	if (cutPolyLine.empty())
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, QTDeviceWidth(gla), 0, QTDeviceHeight(gla), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1, 1, 1);
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glLineWidth(QTLogicalToDevice(gla, 1));

	if (cutPolyLine.size() == 1) {
		glBegin(GL_POINTS);
		glVertex(cutPolyLine[0]);
	} else if (cutPolyLine.size() == 2) {
		glBegin(GL_LINES);
		glVertex(cutPolyLine[0]);
		glVertex(cutPolyLine[1]);
	} else {
		glBegin(GL_LINE_LOOP);
		for (size_t ii = 0; ii < cutPolyLine.size(); ii++)
			glVertex(cutPolyLine[ii]);
	}
	glEnd();

	glDisable(GL_LOGIC_OP);
	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// ========================================================================
// Phase 1: Cut edges along polyline boundary, then select faces inside
// ========================================================================

void EditCutPlugin::executeCut(MeshModel &m, GLArea *gla)
{
	cutLog(">>> executeCut ENTRY <<<");
	cutLogQ(QString("Viewport: %1 x %2").arg(viewpSize[2]).arg(viewpSize[3]));
	cutLogQ(QString("Polyline points: %1").arg(cutPolyLine.size()));
	cutLogQ(QString("Mesh verts: %1, faces: %2").arg(m.cm.vert.size()).arg(m.cm.face.size()));

	// Log polyline coordinates for debugging
	for (size_t i = 0; i < cutPolyLine.size(); ++i) {
		char buf[128];
		snprintf(buf, sizeof(buf), "  polyline[%d] = (%.1f, %.1f)", (int)i, cutPolyLine[i][0], cutPolyLine[i][1]);
		cutLog(buf);
	}

	if (viewpSize[2] <= 0 || viewpSize[3] <= 0) {
		cutLog("ABORT: viewport invalid");
		return;
	}
	if (cutPolyLine.size() < 3) {
		cutLog("ABORT: polyline too small");
		return;
	}

	try {

	int numPolySeg = (int)cutPolyLine.size();

	// ---- Step 1: Project all vertices to screen space ----
	cutLog("Step 1: Projecting vertices...");
	vector<Point3m> projVec;
	GLPickTri<CMeshO>::FillProjectedVector(m.cm, projVec, this->SelMatrix, this->SelViewport);
	cutLogQ(QString("  Projected %1 vertices").arg(projVec.size()));

	// ---- Step 2: Find which edges are crossed by the polyline ----
	cutLog("Step 2: Finding edge crossings...");
	typedef pair<int,int> EdgeKey;
	map<EdgeKey, pair<float, Point3m>> edgeSplitMap;

	int faceCount = (int)m.cm.face.size();
	int vertCount = (int)m.cm.vert.size();

	for (size_t fi = 0; fi < (size_t)faceCount; ++fi)
	{
		if (m.cm.face[fi].IsD()) continue;

		for (int j = 0; j < 3; ++j)
		{
			int vi0 = tri::Index(m.cm, m.cm.face[fi].V(j));
			int vi1 = tri::Index(m.cm, m.cm.face[fi].V((j + 1) % 3));

			if (vi0 < 0 || vi0 >= vertCount || vi1 < 0 || vi1 >= vertCount)
				continue;

			EdgeKey ek(min(vi0, vi1), max(vi0, vi1));
			if (edgeSplitMap.count(ek)) continue;

			if (projVec[vi0][2] <= -1.0 || projVec[vi0][2] >= 1.0) continue;
			if (projVec[vi1][2] <= -1.0 || projVec[vi1][2] >= 1.0) continue;

			Point2f a((float)projVec[vi0][0], (float)projVec[vi0][1]);
			Point2f b((float)projVec[vi1][0], (float)projVec[vi1][1]);

			for (int k = 0; k < numPolySeg; ++k)
			{
				Point2f c = cutPolyLine[k];
				Point2f d = cutPolyLine[(k + 1) % numPolySeg];

				float lambda0;
				if (SegSegIntersect(a, b, c, d, lambda0))
				{
					if (lambda0 < 0.005f || lambda0 > 0.995f) continue;

					float t = (vi0 < vi1) ? lambda0 : (1.0f - lambda0);

					Point3m p0 = m.cm.vert[ek.first].P();
					Point3m p1 = m.cm.vert[ek.second].P();
					Point3m pos = p0 * (Scalarm)(1.0 - t) + p1 * (Scalarm)t;
					edgeSplitMap[ek] = make_pair(t, pos);
					break;
				}
			}
		}
	}

	cutLogQ(QString("  Found %1 edge crossings").arg(edgeSplitMap.size()));

	// ---- Step 3: Create new split vertices ----
	if (!edgeSplitMap.empty())
	{
		cutLog("Step 3: Creating split vertices...");
		map<EdgeKey, int> edgeToNewVert;

		int numNew = (int)edgeSplitMap.size();
		int firstNewIdx = (int)m.cm.vert.size();
		tri::Allocator<CMeshO>::AddVertices(m.cm, numNew);

		int idx = firstNewIdx;
		for (auto &entry : edgeSplitMap)
		{
			m.cm.vert[idx].P() = entry.second.second;

			float t = entry.second.first;
			Point3m n0 = m.cm.vert[entry.first.first].N();
			Point3m n1 = m.cm.vert[entry.first.second].N();
			Point3m nn = n0 * (Scalarm)(1.0f - t) + n1 * (Scalarm)t;
			Scalarm len = nn.Norm();
			if (len > 0) nn /= len;
			m.cm.vert[idx].N() = nn;

			edgeToNewVert[entry.first] = idx;
			idx++;
		}
		cutLogQ(QString("  Created %1 new vertices").arg(edgeToNewVert.size()));

		// ---- Step 4: Split faces ----
		cutLog("Step 4: Splitting faces...");
		size_t origFaceCount = m.cm.face.size();

		int totalNewFaces = 0;
		for (size_t fi = 0; fi < origFaceCount; ++fi)
		{
			if (m.cm.face[fi].IsD()) continue;

			int mask = 0;
			for (int j = 0; j < 3; ++j)
			{
				int a = tri::Index(m.cm, m.cm.face[fi].V(j));
				int b = tri::Index(m.cm, m.cm.face[fi].V((j + 1) % 3));
				EdgeKey ek(min(a, b), max(a, b));
				if (edgeToNewVert.count(ek)) mask |= (1 << j);
			}
			if (mask > 0)
				totalNewFaces += SplitTab[mask].TriNum - 1;
		}

		cutLogQ(QString("  Need %1 new faces").arg(totalNewFaces));

		if (totalNewFaces > 0)
		{
			int firstNewFace = (int)m.cm.face.size();
			tri::Allocator<CMeshO>::AddFaces(m.cm, totalNewFaces);
			int nextNewFace = firstNewFace;

			for (size_t fi = 0; fi < origFaceCount; ++fi)
			{
				if (m.cm.face[fi].IsD()) continue;

				int vIdx[3];
				vIdx[0] = tri::Index(m.cm, m.cm.face[fi].V(0));
				vIdx[1] = tri::Index(m.cm, m.cm.face[fi].V(1));
				vIdx[2] = tri::Index(m.cm, m.cm.face[fi].V(2));

				int vv[6];
				vv[0] = vIdx[0];
				vv[1] = vIdx[1];
				vv[2] = vIdx[2];
				vv[3] = vv[4] = vv[5] = -1;

				int mask = 0;
				for (int j = 0; j < 3; ++j)
				{
					int a = vIdx[j];
					int b = vIdx[(j + 1) % 3];
					EdgeKey ek(min(a, b), max(a, b));
					auto it = edgeToNewVert.find(ek);
					if (it != edgeToNewVert.end())
					{
						vv[3 + j] = it->second;
						mask |= (1 << j);
					}
				}

				if (mask == 0) continue;

				const SplitEntry &se = SplitTab[mask];

				// Validate all vertex indices
				bool valid = true;
				for (int i = 0; i < se.TriNum && valid; ++i)
					for (int c = 0; c < 3; ++c)
						if (vv[se.TV[i][c]] < 0 || vv[se.TV[i][c]] >= (int)m.cm.vert.size())
							valid = false;
				if (!valid) continue;

				for (int i = 0; i < se.TriNum; ++i)
				{
					CMeshO::FacePointer fp;
					if (i == 0)
						fp = &m.cm.face[fi];
					else {
						fp = &m.cm.face[nextNewFace];
						nextNewFace++;
					}

					fp->V(0) = &m.cm.vert[vv[se.TV[i][0]]];
					fp->V(1) = &m.cm.vert[vv[se.TV[i][1]]];
					fp->V(2) = &m.cm.vert[vv[se.TV[i][2]]];
				}
			}
			cutLogQ(QString("  Split done, used %1 new face slots").arg(nextNewFace - firstNewFace));
		}
	}

	// ---- Step 5: Select faces inside the polyline (preview only, no deletion) ----
	cutLog("Step 5: Selecting inside faces (preview)...");
	selectInsideFaces(m, gla);

	cutLog("=== executeCut DONE ===");

	} catch (std::exception &e) {
		cutLogQ(QString("EXCEPTION: %1").arg(e.what()));
		cutPolyLine.clear();
	} catch (...) {
		cutLog("UNKNOWN EXCEPTION caught");
		cutPolyLine.clear();
	}
}

// ========================================================================
// Select faces whose ANY vertex is inside the polyline
// Uses same approach as edit_select (proven to work)
// ========================================================================

void EditCutPlugin::selectInsideFaces(MeshModel &m, GLArea *gla)
{
	cutLog("  selectInsideFaces START");

	// Re-project all vertices (including newly created split vertices)
	vector<Point3m> projVec;
	GLPickTri<CMeshO>::FillProjectedVector(m.cm, projVec, this->SelMatrix, this->SelViewport);
	cutLogQ(QString("  Projected %1 vertices").arg(projVec.size()));

	// Rasterize polyline as filled polygon (same as edit_select)
	QImage bufQImg(viewpSize[2], viewpSize[3], QImage::Format_RGB32);
	bufQImg.fill(Qt::white);
	QPainter bufQPainter(&bufQImg);
	vector<QPointF> qpoints;
	for (size_t i = 0; i < cutPolyLine.size(); ++i)
		qpoints.push_back(QPointF(cutPolyLine[i][0], cutPolyLine[i][1]));
	bufQPainter.setBrush(QBrush(Qt::black));
	bufQPainter.drawPolygon(&qpoints[0], (int)qpoints.size(), Qt::WindingFill);
	bufQPainter.end();
	QRgb blk = QColor(Qt::black).rgb();

	// Log some pixel checks for debugging
	{
		int cx = (int)cutPolyLine[0][0];
		int cy = (int)cutPolyLine[0][1];
		char buf[256];
		snprintf(buf, sizeof(buf), "  QImage size: %dx%d, first polyline pt pixel(%d,%d)=%s",
			bufQImg.width(), bufQImg.height(), cx, cy,
			(cx >= 0 && cx < bufQImg.width() && cy >= 0 && cy < bufQImg.height())
				? (bufQImg.pixel(cx, cy) == blk ? "BLACK" : "WHITE") : "OOB");
		cutLog(buf);
	}

	// Don't clear existing selection — Q adds to selection (like edit_select)

	// Use face CENTROID for inside test — gives clean cut along the polyline
	// (using "any vertex" would over-select at the boundary where split vertices
	// sit exactly on the polyline, creating a saw-tooth pattern)
	int selectedCount = 0;
	for (size_t fi = 0; fi < m.cm.face.size(); ++fi)
	{
		if (m.cm.face[fi].IsD()) continue;

		int v0 = tri::Index(m.cm, m.cm.face[fi].V(0));
		int v1 = tri::Index(m.cm, m.cm.face[fi].V(1));
		int v2 = tri::Index(m.cm, m.cm.face[fi].V(2));
		if (v0 < 0 || v0 >= (int)projVec.size() ||
		    v1 < 0 || v1 >= (int)projVec.size() ||
		    v2 < 0 || v2 >= (int)projVec.size()) continue;

		// Skip if any vertex is behind camera
		if (projVec[v0][2] <= -1.0 || projVec[v0][2] >= 1.0) continue;
		if (projVec[v1][2] <= -1.0 || projVec[v1][2] >= 1.0) continue;
		if (projVec[v2][2] <= -1.0 || projVec[v2][2] >= 1.0) continue;

		// Average projected 2D positions to get screen-space centroid
		float cx = (float)(projVec[v0][0] + projVec[v1][0] + projVec[v2][0]) / 3.0f;
		float cy = (float)(projVec[v0][1] + projVec[v1][1] + projVec[v2][1]) / 3.0f;

		int px = (int)cx;
		int py = (int)cy;
		if (px <= 0 || px >= viewpSize[2] || py <= 0 || py >= viewpSize[3]) continue;

		if (bufQImg.pixel(px, py) == blk) {
			m.cm.face[fi].SetS();
			selectedCount++;
		}
	}

	cutLogQ(QString("  Selected %1 new faces (added to existing selection)").arg(selectedCount));

	// Polyline stays visible — user can add more points and press Q again

	// Tell MeshLab to update the selection display
	gla->updateSelection(m.id(), false, true);
	gla->update();

	cutLog("  selectInsideFaces DONE");
}

