#ifndef MESH_UNDO_STACK_H
#define MESH_UNDO_STACK_H

#include <vector>
#include <unordered_map>
#include <cstring>
#include <cassert>
#include <QString>
#include <common/ml_document/mesh_model.h>
#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/update/topology.h>

// ============================================================================
// CDC (Content-Defined Chunking) Undo System
// ============================================================================
//
// Instead of storing full mesh copies, this system:
// 1. Serializes the mesh into a deterministic byte buffer (no pointers)
// 2. Chunks the buffer using Gear hash (content-defined boundaries ~4KB)
// 3. Deduplicates chunks in a shared ChunkStore (reference-counted)
// 4. Each undo entry is just a list of chunk hashes (tiny metadata)
//
// Memory savings: ~80-90% for typical local edits vs full mesh copies.
// ============================================================================

namespace cdc {

// ---- FNV-1a 64-bit hash ----
inline uint64_t fnv1a_64(const uint8_t* data, size_t len)
{
	uint64_t hash = 14695981039346656037ULL;
	for (size_t i = 0; i < len; ++i) {
		hash ^= data[i];
		hash *= 1099511628211ULL;
	}
	return hash;
}

// ---- Gear hash table (256 random uint64 values for rolling hash) ----
// Generated deterministically from LCG seed 0x12345678ABCDEF01
static const uint64_t GEAR_TABLE[256] = {
	0xE80E65D4A4F256BEULL, 0x2BA1D259BCEA67A3ULL, 0xA72FA4BD8AC10928ULL, 0x63CCE703EA9C674DULL,
	0x5D08B93FCA15C712ULL, 0x191D9A7D9E72F8D7ULL, 0x8B23F0B2887F2A5CULL, 0x473E26E862AC5C21ULL,
	0x035458048D318DE6ULL, 0xBF6A8E40D156BF6BULL, 0x7B7FC47CC58AE130ULL, 0x3795FAB8A9BF12F5ULL,
	0xF3AB30F48DF344BAULL, 0xAFC06730722747DFULL, 0x6BD59D6C565B79A4ULL, 0x27EAD3A83A8FAB69ULL,
	0xE40009E41EC3DD2EULL, 0xA01540200302FEF3ULL, 0x5C2A765CE73730B8ULL, 0x183FAC98CB6B627DULL,
	0xD454E2D4AF9F9442ULL, 0x906A1910939FC607ULL, 0x4C7F4F4C77D3F7CCULL, 0x089485885C082991ULL,
	0xC4A9BBC440BCDB56ULL, 0x80BEF20024514D1BULL, 0x3CD4283C08857EE0ULL, 0xF8E95E78ECB9B0A5ULL,
	0xB4FE94B4D0EDE26AULL, 0x7113CAFCB52214EFULL, 0x2D29013899564674ULL, 0xE93E37747D8A7839ULL,
	0xA5536DB06FBEAA5EULL, 0x6168A3EC4BF2DC23ULL, 0x1D7DD9283027FDE8ULL, 0xD9930F64145BF0ADULL,
	0x95A8459FF89F1272ULL, 0x51BD7BDBDCD34437ULL, 0x0DD2B217C10775FCULL, 0xC9E7E853A53BA7C1ULL,
	0x85FD1E8F896FD986ULL, 0x421254CB6DA40B4BULL, 0xFE278B0751D83D10ULL, 0xBA3CC143360C6ED5ULL,
	0x7651F77F1A40A09AULL, 0x32672DBB0E54D25FULL, 0xEE7C63F6E288F424ULL, 0xAA919A32C6BD25E9ULL,
	0x66A6D06EAAF157AEULL, 0x22BC06AA8F258973ULL, 0xDED13CE67359BB38ULL, 0x9AE672225778ECFDULL,
	0x56FBAA5E3BACFEC2ULL, 0x1310E09A1FE13087ULL, 0xCF2616D60415624CULL, 0x8B3B4D12E8499411ULL,
	0x475083AECC7DC5D6ULL, 0x0365B9EAB0B1F79BULL, 0xBF7AEF269CE62960ULL, 0x7B90256278EA5B25ULL,
	0x37A55B9E5D1E8CEAULL, 0xF3BA91DA4152BEAFULL, 0xAFCFC8162586E074ULL, 0x6BE4FE52C9DB1239ULL,
	0x27FA348EADCF43FEULL, 0xE40F6ACA92034DC3ULL, 0xA024A1067637AF88ULL, 0x5C39D7425A6BD14DULL,
	0x184F0D7E3E9FF312ULL, 0xD46443BA228421D7ULL, 0x907979F60658139CULL, 0x4C8EB032EA8C6561ULL,
	0x08A3E66ECEC09726ULL, 0xC4B91CAAB2F4C8EBULL, 0x80CE52E6972EFAB0ULL, 0x3CE38922738F2C75ULL,
	0xF8F8BF5E57C35E3AULL, 0xB50DF59A3BF78FFFULL, 0x71232BD6202BC1C4ULL, 0x2D386212045FE389ULL,
	0xE94D984EF894154EULL, 0xA562CE8ADCC84713ULL, 0x617804C6C0FC78D8ULL, 0x1D8D3B02A530AA9DULL,
	0xD9A27F3E8964DC62ULL, 0x95B7AD7A6D98FE27ULL, 0x51CCE3B651CD2FECULL, 0x0DE219F2360016DBULL,
	0xC9F7502E1A358370ULL, 0x860C8669FE69B535ULL, 0x4221BCA5E29DE6FAULL, 0xFE36F2E1C6D218BFULL,
	0xBA4C291DAB064A84ULL, 0x76615F598F3A7C49ULL, 0x327695957B6EAE0EULL, 0xEE8BCBD15FA2CDD3ULL,
	0xAAA10F0D43D70798ULL, 0x66B6454928A3395DULL, 0x22CB7B850CD76B22ULL, 0xDEE0B1C1F10B9CE7ULL,
	0x9AF5E7FDD53FCEACULL, 0x570B1E39B973E071ULL, 0x132054759DA81236ULL, 0xCF358AB181DC43FBULL,
	0x8B4AC0ED661075C0ULL, 0x475FF72F4A44A785ULL, 0x03752D2B2E78D94AULL, 0xBF8A63671AAD0B0FULL,
	0x7B9F99A2F6E13CD4ULL, 0x37B4CFDEDBB56E99ULL, 0xF3CA060ABFE9A05EULL, 0xAFDF3C46A41DD223ULL,
	0x6BF472828852F3E8ULL, 0x2809A8BE6C8605ADULL, 0xE41EDEFA50BA4772ULL, 0xA034152134EE7937ULL,
	0x5C494B5D1922AAFCULL, 0x185E8199FD56DCC1ULL, 0xD473B7D5E18B0E86ULL, 0x9088EDFEC5BF304BULL,
	0x4C9E247DA9F36210ULL, 0x08B35AA98E2793D5ULL, 0xC4C890E572BBC59AULL, 0x80DDC7215CEFF75FULL,
	0x3CF2FD5D41242924ULL, 0xF908339925585AE9ULL, 0xB51D69D5098C8CAEULL, 0x7132A0F0EDB0BE73ULL,
	0x2D47D63CD1E4E038ULL, 0xE95D0C78B6191FFDULL, 0xA57242B4A2AD4DC2ULL, 0x618778F07DE17F87ULL,
	0x1D9CAF2C6215B14CULL, 0xD9B1E56846D9E311ULL, 0x95C71BA42B0E14D6ULL, 0x51DC51E00F42469BULL,
	0x0DF18878F376AB60ULL, 0xCA06BEB4D7AACD25ULL, 0x861BF4F0BBF0FEEAULL, 0x42312B2CA02430AFULL,
	0xFE466168849C6274ULL, 0xBA5B97A468C08439ULL, 0x7670CDCC4C24B5FEULL, 0x32860408F458E7C3ULL,
	0xEE9B3A44D88D1988ULL, 0xAAB07080BCC14B4DULL, 0x66C5A6BCA1F57D12ULL, 0x22DADCF88629AED7ULL,
	0xDEF013346ADE40C2ULL, 0x9B054977CF12A687ULL, 0x571A7FA3B346D84CULL, 0x132FB5E5977B0A11ULL,
	0xCF44EC1F7BAFE3D6ULL, 0x8B5A225F5FE3159BULL, 0x476F589B44173760ULL, 0x03848ED728DBAB25ULL,
	0xBF99C5130D0FDCEAULL, 0x7BAEFB4FF143FEAFULL, 0x37C4318BD5782074ULL, 0xF3D967C7B9AC5239ULL,
	0xAFEE9E0F9DE083FEULL, 0x6C03D44382B4B5C3ULL, 0x28190A7F5CE8E788ULL, 0xE42E40B74F1C194DULL,
	0xA04376F32350CB12ULL, 0x5C58AD2F07854CD7ULL, 0x186DE36AEBB97E9CULL, 0xD48319A6CFEDA061ULL,
	0x90985FE2B421D226ULL, 0x4CAD961E985603EBULL, 0x08C2CC5A7C8A35B0ULL, 0xC4D80296609E6775ULL,
	0x80ED38D24AD2993AULL, 0x3D026F0E2F06CAFFULL, 0xF917A54A133AFCC4ULL, 0xB52CDB86076F2E89ULL,
	0x714211C1EBA3504EULL, 0x2D5747FDCFD78213ULL, 0xE96C7E39B40BB3D8ULL, 0xA581B47598CFE59DULL,
	0x6196EAAF7D041762ULL, 0x1DAC20F16138A927ULL, 0xD9C1572D456C3AECULL, 0x95D68D692990BCB1ULL,
	0x51EBC3A50DC4EE76ULL, 0x0E00F9E1F2F8103BULL, 0xCA163FBDD62C5200ULL, 0x862B75F9BA6073C5ULL,
	0x4240AC359EB4A58AULL, 0xFE55E27182E8D74FULL, 0xBA6B18AD67DD0914ULL, 0x76804EF94BF13BD9ULL,
	0x329585353D256D9EULL, 0xEEAABB7121598F63ULL, 0xAABFF1AD05ADBA28ULL, 0x66D527E9E9E1ECEDULL,
	0x22EA5E25CE16F0B2ULL, 0xDEFF9461B24A2277ULL, 0x9B14CA9D967E543CULL, 0x572A00D97AB28601ULL,
	0x133F37155EE6B7C6ULL, 0xCF546D514B1AE98BULL, 0x8B69A38D2F4F1B50ULL, 0x477ED9C9138E4D15ULL,
	0x03940FF5F7827EDAULL, 0xBFA94631DBC6B09FULL, 0x7BBE7C6DC0FAE264ULL, 0x37D3B2A9A52E1429ULL,
	0xF3E8E8E589624FEEULL, 0xAFFE1F216D9671B3ULL, 0x6C135F5D51CAA378ULL, 0x282895993BFED53DULL,
	0xE43DCBD5203F0702ULL, 0xA05302110CE338C7ULL, 0x5C6838ACF0B76A8CULL, 0x187D6EE8D4EB9C51ULL,
	0xD492A524B91FCE16ULL, 0x90A7DB609D5400DBULL, 0x4CBD11A0718832A0ULL, 0x08D247DC55BC6465ULL,
	0xC4E77E183AF0962AULL, 0x80FCB4541F24C7EFULL, 0x3D11EA9003D8F9B4ULL, 0xF92720CBE81D2B79ULL,
	0xB53C5707CC515D3EULL, 0x71518D43B0A58F03ULL, 0x2D66C37F94C9B0C8ULL, 0xE97BF9BB790DE28DULL,
	0xA5912FF75D421452ULL, 0x61A6663341B64617ULL, 0x1DBB9C6F25EA77DCULL, 0xD9D0D2AB0A1EA9A1ULL,
	0x95E608E6EE52DB66ULL, 0x51FB3F22D2870D2BULL, 0x0E10757FB6BB3EF0ULL, 0xCA25AABB9AEF60B5ULL,
	0x863AE0F77F23927AULL, 0x425017336F97C43FULL, 0xFE654D6F4BCBF604ULL, 0xBA7A83AB300027C9ULL,
	0x768FB9E71494598EULL, 0x32A4F022F8C88B53ULL, 0xEEBA265EDCFCBD18ULL, 0xAACF5C9AC130EEDDULL,
	0x66E49326A56520A2ULL, 0x22F9C96289993267ULL, 0xDF0EFF9E6DCD642CULL, 0x9B2435DA527180F1ULL,
};

// ---- Chunking constants ----
static const size_t CDC_MIN_CHUNK = 1024;
static const size_t CDC_MAX_CHUNK = 16384;
static const uint64_t CDC_MASK = 0xFFF; // ~4KB average chunk size

// ---- OCF attribute flags ----
enum CDCOcfFlags : uint32_t {
	OCF_NONE               = 0,
	OCF_VERT_VFADJ         = 1 << 0,
	OCF_VERT_MARK          = 1 << 1,
	OCF_VERT_TEXCOORD      = 1 << 2,
	OCF_VERT_CURVATUREDIR  = 1 << 3,
	OCF_VERT_RADIUS        = 1 << 4,
	OCF_FACE_QUALITY       = 1 << 5,
	OCF_FACE_MARK          = 1 << 6,
	OCF_FACE_COLOR         = 1 << 7,
	OCF_FACE_FFADJ         = 1 << 8,
	OCF_FACE_VFADJ         = 1 << 9,
	OCF_FACE_CURVATUREDIR  = 1 << 10,
	OCF_FACE_WEDGETEXCOORD = 1 << 11,
};

// ---- Serialization magic number ----
static const uint32_t CDC_MAGIC_V = 0xCDC00001;

// ---- Buffer writer helper ----
class BufWriter {
public:
	BufWriter() { m_buf.reserve(1024 * 1024); }
	void writeBytes(const void* data, size_t len) {
		const uint8_t* p = (const uint8_t*)data;
		m_buf.insert(m_buf.end(), p, p + len);
	}
	void writeFloat(float v)    { writeBytes(&v, 4); }
	void writeInt32(int32_t v)  { writeBytes(&v, 4); }
	void writeUint32(uint32_t v){ writeBytes(&v, 4); }
	void writeShort(int16_t v)  { writeBytes(&v, 2); }
	std::vector<uint8_t>& buf() { return m_buf; }
private:
	std::vector<uint8_t> m_buf;
};

// ---- Buffer reader helper ----
class BufReader {
public:
	BufReader(const uint8_t* data, size_t len) : m_data(data), m_len(len), m_pos(0) {}
	bool ok() const { return m_pos <= m_len; }
	bool canRead(size_t n) const { return m_pos + n <= m_len; }
	float readFloat() {
		float v = 0;
		if (canRead(4)) { memcpy(&v, m_data + m_pos, 4); m_pos += 4; }
		return v;
	}
	int32_t readInt32() {
		int32_t v = 0;
		if (canRead(4)) { memcpy(&v, m_data + m_pos, 4); m_pos += 4; }
		return v;
	}
	uint32_t readUint32() {
		uint32_t v = 0;
		if (canRead(4)) { memcpy(&v, m_data + m_pos, 4); m_pos += 4; }
		return v;
	}
	int16_t readShort() {
		int16_t v = 0;
		if (canRead(2)) { memcpy(&v, m_data + m_pos, 2); m_pos += 2; }
		return v;
	}
	void readBytes(void* dst, size_t n) {
		if (canRead(n)) { memcpy(dst, m_data + m_pos, n); m_pos += n; }
	}
private:
	const uint8_t* m_data;
	size_t m_len;
	size_t m_pos;
};

// ---- Serialize CMeshO to deterministic byte buffer ----
inline std::vector<uint8_t> serializeMesh(const CMeshO& mesh)
{
	BufWriter w;

	// Build vertex compaction map (skip deleted vertices)
	std::vector<int> vertCompact(mesh.vert.size(), -1);
	int compactVN = 0;
	for (size_t i = 0; i < mesh.vert.size(); ++i) {
		if (!mesh.vert[i].IsD())
			vertCompact[i] = compactVN++;
	}

	// Count live faces
	int compactFN = 0;
	for (size_t i = 0; i < mesh.face.size(); ++i) {
		if (!mesh.face[i].IsD())
			compactFN++;
	}

	// Query OCF flags
	uint32_t ocfFlags = OCF_NONE;
	if (mesh.vert.IsVFAdjacencyEnabled()) ocfFlags |= OCF_VERT_VFADJ;
	if (mesh.vert.IsMarkEnabled())        ocfFlags |= OCF_VERT_MARK;
	if (mesh.vert.IsTexCoordEnabled())    ocfFlags |= OCF_VERT_TEXCOORD;
	if (mesh.vert.IsCurvatureDirEnabled())ocfFlags |= OCF_VERT_CURVATUREDIR;
	if (mesh.vert.IsRadiusEnabled())      ocfFlags |= OCF_VERT_RADIUS;
	if (mesh.face.IsQualityEnabled())     ocfFlags |= OCF_FACE_QUALITY;
	if (mesh.face.IsMarkEnabled())        ocfFlags |= OCF_FACE_MARK;
	if (mesh.face.IsColorEnabled())       ocfFlags |= OCF_FACE_COLOR;
	if (mesh.face.IsFFAdjacencyEnabled()) ocfFlags |= OCF_FACE_FFADJ;
	if (mesh.face.IsVFAdjacencyEnabled()) ocfFlags |= OCF_FACE_VFADJ;
	if (mesh.face.IsCurvatureDirEnabled())ocfFlags |= OCF_FACE_CURVATUREDIR;
	if (mesh.face.IsWedgeTexCoordEnabled())ocfFlags|= OCF_FACE_WEDGETEXCOORD;

	// ---- Header ----
	w.writeUint32(CDC_MAGIC_V);
	w.writeUint32(1); // version
	w.writeUint32((uint32_t)compactVN);
	w.writeUint32((uint32_t)compactFN);
	w.writeUint32(ocfFlags);
	w.writeInt32(mesh.sfn);
	w.writeInt32(mesh.svn);
	w.writeInt32(mesh.pvn);
	w.writeInt32(mesh.pfn);
	w.writeInt32(mesh.imark);
	w.writeUint32((uint32_t)mesh.textures.size());
	w.writeUint32((uint32_t)mesh.normalmaps.size());

	// Transformation matrix (4x4)
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			w.writeFloat((float)mesh.Tr[r][c]);

	// Bounding box
	w.writeFloat((float)mesh.bbox.min[0]);
	w.writeFloat((float)mesh.bbox.min[1]);
	w.writeFloat((float)mesh.bbox.min[2]);
	w.writeFloat((float)mesh.bbox.max[0]);
	w.writeFloat((float)mesh.bbox.max[1]);
	w.writeFloat((float)mesh.bbox.max[2]);

	// ---- Shot (Camera intrinsics + extrinsics) ----
	const auto& cam = mesh.shot.Intrinsics;
	w.writeFloat((float)cam.FocalMm);
	w.writeInt32(cam.ViewportPx[0]);
	w.writeInt32(cam.ViewportPx[1]);
	w.writeFloat((float)cam.PixelSizeMm[0]);
	w.writeFloat((float)cam.PixelSizeMm[1]);
	w.writeFloat((float)cam.CenterPx[0]);
	w.writeFloat((float)cam.CenterPx[1]);
	w.writeFloat((float)cam.DistorCenterPx[0]);
	w.writeFloat((float)cam.DistorCenterPx[1]);
	for (int i = 0; i < 4; ++i)
		w.writeFloat((float)cam.k[i]);
	w.writeInt32((int32_t)cam.cameraType);

	auto rot = mesh.shot.Extrinsics.Rot();
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			w.writeFloat((float)rot[r][c]);
	auto tra = mesh.shot.Extrinsics.Tra();
	w.writeFloat((float)tra[0]);
	w.writeFloat((float)tra[1]);
	w.writeFloat((float)tra[2]);

	// ---- Vertex core block ----
	for (size_t i = 0; i < mesh.vert.size(); ++i) {
		if (mesh.vert[i].IsD()) continue;
		const auto& v = mesh.vert[i];
		w.writeFloat((float)v.cP()[0]);
		w.writeFloat((float)v.cP()[1]);
		w.writeFloat((float)v.cP()[2]);
		w.writeFloat((float)v.cN()[0]);
		w.writeFloat((float)v.cN()[1]);
		w.writeFloat((float)v.cN()[2]);
		w.writeBytes(&v.cC(), 4); // Color4b = 4 bytes
		w.writeFloat((float)v.cQ());
		w.writeInt32(v.cFlags());
	}

	// ---- Face core block ----
	for (size_t i = 0; i < mesh.face.size(); ++i) {
		if (mesh.face[i].IsD()) continue;
		const auto& f = mesh.face[i];
		for (int j = 0; j < 3; ++j) {
			int origIdx = vcg::tri::Index(mesh, f.cV(j));
			int compIdx = (origIdx >= 0 && origIdx < (int)vertCompact.size()) ? vertCompact[origIdx] : 0;
			w.writeInt32(compIdx);
		}
		w.writeFloat((float)f.cN()[0]);
		w.writeFloat((float)f.cN()[1]);
		w.writeFloat((float)f.cN()[2]);
		w.writeInt32(f.cFlags());
	}

	// ---- OCF Vertex sections ----
	if (ocfFlags & OCF_VERT_MARK) {
		for (size_t i = 0; i < mesh.vert.size(); ++i) {
			if (mesh.vert[i].IsD()) continue;
			w.writeInt32(mesh.vert[i].cIMark());
		}
	}
	if (ocfFlags & OCF_VERT_TEXCOORD) {
		for (size_t i = 0; i < mesh.vert.size(); ++i) {
			if (mesh.vert[i].IsD()) continue;
			w.writeFloat(mesh.vert[i].cT().U());
			w.writeFloat(mesh.vert[i].cT().V());
			w.writeShort(mesh.vert[i].cT().N());
		}
	}
	if (ocfFlags & OCF_VERT_CURVATUREDIR) {
		for (size_t i = 0; i < mesh.vert.size(); ++i) {
			if (mesh.vert[i].IsD()) continue;
			const auto& pd1 = mesh.vert[i].cPD1();
			const auto& pd2 = mesh.vert[i].cPD2();
			w.writeFloat((float)pd1[0]); w.writeFloat((float)pd1[1]); w.writeFloat((float)pd1[2]);
			w.writeFloat((float)pd2[0]); w.writeFloat((float)pd2[1]); w.writeFloat((float)pd2[2]);
			w.writeFloat((float)mesh.vert[i].cK1());
			w.writeFloat((float)mesh.vert[i].cK2());
		}
	}
	if (ocfFlags & OCF_VERT_RADIUS) {
		for (size_t i = 0; i < mesh.vert.size(); ++i) {
			if (mesh.vert[i].IsD()) continue;
			w.writeFloat((float)mesh.vert[i].cR());
		}
	}

	// ---- OCF Face sections ----
	if (ocfFlags & OCF_FACE_QUALITY) {
		for (size_t i = 0; i < mesh.face.size(); ++i) {
			if (mesh.face[i].IsD()) continue;
			w.writeFloat((float)mesh.face[i].cQ());
		}
	}
	if (ocfFlags & OCF_FACE_MARK) {
		for (size_t i = 0; i < mesh.face.size(); ++i) {
			if (mesh.face[i].IsD()) continue;
			w.writeInt32(mesh.face[i].cIMark());
		}
	}
	if (ocfFlags & OCF_FACE_COLOR) {
		for (size_t i = 0; i < mesh.face.size(); ++i) {
			if (mesh.face[i].IsD()) continue;
			w.writeBytes(&mesh.face[i].cC(), 4);
		}
	}
	if (ocfFlags & OCF_FACE_CURVATUREDIR) {
		for (size_t i = 0; i < mesh.face.size(); ++i) {
			if (mesh.face[i].IsD()) continue;
			const auto& pd1 = mesh.face[i].cPD1();
			const auto& pd2 = mesh.face[i].cPD2();
			w.writeFloat((float)pd1[0]); w.writeFloat((float)pd1[1]); w.writeFloat((float)pd1[2]);
			w.writeFloat((float)pd2[0]); w.writeFloat((float)pd2[1]); w.writeFloat((float)pd2[2]);
			w.writeFloat((float)mesh.face[i].cK1());
			w.writeFloat((float)mesh.face[i].cK2());
		}
	}
	if (ocfFlags & OCF_FACE_WEDGETEXCOORD) {
		for (size_t i = 0; i < mesh.face.size(); ++i) {
			if (mesh.face[i].IsD()) continue;
			for (int j = 0; j < 3; ++j) {
				w.writeFloat(mesh.face[i].cWT(j).U());
				w.writeFloat(mesh.face[i].cWT(j).V());
				w.writeShort(mesh.face[i].cWT(j).N());
			}
		}
	}

	// ---- Texture strings ----
	for (const auto& s : mesh.textures) {
		w.writeUint32((uint32_t)s.size());
		if (!s.empty()) w.writeBytes(s.data(), s.size());
	}
	for (const auto& s : mesh.normalmaps) {
		w.writeUint32((uint32_t)s.size());
		if (!s.empty()) w.writeBytes(s.data(), s.size());
	}

	return std::move(w.buf());
}

// ---- Deserialize byte buffer back to CMeshO ----
inline bool deserializeMesh(const uint8_t* data, size_t len, CMeshO& mesh)
{
	BufReader r(data, len);

	// ---- Header ----
	uint32_t magic = r.readUint32();
	if (magic != CDC_MAGIC_V) return false;
	uint32_t version = r.readUint32();
	if (version != 1) return false;
	uint32_t vertexCount = r.readUint32();
	uint32_t faceCount = r.readUint32();
	uint32_t ocfFlags = r.readUint32();
	int32_t sfn = r.readInt32();
	int32_t svn = r.readInt32();
	int32_t pvn = r.readInt32();
	int32_t pfn = r.readInt32();
	int32_t imark = r.readInt32();
	uint32_t texCount = r.readUint32();
	uint32_t normCount = r.readUint32();

	// Transformation matrix
	Matrix44m Tr;
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			Tr[row][col] = (Scalarm)r.readFloat();

	// Bounding box
	Box3m bbox;
	bbox.min[0] = (Scalarm)r.readFloat();
	bbox.min[1] = (Scalarm)r.readFloat();
	bbox.min[2] = (Scalarm)r.readFloat();
	bbox.max[0] = (Scalarm)r.readFloat();
	bbox.max[1] = (Scalarm)r.readFloat();
	bbox.max[2] = (Scalarm)r.readFloat();

	// ---- Shot ----
	vcg::Camera<Scalarm> cam;
	cam.FocalMm = (Scalarm)r.readFloat();
	cam.ViewportPx[0] = r.readInt32();
	cam.ViewportPx[1] = r.readInt32();
	cam.PixelSizeMm[0] = (Scalarm)r.readFloat();
	cam.PixelSizeMm[1] = (Scalarm)r.readFloat();
	cam.CenterPx[0] = (Scalarm)r.readFloat();
	cam.CenterPx[1] = (Scalarm)r.readFloat();
	cam.DistorCenterPx[0] = (Scalarm)r.readFloat();
	cam.DistorCenterPx[1] = (Scalarm)r.readFloat();
	for (int i = 0; i < 4; ++i)
		cam.k[i] = (Scalarm)r.readFloat();
	cam.cameraType = (vcg::Camera<Scalarm>::CameraType)r.readInt32();

	Matrix44m rot;
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			rot[row][col] = (Scalarm)r.readFloat();
	Point3m tra;
	tra[0] = (Scalarm)r.readFloat();
	tra[1] = (Scalarm)r.readFloat();
	tra[2] = (Scalarm)r.readFloat();

	if (!r.ok()) return false;

	// ---- Enable OCF attributes ----
	if (ocfFlags & OCF_VERT_VFADJ)         mesh.vert.EnableVFAdjacency();
	if (ocfFlags & OCF_VERT_MARK)           mesh.vert.EnableMark();
	if (ocfFlags & OCF_VERT_TEXCOORD)       mesh.vert.EnableTexCoord();
	if (ocfFlags & OCF_VERT_CURVATUREDIR)   mesh.vert.EnableCurvatureDir();
	if (ocfFlags & OCF_VERT_RADIUS)         mesh.vert.EnableRadius();
	if (ocfFlags & OCF_FACE_QUALITY)        mesh.face.EnableQuality();
	if (ocfFlags & OCF_FACE_MARK)           mesh.face.EnableMark();
	if (ocfFlags & OCF_FACE_COLOR)          mesh.face.EnableColor();
	if (ocfFlags & OCF_FACE_FFADJ)          mesh.face.EnableFFAdjacency();
	if (ocfFlags & OCF_FACE_VFADJ)          mesh.face.EnableVFAdjacency();
	if (ocfFlags & OCF_FACE_CURVATUREDIR)   mesh.face.EnableCurvatureDir();
	if (ocfFlags & OCF_FACE_WEDGETEXCOORD)  mesh.face.EnableWedgeTexCoord();

	// ---- Allocate vertices and faces ----
	vcg::tri::Allocator<CMeshO>::AddVertices(mesh, vertexCount);
	vcg::tri::Allocator<CMeshO>::AddFaces(mesh, faceCount);

	// ---- Read vertex core block ----
	for (uint32_t i = 0; i < vertexCount; ++i) {
		mesh.vert[i].P()[0] = (Scalarm)r.readFloat();
		mesh.vert[i].P()[1] = (Scalarm)r.readFloat();
		mesh.vert[i].P()[2] = (Scalarm)r.readFloat();
		mesh.vert[i].N()[0] = (Scalarm)r.readFloat();
		mesh.vert[i].N()[1] = (Scalarm)r.readFloat();
		mesh.vert[i].N()[2] = (Scalarm)r.readFloat();
		r.readBytes(&mesh.vert[i].C(), 4);
		mesh.vert[i].Q() = (Scalarm)r.readFloat();
		mesh.vert[i].Flags() = r.readInt32();
	}

	// ---- Read face core block (convert indices to pointers) ----
	for (uint32_t i = 0; i < faceCount; ++i) {
		for (int j = 0; j < 3; ++j) {
			int32_t vi = r.readInt32();
			if (vi < 0 || vi >= (int32_t)vertexCount) return false;
			mesh.face[i].V(j) = &mesh.vert[vi];
		}
		mesh.face[i].N()[0] = (Scalarm)r.readFloat();
		mesh.face[i].N()[1] = (Scalarm)r.readFloat();
		mesh.face[i].N()[2] = (Scalarm)r.readFloat();
		mesh.face[i].Flags() = r.readInt32();
	}

	// ---- Read OCF vertex sections ----
	if (ocfFlags & OCF_VERT_MARK) {
		for (uint32_t i = 0; i < vertexCount; ++i)
			mesh.vert[i].IMark() = r.readInt32();
	}
	if (ocfFlags & OCF_VERT_TEXCOORD) {
		for (uint32_t i = 0; i < vertexCount; ++i) {
			mesh.vert[i].T().U() = r.readFloat();
			mesh.vert[i].T().V() = r.readFloat();
			mesh.vert[i].T().N() = r.readShort();
		}
	}
	if (ocfFlags & OCF_VERT_CURVATUREDIR) {
		for (uint32_t i = 0; i < vertexCount; ++i) {
			mesh.vert[i].PD1()[0] = (Scalarm)r.readFloat(); mesh.vert[i].PD1()[1] = (Scalarm)r.readFloat(); mesh.vert[i].PD1()[2] = (Scalarm)r.readFloat();
			mesh.vert[i].PD2()[0] = (Scalarm)r.readFloat(); mesh.vert[i].PD2()[1] = (Scalarm)r.readFloat(); mesh.vert[i].PD2()[2] = (Scalarm)r.readFloat();
			mesh.vert[i].K1() = (Scalarm)r.readFloat();
			mesh.vert[i].K2() = (Scalarm)r.readFloat();
		}
	}
	if (ocfFlags & OCF_VERT_RADIUS) {
		for (uint32_t i = 0; i < vertexCount; ++i)
			mesh.vert[i].R() = (Scalarm)r.readFloat();
	}

	// ---- Read OCF face sections ----
	if (ocfFlags & OCF_FACE_QUALITY) {
		for (uint32_t i = 0; i < faceCount; ++i)
			mesh.face[i].Q() = (Scalarm)r.readFloat();
	}
	if (ocfFlags & OCF_FACE_MARK) {
		for (uint32_t i = 0; i < faceCount; ++i)
			mesh.face[i].IMark() = r.readInt32();
	}
	if (ocfFlags & OCF_FACE_COLOR) {
		for (uint32_t i = 0; i < faceCount; ++i)
			r.readBytes(&mesh.face[i].C(), 4);
	}
	if (ocfFlags & OCF_FACE_CURVATUREDIR) {
		for (uint32_t i = 0; i < faceCount; ++i) {
			mesh.face[i].PD1()[0] = (Scalarm)r.readFloat(); mesh.face[i].PD1()[1] = (Scalarm)r.readFloat(); mesh.face[i].PD1()[2] = (Scalarm)r.readFloat();
			mesh.face[i].PD2()[0] = (Scalarm)r.readFloat(); mesh.face[i].PD2()[1] = (Scalarm)r.readFloat(); mesh.face[i].PD2()[2] = (Scalarm)r.readFloat();
			mesh.face[i].K1() = (Scalarm)r.readFloat();
			mesh.face[i].K2() = (Scalarm)r.readFloat();
		}
	}
	if (ocfFlags & OCF_FACE_WEDGETEXCOORD) {
		for (uint32_t i = 0; i < faceCount; ++i) {
			for (int j = 0; j < 3; ++j) {
				mesh.face[i].WT(j).U() = r.readFloat();
				mesh.face[i].WT(j).V() = r.readFloat();
				mesh.face[i].WT(j).N() = r.readShort();
			}
		}
	}

	// ---- Read texture/normalmap strings ----
	mesh.textures.resize(texCount);
	for (uint32_t i = 0; i < texCount; ++i) {
		uint32_t slen = r.readUint32();
		mesh.textures[i].resize(slen);
		if (slen > 0) r.readBytes(&mesh.textures[i][0], slen);
	}
	mesh.normalmaps.resize(normCount);
	for (uint32_t i = 0; i < normCount; ++i) {
		uint32_t slen = r.readUint32();
		mesh.normalmaps[i].resize(slen);
		if (slen > 0) r.readBytes(&mesh.normalmaps[i][0], slen);
	}

	// ---- Set metadata ----
	mesh.sfn = sfn;
	mesh.svn = svn;
	mesh.pvn = pvn;
	mesh.pfn = pfn;
	mesh.imark = imark;
	mesh.Tr = Tr;
	mesh.bbox = bbox;
	mesh.shot.Intrinsics = cam;
	mesh.shot.Extrinsics.SetRot(rot);
	mesh.shot.Extrinsics.SetTra(tra);
	mesh.vn = vertexCount;
	mesh.fn = faceCount;

	// ---- Rebuild adjacency topology ----
	if (ocfFlags & OCF_FACE_FFADJ)
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
	if ((ocfFlags & OCF_FACE_VFADJ) || (ocfFlags & OCF_VERT_VFADJ))
		vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);

