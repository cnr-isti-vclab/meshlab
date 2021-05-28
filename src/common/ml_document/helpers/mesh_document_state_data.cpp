#include "mesh_document_state_data.h"

#include "../mesh_document.h"

MeshDocumentStateData::MeshDocumentStateData() :
	_lock(QReadWriteLock::Recursive)
{
}

MeshDocumentStateData::~MeshDocumentStateData()
{
	QWriteLocker locker(&_lock);
	_existingmeshesbeforeoperation.clear();
}

void MeshDocumentStateData::create(MeshDocument& md)
{
	QWriteLocker locker(&_lock);
	for (MeshModel* mm :md.meshList) {
		if (mm != NULL)
			insert(mm->id(), MeshModelStateData(mm->dataMask(), mm->cm.VN(), mm->cm.FN(), mm->cm.EN()));
	}
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::insert(const int key, const MeshModelStateData & value)
{
	QWriteLocker locker(&_lock);
	return _existingmeshesbeforeoperation.insert(key,value);
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::find(const int key)
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.find(key);
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::begin()
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.begin();
}

QMap<int, MeshModelStateData>::iterator MeshDocumentStateData::end()
{
	QReadLocker locker(&_lock);
	return _existingmeshesbeforeoperation.end();
}

void MeshDocumentStateData::clear()
{
	QWriteLocker locker(&_lock);
	_existingmeshesbeforeoperation.clear();
}
