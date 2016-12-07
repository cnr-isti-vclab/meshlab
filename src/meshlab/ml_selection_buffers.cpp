#include "ml_selection_buffers.h"

MLSelectionBuffers::MLSelectionBuffers(MeshModel& m,unsigned int primitivebatch)
	:_lock(),_m(m),_primitivebatch(primitivebatch),_selmap(2)
{

}

MLSelectionBuffers::~MLSelectionBuffers()
{
	QWriteLocker locker(&_lock);

	for (size_t ii = 0; ii < _selmap.size(); ++ii)
	{
		if (_selmap[ii].size() != 0)
		{
			glDeleteBuffers(_selmap[ii].size(), &(_selmap[ii][0]));
			_selmap[ii].clear();
		}
	}
	_selmap.clear();
}

void MLSelectionBuffers::updateBuffer(ML_SELECTION_TYPE selbuf)
{
	QWriteLocker locker(&_lock);

	size_t privchunksize=0;
	if (selbuf == ML_PERVERT_SEL)
	{
		_m.cm.svn = 0;
		privchunksize = std::min(size_t(_m.cm.VN()), size_t(_primitivebatch));
		if (_m.cm.VN() == 0)
			return;
	}

	if (selbuf == ML_PERFACE_SEL)
	{
		_m.cm.sfn = 0;
		privchunksize = std::min(size_t(_m.cm.FN()), size_t(_primitivebatch));
		if (_m.cm.FN() == 0)
			return;
	}

	if (_selmap[selbuf].size() != 0)
	{
		glDeleteBuffers(_selmap[selbuf].size(), &(_selmap[selbuf][0]));
		_selmap[selbuf].clear();
	}
	


	if (selbuf == ML_PERFACE_SEL)
	{
		size_t selectedperchunk = 0;
		std::vector<vcg::Point3f> rpv;
		rpv.resize(privchunksize * 3);

		for (size_t faceind = 0; faceind < _m.cm.FN(); ++faceind)
		{
			CFaceO& ff = _m.cm.face[faceind];
			size_t chunkindex = _m.cm.sfn % privchunksize;
			if (!ff.IsD() && ff.IsS())
			{	
				rpv[chunkindex * 3 + 0].Import(ff.V(0)->P());
				rpv[chunkindex * 3 + 1].Import(ff.V(1)->P());
				rpv[chunkindex * 3 + 2].Import(ff.V(2)->P());
				
				++_m.cm.sfn;
				++selectedperchunk;
			}

			if (((faceind == _m.cm.FN() - 1) && (selectedperchunk > 0)) || ((chunkindex == privchunksize - 1) && (selectedperchunk == privchunksize)))
			{
				_selmap[ML_PERFACE_SEL].push_back(0);
				glGenBuffers(1, &(_selmap[ML_PERFACE_SEL][_selmap[ML_PERFACE_SEL].size() - 1]));

				glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERFACE_SEL][_selmap[ML_PERFACE_SEL].size() - 1]);
				glBufferData(GL_ARRAY_BUFFER,3 * 3 * 4 * selectedperchunk, &rpv[0],GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				selectedperchunk = 0;
			}
		}

		rpv.clear();
	}

	if (selbuf == ML_PERVERT_SEL)
	{
		size_t selectedperchunk = 0;
		std::vector<vcg::Point3f> rpv;
		rpv.resize(privchunksize);
		_m.cm.svn = 0;

		for (size_t vertind = 0; vertind < _m.cm.VN(); ++vertind)
		{
			CVertexO& vv = _m.cm.vert[vertind];
			size_t chunkindex = _m.cm.svn % privchunksize;
			if (!vv.IsD() && vv.IsS())
			{
				rpv[chunkindex].Import(vv.cP());
				++_m.cm.svn;
				++selectedperchunk;
			}
			if (((vertind == _m.cm.VN() - 1) && (selectedperchunk > 0)) || ((chunkindex == privchunksize - 1) && (selectedperchunk == privchunksize)))
			{
				_selmap[ML_PERVERT_SEL].push_back(0);

				glGenBuffers(1, &(_selmap[ML_PERVERT_SEL][_selmap[ML_PERVERT_SEL].size() - 1]));

				glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERVERT_SEL][_selmap[selbuf].size() - 1]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vcg::Point3f) * selectedperchunk, &(rpv[0]), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				selectedperchunk = 0;
			}
		}

		rpv.clear();
	}
}

void MLSelectionBuffers::drawSelection(ML_SELECTION_TYPE selbuf) const
{
	QReadLocker locker(&_lock);

	if ((selbuf == ML_PERVERT_SEL) && (_m.cm.svn != 0))
	{
		const size_t privchunksize = std::min(size_t(_m.cm.VN()), size_t(_primitivebatch));
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 0.0, 0.0, .3f);
		glDepthRange(0.00, 0.999);
		glPointSize(3.0);
		glPushMatrix();
		glMultMatrix(_m.cm.Tr);

		if (_pointsize > 0.0f)
			glPointSize((GLfloat)_pointsize);
		for (size_t ii = 0; ii < _selmap[ML_PERVERT_SEL].size(); ++ii)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERVERT_SEL][ii]);
			glVertexPointer(3, GL_FLOAT, GLsizei(0), 0);
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			unsigned int todraw = privchunksize;
			if (ii == (_selmap[ML_PERVERT_SEL].size() - 1))
				todraw = _m.cm.svn -  (privchunksize * ii); 

			//glGetBufferSubData(GL_ARRAY_BUFFER, 0, rp.size() * sizeof(vcg::Point3f), &(rp[0]));
			glDrawArrays(GL_POINTS, 0, todraw);

			glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERVERT_SEL][ii]);
			glDisableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glPopMatrix();
		glPopAttrib();
	}


	if ((selbuf == ML_PERFACE_SEL) && (_m.cm.sfn != 0))
	{
		const size_t privchunksize = std::min(size_t(_m.cm.FN()), size_t(_primitivebatch));

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 0.0, 0.0, .3f);
		glPolygonOffset(-1.0, -1);
		glPushMatrix();
		glMultMatrix(_m.cm.Tr);


		for (size_t ii = 0; ii < _selmap[ML_PERFACE_SEL].size(); ++ii)
		{
			glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERFACE_SEL][ii]);
			glVertexPointer(3, GL_FLOAT, GLsizei(0), 0);
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			unsigned int todraw = privchunksize;
			if (ii == _selmap[ML_PERFACE_SEL].size() - 1)
				todraw = _m.cm.sfn - (privchunksize * ii);
			glDrawArrays(GL_TRIANGLES, 0, 3 * todraw);
			glBindBuffer(GL_ARRAY_BUFFER, _selmap[ML_PERFACE_SEL][ii]);
			glDisableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}
		
		glPopMatrix();
		glPopAttrib();
	}
}

void MLSelectionBuffers::deallocateBuffer(ML_SELECTION_TYPE selbuf)
{
	if (_selmap[selbuf].size() != 0)
	{
		glDeleteBuffers(_selmap[selbuf].size(), &(_selmap[selbuf][0]));
		_selmap[selbuf].clear();
	}
}

void MLSelectionBuffers::setPointSize(float ptsz)
{
	_pointsize = ptsz;
}