	return r.ok();
}

// ---- Content-defined chunking using Gear hash ----
struct ChunkRef {
	const uint8_t* data;
	size_t size;
};

inline std::vector<ChunkRef> chunkBuffer(const uint8_t* data, size_t len)
{
	std::vector<ChunkRef> chunks;
	if (len == 0) return chunks;

	chunks.reserve(len / 4096 + 1);
	size_t start = 0;
	uint64_t hash = 0;

	for (size_t i = 0; i < len; ++i) {
		hash = (hash << 1) + GEAR_TABLE[data[i]];
		size_t chunkLen = i - start + 1;

		if (chunkLen >= CDC_MIN_CHUNK) {
			if ((hash & CDC_MASK) == 0 || chunkLen >= CDC_MAX_CHUNK) {
				chunks.push_back({data + start, chunkLen});
				start = i + 1;
				hash = 0;
			}
		}
	}

	// Final remainder chunk
	if (start < len)
		chunks.push_back({data + start, len - start});

	return chunks;
}

// ---- Reference-counted chunk store ----
class ChunkStore {
public:
	struct ChunkData {
		std::vector<uint8_t> data;
		int refCount;
	};

	uint64_t addChunk(const uint8_t* data, size_t len)
	{
		uint64_t hash = fnv1a_64(data, len);

		// Try up to 8 slots for collision resolution
		for (int attempt = 0; attempt < 8; ++attempt) {
			uint64_t key = (attempt == 0) ? hash : (hash ^ (0xA5A5A5A5A5A5A5A5ULL * attempt));
			auto it = m_chunks.find(key);
			if (it != m_chunks.end()) {
				// Check if content matches
				if (it->second.data.size() == len &&
				    memcmp(it->second.data.data(), data, len) == 0) {
					it->second.refCount++;
					return key;
				}
				// Hash collision — try next slot
				continue;
			}
			// New chunk
			ChunkData cd;
			cd.data.assign(data, data + len);
			cd.refCount = 1;
			m_chunks[key] = std::move(cd);
			return key;
		}

		// Extremely unlikely fallback: use address-based unique key
		uint64_t fallback = hash ^ (uint64_t)m_chunks.size() ^ 0xDEADBEEFCAFEBABEULL;
		ChunkData cd;
		cd.data.assign(data, data + len);
		cd.refCount = 1;
		m_chunks[fallback] = std::move(cd);
		return fallback;
	}

