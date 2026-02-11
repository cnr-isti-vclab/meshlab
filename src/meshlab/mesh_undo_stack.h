#ifndef MESH_UNDO_STACK_H
#define MESH_UNDO_STACK_H

#include <vector>
#include <QString>
#include <common/ml_document/mesh_model.h>
#include <vcg/complex/append.h>

struct UndoEntry {
	int meshId;
	CMeshO mesh;
	QString description;
};

class MeshUndoStack {
public:
	void pushUndo(MeshModel& m, const QString& desc)
	{
		UndoEntry entry;
		entry.meshId = m.id();
		entry.description = desc;
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(entry.mesh, m.cm);
		m_undoStack.push_back(std::move(entry));
		if ((int)m_undoStack.size() > MAX_UNDO)
			m_undoStack.erase(m_undoStack.begin());
		m_redoStack.clear();
	}

	bool canUndo() const { return !m_undoStack.empty(); }
	bool canRedo() const { return !m_redoStack.empty(); }

	// Returns the meshId that was restored, or -1 on failure
	int undo(MeshDocument& doc)
	{
		if (m_undoStack.empty()) return -1;
		UndoEntry& entry = m_undoStack.back();
		MeshModel* mm = doc.getMesh(entry.meshId);
		if (!mm) { m_undoStack.pop_back(); return -1; }

		// Save current state to redo stack
		UndoEntry redoEntry;
		redoEntry.meshId = mm->id();
		redoEntry.description = entry.description;
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(redoEntry.mesh, mm->cm);
		m_redoStack.push_back(std::move(redoEntry));

		// Restore from undo stack
		mm->cm.Clear();
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mm->cm, entry.mesh);

		int restoredId = entry.meshId;
		m_undoStack.pop_back();
		return restoredId;
	}

	int redo(MeshDocument& doc)
	{
		if (m_redoStack.empty()) return -1;
		UndoEntry& entry = m_redoStack.back();
		MeshModel* mm = doc.getMesh(entry.meshId);
		if (!mm) { m_redoStack.pop_back(); return -1; }

		// Save current state to undo stack
		UndoEntry undoEntry;
		undoEntry.meshId = mm->id();
		undoEntry.description = entry.description;
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(undoEntry.mesh, mm->cm);
		m_undoStack.push_back(std::move(undoEntry));

		// Restore from redo stack
		mm->cm.Clear();
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mm->cm, entry.mesh);

		int restoredId = entry.meshId;
		m_redoStack.pop_back();
		return restoredId;
	}

	void clear()
	{
		m_undoStack.clear();
		m_redoStack.clear();
	}

private:
	std::vector<UndoEntry> m_undoStack;
	std::vector<UndoEntry> m_redoStack;
	static const int MAX_UNDO = 10;
};

#endif // MESH_UNDO_STACK_H