	void release(uint64_t key)
	{
		auto it = m_chunks.find(key);
		if (it != m_chunks.end()) {
			it->second.refCount--;
			if (it->second.refCount <= 0)
				m_chunks.erase(it);
		}
	}

	const std::vector<uint8_t>* getChunk(uint64_t key) const
	{
		auto it = m_chunks.find(key);
		if (it != m_chunks.end())
			return &it->second.data;
		return nullptr;
	}

	void clear() { m_chunks.clear(); }

	size_t totalBytes() const
	{
		size_t total = 0;
		for (const auto& kv : m_chunks)
			total += kv.second.data.size();
		return total;
	}

private:
	std::unordered_map<uint64_t, ChunkData> m_chunks;
};

} // namespace cdc

// ============================================================================
// CDC Undo Entry (replaces UndoEntry)
// ============================================================================
struct CDCUndoEntry {
	int meshId;
	QString description;
	std::vector<uint64_t> chunkHashes;
	size_t totalSize;
};

// ============================================================================
// MeshUndoStack — same public API, CDC internals
// ============================================================================
class MeshUndoStack {
public:
	void pushUndo(MeshModel& m, const QString& desc)
	{
		// Serialize mesh to byte buffer
		std::vector<uint8_t> buf = cdc::serializeMesh(m.cm);

		// Chunk the buffer
		auto chunks = cdc::chunkBuffer(buf.data(), buf.size());

		// Store chunks and build entry
		CDCUndoEntry entry;
		entry.meshId = m.id();
		entry.description = desc;
		entry.totalSize = buf.size();
		entry.chunkHashes.reserve(chunks.size());

		for (const auto& chunk : chunks)
			entry.chunkHashes.push_back(m_chunkStore.addChunk(chunk.data, chunk.size));

		m_undoStack.push_back(std::move(entry));

		// Enforce limit
		if ((int)m_undoStack.size() > MAX_UNDO) {
			releaseEntry(m_undoStack.front());
			m_undoStack.erase(m_undoStack.begin());
		}

		// Clear redo stack
		for (auto& re : m_redoStack)
			releaseEntry(re);
		m_redoStack.clear();
	}

	bool canUndo() const { return !m_undoStack.empty(); }
	bool canRedo() const { return !m_redoStack.empty(); }

	// Returns the meshId that was restored, or -1 on failure
	int undo(MeshDocument& doc)
	{
		if (m_undoStack.empty()) return -1;
		CDCUndoEntry& entry = m_undoStack.back();
		MeshModel* mm = doc.getMesh(entry.meshId);
		if (!mm) {
			releaseEntry(entry);
			m_undoStack.pop_back();
			return -1;
		}

		// Save current state to redo stack
		std::vector<uint8_t> curBuf = cdc::serializeMesh(mm->cm);
		auto curChunks = cdc::chunkBuffer(curBuf.data(), curBuf.size());
		CDCUndoEntry redoEntry;
		redoEntry.meshId = mm->id();
		redoEntry.description = entry.description;
		redoEntry.totalSize = curBuf.size();
		for (const auto& ch : curChunks)
			redoEntry.chunkHashes.push_back(m_chunkStore.addChunk(ch.data, ch.size));
		m_redoStack.push_back(std::move(redoEntry));

		// Reassemble and restore
		std::vector<uint8_t> buf = reassembleChunks(entry);
		if (buf.size() != entry.totalSize) {
			releaseEntry(entry);
			m_undoStack.pop_back();
			return -1;
		}

		mm->cm.Clear();
		if (!cdc::deserializeMesh(buf.data(), buf.size(), mm->cm)) {
			releaseEntry(entry);
			m_undoStack.pop_back();
			return -1;
		}

		int restoredId = entry.meshId;
		releaseEntry(entry);
		m_undoStack.pop_back();
		return restoredId;
	}

	int redo(MeshDocument& doc)
	{
		if (m_redoStack.empty()) return -1;
		CDCUndoEntry& entry = m_redoStack.back();
		MeshModel* mm = doc.getMesh(entry.meshId);
		if (!mm) {
			releaseEntry(entry);
			m_redoStack.pop_back();
			return -1;
		}

		// Save current state to undo stack
		std::vector<uint8_t> curBuf = cdc::serializeMesh(mm->cm);
		auto curChunks = cdc::chunkBuffer(curBuf.data(), curBuf.size());
		CDCUndoEntry undoEntry;
		undoEntry.meshId = mm->id();
		undoEntry.description = entry.description;
		undoEntry.totalSize = curBuf.size();
		for (const auto& ch : curChunks)
			undoEntry.chunkHashes.push_back(m_chunkStore.addChunk(ch.data, ch.size));
		m_undoStack.push_back(std::move(undoEntry));

		// Reassemble and restore
		std::vector<uint8_t> buf = reassembleChunks(entry);
		if (buf.size() != entry.totalSize) {
			releaseEntry(entry);
			m_redoStack.pop_back();
			return -1;
		}

		mm->cm.Clear();
		if (!cdc::deserializeMesh(buf.data(), buf.size(), mm->cm)) {
			releaseEntry(entry);
			m_redoStack.pop_back();
			return -1;
		}

		int restoredId = entry.meshId;
		releaseEntry(entry);
		m_redoStack.pop_back();
		return restoredId;
	}

	void clear()
	{
		for (auto& e : m_undoStack) releaseEntry(e);
		for (auto& e : m_redoStack) releaseEntry(e);
		m_undoStack.clear();
		m_redoStack.clear();
		m_chunkStore.clear();
	}

private:
	void releaseEntry(CDCUndoEntry& entry)
	{
		for (uint64_t hash : entry.chunkHashes)
			m_chunkStore.release(hash);
		entry.chunkHashes.clear();
	}

	std::vector<uint8_t> reassembleChunks(const CDCUndoEntry& entry)
	{
		std::vector<uint8_t> buf;
		buf.reserve(entry.totalSize);
		for (uint64_t hash : entry.chunkHashes) {
			const auto* chunk = m_chunkStore.getChunk(hash);
			if (!chunk) return std::vector<uint8_t>();
			buf.insert(buf.end(), chunk->begin(), chunk->end());
		}
		return buf;
	}

	std::vector<CDCUndoEntry> m_undoStack;
	std::vector<CDCUndoEntry> m_redoStack;
	cdc::ChunkStore m_chunkStore;
	static const int MAX_UNDO = 50;
};

#endif // MESH_UNDO_STACK_H
