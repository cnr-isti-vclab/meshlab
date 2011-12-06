SGL = {};

SGL.undefV2 = function(v) {
	return new Array(2);
}

SGL.v2V = function(v) {
	return v.slice(0, 2);
}

SGL.v2S = function(s) {
	return [ s, s ];
}

SGL.v2C = function(x, y) {
	return [ x, y ];
}

SGL.zeroV2 = function() {
	return SGL.v2S(0.0);
}

SGL.oneV2 = function() {
	return SGL.v2S(1.0);
}

SGL.v2 = function() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return SGL.v2V(arguments[0]);
			}
			return SGL.v2S(arguments[0]);
		break;

		case 2:
			return SGL.v2V(arguments);
		break;

		default:
			return SGL.zeroV2();
		break;
	}
	return null;
}

SGL.dupV2 = function(v) {
	return v.slice(0, 2);
}

SGL.negV2 = function(v) {
	return SGL.v2C(-v[0], -v[1]);
}

SGL.addV2 = function(u, v) {
	return SGL.v2C(u[0]+v[0], u[1]+v[1]);
}

SGL.addV2S = function(v, s) {
	return SGL.v2C(v[0]+s, v[1]+s);
}

SGL.addSV2 = function(s, v) {
	return SGL.addV2S(v, s)
}

SGL.subV2 = function(u, v) {
	return SGL.v2C(u[0]-v[0], u[1]-v[1]);
}

SGL.subV2S = function(v, s) {
	return SGL.v2C(v[0]-s, v[1]-s);
}

SGL.subSV2 = function(v, s) {
	return SGL.v2C(s-v[0], s-v[1]);
}

SGL.mulV2 = function(u, v) {
	return SGL.v2C(u[0]*v[0], u[1]*v[1]);
}

SGL.mulV2S = function(v, s) {
	return SGL.v2C(v[0]*s, v[1]*s);
}

SGL.mulSV2 = function(s, v) {
	return SGL.mulV2S(v, s);
}

SGL.divV2 = function(u, v) {
	return SGL.v2C(u[0]/v[0], u[1]/v[1]);
}

SGL.divV2S = function(v, s) {
	return SGL.v2C(v[0]/s, v[1]/s);
}

SGL.divSV2 = function(s, v) {
	return SGL.v2C(s/v[0], s/v[1]);
}

SGL.rcpV2 = function(v) {
	return SGL.divSV2(1.0, v);
}

SGL.dotV2 = function(u, v) {
	return (u[0]*v[0] + u[1]*v[1]);
}

SGL.crossV2 = function(u, v) {
	return (u[0]*v[1] - u[1]*v[0]);
}

SGL.sqLengthV2 = function(v) {
	return SGL.dotV2(v, v);
}

SGL.lengthV2 = function(v) {
	return SGL.sqrt(sglSqLengthV2(v));
}

SGL.normalizedV2 = function(v) {
	var f = 1.0 / SGL.lengthV2(v);
	return SGL.mulV2S(v, f);
}

SGL.selfNegV2 = function(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	return v;
}

SGL.selfAddV2 = function(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	return u;
}

SGL.selfAddV2S = function(v, s) {
	v[0] += s;
	v[1] += s;
	return v;
}

SGL.selfAddSV2 = function(s, v) {
	v[0] += s;
	v[1] += s;
	return v;
}

SGL.selfSubV2 = function(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	return u;
}

SGL.selfSubV2S = function(v, s) {
	v[0] -= s;
	v[1] -= s;
	return v;
}

SGL.selfSubSV2 = function(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	return v;
}

SGL.selfMulV2 = function(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	return u;
}

SGL.selfMulV2S = function(v, s) {
	v[0] *= s;
	v[1] *= s;
	return v;
}

SGL.selfMulSV2 = function(s, v) {
	v[0] *= s;
	v[1] *= s;
	return v;
}

SGL.selfDivV2 = function(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	return u;
}

SGL.selfDivV2S = function(v, s) {
	u[0] /= s;
	u[1] /= s;
	return u;
}

SGL.selfDivSV2 = function(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	return v;
}

SGL.selfRcpV2 = function(v) {
	return SGL.selfDivSV2(1.0, v);
}

SGL.selfNormalizeV2 = function(v) {
	var f = 1.0 / SGL.lengthV2(v);
	return SGL.selfMulV2S(v, f);
}

SGL.minV2 = function(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1]))
	];
}

SGL.maxV2 = function(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1]))
	];
}

SGL.v2toV3 = function(v, z) {
	return SGL.v3C(v[0], v[1], z);
}

SGL.v2toV4 = function(v, z, w) {
	return SGL.v4C(v[0], v[1], z, w);
}
/***********************************************************************/


// 3-dimensional vector
/***********************************************************************/
SGL.undefV3 = function(v) {
	return new Array(3);
}

SGL.v3V = function(v) {
	return v.slice(0, 3);
}

SGL.v3S = function(s) {
	return [ s, s, s ];
}

SGL.v3C = function(x, y, z) {
	return [ x, y, z ];
}

SGL.zeroV3 = function() {
	return SGL.v3S(0.0);
}

SGL.oneV3 = function() {
	return SGL.v3S(1.0);
}

SGL.v3 = function() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return SGL.v3V(arguments[0]);
			}
			return SGL.v3S(arguments[0]);
		break;

		case 3:
			return SGL.v3V(arguments);
		break;

		default:
			return SGL.zeroV3();
		break;
	}
	return null;
}

SGL.dupV3 = function(v) {
	return v.slice(0, 3);
}

SGL.negV3 = function(v) {
	return SGL.v3C(-v[0], -v[1], -v[2]);
}

SGL.addV3 = function(u, v) {
	return SGL.v3C(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

SGL.addV3S = function(v, s) {
	return SGL.v3C(v[0]+s, v[1]+s, v[2]+s);
}

SGL.addSV3 = function(s, v) {
	return SGL.addV3S(v, s)
}

SGL.subV3 = function(u, v) {
	return SGL.v3C(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

SGL.subV3S = function(v, s) {
	return SGL.v3C(v[0]-s, v[1]-s, v[2]-s);
}

SGL.subSV3 = function(v, s) {
	return SGL.v3C(s-v[0], s-v[1], s-v[2]);
}

SGL.mulV3 = function(u, v) {
	return SGL.v3C(u[0]*v[0], u[1]*v[1], u[2]*v[2]);
}

SGL.mulV3S = function(v, s) {
	return SGL.v3C(v[0]*s, v[1]*s, v[2]*s);
}

SGL.mulSV3 = function(s, v) {
	return SGL.mulV3S(v, s);
}

SGL.divV3 = function(u, v) {
	return SGL.v3C(u[0]/v[0], u[1]/v[1], u[2]/v[2]);
}

SGL.divV3S = function(v, s) {
	return SGL.v3C(v[0]/s, v[1]/s, v[2]/s);
}

SGL.divSV3 = function(s, v) {
	return SGL.v3C(s/v[0], s/v[1], s/v[2]);
}

SGL.rcpV3 = function(v) {
	return SGL.divSV3(1.0, v);
}

SGL.dotV3 = function(u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2]);
}

SGL.crossV3 = function(u, v) {
	return SGL.v3C(u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]);
}

SGL.sqLengthV3 = function(v) {
	return SGL.dotV3(v, v);
}

SGL.lengthV3 = function(v) {
	return SGL.sqrt(sglSqLengthV3(v));
}

SGL.normalizedV3 = function(v) {
	var f = 1.0 / SGL.lengthV3(v);
	return SGL.mulV3S(v, f);
}

SGL.selfNegV3 = function(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	return v;
}

SGL.selfAddV3 = function(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	return u;
}

SGL.selfAddV3S = function(v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	return v;
}

SGL.selfAddSV3 = function(s, v) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	return v;
}

SGL.selfSubV3 = function(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	return u;
}

SGL.selfSubV3S = function(v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	return v;
}

SGL.selfSubSV3 = function(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	return v;
}

SGL.selfMulV3 = function(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	return u;
}

SGL.selfMulV3S = function(v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	return v;
}

SGL.selfMulSV3 = function(s, v) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	return v;
}

SGL.selfDivV3 = function(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	return u;
}

SGL.selfDivV3S = function(v, s) {
	u[0] /= s;
	u[1] /= s;
	u[2] /= s;
	return u;
}

SGL.selfDivSV3 = function(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	return v;
}

SGL.selfRcpV3 = function(v) {
	return SGL.selfDivSV3(1.0, v);
}

SGL.selfCrossV3 = function(u, v) {
	var t = SGL.v3C(u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]);
	u[0] = t[0];
	u[1] = t[1];
	u[2] = t[2];
	return u;
}

SGL.selfNormalizeV3 = function(v) {
	var f = 1.0 / SGL.lengthV3(v);
	return SGL.selfMulV3S(v, f);
}

SGL.minV3 = function(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1])),
		((u[2] < v[2]) ? (u[2]) : (v[2]))
	];
}

SGL.maxV3 = function(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1])),
		((u[2] > v[2]) ? (u[2]) : (v[2]))
	];
}

SGL.v3toV2 = function(v) {
	return v.slice(0, 2);
}

SGL.v3toV4 = function(v, w) {
	return SGL.v4C(v[0], v[1], v[2], w);
}
/***********************************************************************/


// 4-dimensional vector
/***********************************************************************/
SGL.undefV4 = function(v) {
	return new Array(4);
}

SGL.v4V = function(v) {
	return v.slice(0, 4);
}

SGL.v4S = function(s) {
	return [ s, s, s, s ];
}

SGL.v4C = function(x, y, z, w) {
	return [ x, y, z, w ];
}

SGL.zeroV4 = function() {
	return SGL.v4S(0.0);
}

SGL.oneV4 = function() {
	return SGL.v4S(1.0);
}

SGL.v4 = function() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return SGL.v4V(arguments[0]);
			}
			return SGL.v4S(arguments[0]);
		break;

		case 4:
			return SGL.v4V(arguments);
		break;

		default:
			return SGL.zeroV4();
		break;
	}
	return null;
}

SGL.dupV4 = function(v) {
	return v.slice(0, 4);
}

SGL.negV4 = function(v) {
	return SGL.v4C(-v[0], -v[1], -v[2], -v[3]);
}

SGL.addV4 = function(u, v) {
	return SGL.v4C(u[0]+v[0], u[1]+v[1], u[2]+v[2], u[3]+v[3]);
}

SGL.addV4S = function(v, s) {
	return SGL.v4C(v[0]+s, v[1]+s, v[2]+s, v[3]+s);
}

SGL.addSV4 = function(s, v) {
	return SGL.addV4S(v, s)
}

SGL.subV4 = function(u, v) {
	return SGL.v4C(u[0]-v[0], u[1]-v[1], u[2]-v[2], u[3]-v[3]);
}

SGL.subV4S = function(v, s) {
	return SGL.v4C(v[0]-s, v[1]-s, v[2]-s, v[3]-s);
}

SGL.subSV4 = function(v, s) {
	return SGL.v4C(s-v[0], s-v[1], s-v[2], s-v[3]);
}

SGL.mulV4 = function(u, v) {
	return SGL.v4C(u[0]*v[0], u[1]*v[1], u[2]*v[2], u[3]*v[3]);
}

SGL.mulV4S = function(v, s) {
	return SGL.v4C(v[0]*s, v[1]*s, v[2]*s, v[3]*s);
}

SGL.mulSV4 = function(s, v) {
	return SGL.mulV4S(v, s);
}

SGL.divV4 = function(u, v) {
	return SGL.v4C(u[0]/v[0], u[1]/v[1], u[2]/v[2], u[3]/v[3]);
}

SGL.divV4S = function(v, s) {
	return SGL.v4C(v[0]/s, v[1]/s, v[2]/s, v[3]/s);
}

SGL.divSV4 = function(s, v) {
	return SGL.v4C(s/v[0], s/v[1], s/v[2], s/v[3]);
}

SGL.rcpV4 = function(v) {
	return SGL.divSV4(1.0, v);
}

SGL.dotV4 = function(u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2] + u[3]*v[3]);
}

SGL.sqLengthV4 = function(v) {
	return SGL.dotV4(v, v);
}

SGL.lengthV4 = function(v) {
	return SGL.sqrt(sglSqLengthV4(v));
}

SGL.normalizedV4 = function(v) {
	var f = 1.0 / SGL.lengthV4(v);
	return SGL.mulV4S(v, f);
}

SGL.projectV4 = function(v) {
	var f = 1.0 / v[3];
	return SGL.v4C(v[0]*f, v[1]*f, v[2]*f, 1.0);
}

SGL.selfNegV4 = function(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	v[3] = -v[3];
	return v;
}

SGL.selfAddV4 = function(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	u[3] += v[3];
	return u;
}

SGL.selfAddV4S = function(v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return v;
}

SGL.selfAddSV4 = function(s, v) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return v;
}

SGL.selfSubV4 = function(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	u[3] -= v[3];
	return u;
}

SGL.selfSubV4S = function(v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	v[3] -= s;
	return v;
}

SGL.selfSubSV4 = function(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	v[3] = s - v[3];
	return v;
}

SGL.selfMulV4 = function(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	u[3] *= v[3];
	return u;
}

SGL.selfMulV4S = function(v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return v;
}

SGL.selfMulSV4 = function(s, v) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return v;
}

SGL.selfDivV4 = function(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	u[3] /= v[3];
	return u;
}

SGL.selfDivV4S = function(v, s) {
	u[0] /= s;
	u[1] /= s;
	u[2] /= s;
	u[3] /= s;
	return u;
}

SGL.selfDivSV4 = function(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	v[3] = s / v[3];
	return v;
}

SGL.selfRcpV4 = function(v) {
	return SGL.selfDivSV4(1.0, v);
}

SGL.selfNormalizeV4 = function(v) {
	var f = 1.0 / SGL.lengthV4(v);
	return SGL.selfMulV4S(v, f);
}

SGL.selfProjectV4 = function(v) {
	var f = 1.0 / v[3];
	v[0] *= f;
	v[1] *= f;
	v[2] *= f;
	v[3]  = 1.0;
	return v;
}

SGL.minV4 = function(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1])),
		((u[2] < v[2]) ? (u[2]) : (v[2])),
		((u[3] < v[3]) ? (u[3]) : (v[3]))
	];
}

SGL.maxV4 = function(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1])),
		((u[2] > v[2]) ? (u[2]) : (v[2])),
		((u[3] > v[3]) ? (u[3]) : (v[3]))
	];
}

SGL.v4toV2 = function(v) {
	return v.slice(0, 2);
}

SGL.v4toV3 = function(v) {
	return v.slice(0, 3);
}
/***********************************************************************/


// 4x4 matrix
/***********************************************************************/
SGL.undefM4 = function() {
	return new Array(16);
}

SGL.m4V = function(v) {
	return v.slice(0, 16);
}

SGL.m4S = function(s) {
	var m = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		m[i] = s;
	}
	return m;
}

SGL.diagM4V = function(d) {
	var m = SGL.m4S(0.0);
	m[ 0] = d[0];
	m[ 5] = d[1];
	m[10] = d[2];
	m[15] = d[3];
	return m;
}

SGL.diagM4S = function(s) {
	var m = SGL.m4S(0.0);
	m[ 0] = s;
	m[ 5] = s;
	m[10] = s;
	m[15] = s;
	return m;
}

SGL.diagM4C = function(m00, m11, m22, m33) {
	return SGL.diagM4V(arguments);
}

SGL.zeroM4 = function() {
	return SGL.m4S(0.0);
}

SGL.oneM4 = function() {
	return SGL.m4S(1.0);
}

SGL.identityM4 = function() {
	return SGL.diagM4S(1.0);
}

SGL.m4 = function() {
	var n = arguments.length;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				switch (arguments[0].length) {
					case 1:
						return SGL.diagM4S(arguments[0]);
					break;
					case 4:
						return SGL.diagM4V(arguments[0]);
					break;
					case 16:
						return SGL.m4V(arguments[0]);
					break;
					default:
						return SGL.identityM4();
					break;
				}
			}
			return SGL.m4S(arguments[0]);
		break;

		case 4:
			return SGL.diagM4V(arguments);
		break;

		case 16:
			return SGL.m4V(arguments);
		break;

		default:
			return SGL.identityM4();
		break;
	}
	return null;
}

SGL.dupM4 = function(m) {
	var r = SGL.undefM4();

	r[ 0] = m[ 0];
	r[ 1] = m[ 1];
	r[ 2] = m[ 2];
	r[ 3] = m[ 3];

	r[ 4] = m[ 4];
	r[ 5] = m[ 5];
	r[ 6] = m[ 6];
	r[ 7] = m[ 7];

	r[ 8] = m[ 8];
	r[ 9] = m[ 9];
	r[10] = m[10];
	r[11] = m[11];

	r[12] = m[12];
	r[13] = m[13];
	r[14] = m[14];
	r[15] = m[15];

	return r;

	//return m.slice(0, 16);
	//return m.slice();
}

SGL.getElemM4 = function(m, row, col) {
	return m[row+col*4];
}

SGL.setElemM4 = function(m, row, col, value) {
	m[row+col*4] = value;
}

SGL.getRowM4 = function(m, r) {
	return SGL.v4C(m[r+0], m[r+4], m[r+8], m[r+12]);
}

SGL.setRowM4V = function(m, r, v) {
	m[r+ 0] = v[0];
	m[r+ 4] = v[1];
	m[r+ 8] = v[2];
	m[r+12] = v[3];
}

SGL.setRowM4S = function(m, r, s) {
	m[r+ 0] = s;
	m[r+ 4] = s;
	m[r+ 8] = s;
	m[r+12] = s;
}

SGL.setRowM4C = function(m, r, x, y, z, w) {
	m[r+ 0] = x;
	m[r+ 4] = y;
	m[r+ 8] = z;
	m[r+12] = w;
}

SGL.getColM4 = function(m, c) {
	var i = c * 4;
	return SGL.v4C(m[i+0], m[i+1], m[i+2], m[i+3]);
}

SGL.setColM4V = function(m, c, v) {
	var i = c * 4;
	m[i+0] = v[0];
	m[i+1] = v[1];
	m[i+2] = v[2];
	m[i+3] = v[3];
}

SGL.setColM4S = function(m, c, s) {
	var i = c * 4;
	m[i+0] = s;
	m[i+1] = s;
	m[i+2] = s;
	m[i+3] = s;
}

SGL.setColM4C = function(m, c, x, y, z, w) {
	var i = c * 4;
	m[i+0] = x;
	m[i+1] = y;
	m[i+2] = z;
	m[i+3] = w;
}

SGL.m4toM3 = function(m) {
	return [
		m[ 0], m[ 1], m[ 2],
		m[ 4], m[ 5], m[ 6],
		m[ 8], m[ 9], m[10]
	];
}

SGL.isIdentityM4 = function(m) {
	var i = 0;
	var j = 0;
	var s = 0.0;
	for (i=0; i<4; ++i) {
		for (j=0; j<4; ++j) {
			s = m[i+j*4];
			if ((i == j)) {
				if (s != 1.0) {
					return false;
				}
			}
			else {
				if (s != 0.0) {
					return false;
				}
			}
		}
	}
	return true;
}

SGL.negM4 = function(m) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = -m[i];
	}
	return r;
}

SGL.addM4 = function(a, b) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] + b[i];
	}
	return r;
}

SGL.addM4S = function(m, s)
{
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] + s;
	}
	return r;
}

SGL.addSM4 = function(s, m) {
	return SGL.addM4S(m, s);
}

SGL.subM4 = function(a, b) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] - b[i];
	}
	return r;
}

SGL.subM4S = function(m, s) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] - s;
	}
	return r;
}

SGL.subSM4 = function(s, m) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = s - m[i];
	}
	return r;
}

SGL.mulM4 = function(a, b) {
	var a0  = a[ 0], a1  = a[ 1],  a2 = a[ 2], a3  = a[ 3],
	    a4  = a[ 4], a5  = a[ 5],  a6 = a[ 6], a7  = a[ 7],
	    a8  = a[ 8], a9  = a[ 9], a10 = a[10], a11 = a[11],
	    a12 = a[12], a13 = a[13], a14 = a[14], a15 = a[15],

	    b0  = b[ 0], b1  = b[ 1], b2  = b[ 2], b3  = b[ 3],
	    b4  = b[ 4], b5  = b[ 5], b6  = b[ 6], b7  = b[ 7],
	    b8  = b[ 8], b9  = b[ 9], b10 = b[10], b11 = b[11],
	    b12 = b[12], b13 = b[13], b14 = b[14], b15 = b[15];

	var r = SGL.undefM4();

	r[ 0] = a0*b0 + a4*b1 + a8*b2  + a12*b3;
	r[ 1] = a1*b0 + a5*b1 + a9*b2  + a13*b3;
	r[ 2] = a2*b0 + a6*b1 + a10*b2 + a14*b3;
	r[ 3] = a3*b0 + a7*b1 + a11*b2 + a15*b3;

	r[ 4] = a0*b4 + a4*b5 + a8*b6  + a12*b7;
	r[ 5] = a1*b4 + a5*b5 + a9*b6  + a13*b7;
	r[ 6] = a2*b4 + a6*b5 + a10*b6 + a14*b7;
	r[ 7] = a3*b4 + a7*b5 + a11*b6 + a15*b7;

	r[ 8] = a0*b8 + a4*b9 + a8*b10  + a12*b11;
	r[ 9] = a1*b8 + a5*b9 + a9*b10  + a13*b11;
	r[10] = a2*b8 + a6*b9 + a10*b10 + a14*b11;
	r[11] = a3*b8 + a7*b9 + a11*b10 + a15*b11;

	r[12] = a0*b12 + a4*b13 + a8*b14  + a12*b15;
	r[13] = a1*b12 + a5*b13 + a9*b14  + a13*b15;
	r[14] = a2*b12 + a6*b13 + a10*b14 + a14*b15;
	r[15] = a3*b12 + a7*b13 + a11*b14 + a15*b15;

	/*
	r[ 0] = a[ 0]*b[ 0] + a[ 4]*b[ 1] + a[ 8]*b[ 2] + a[12]*b[ 3];
	r[ 1] = a[ 1]*b[ 0] + a[ 5]*b[ 1] + a[ 9]*b[ 2] + a[13]*b[ 3];
	r[ 2] = a[ 2]*b[ 0] + a[ 6]*b[ 1] + a[10]*b[ 2] + a[14]*b[ 3];
	r[ 3] = a[ 3]*b[ 0] + a[ 7]*b[ 1] + a[11]*b[ 2] + a[15]*b[ 3];

	r[ 4] = a[ 0]*b[ 4] + a[ 4]*b[ 5] + a[ 8]*b[ 6] + a[12]*b[ 7];
	r[ 5] = a[ 1]*b[ 4] + a[ 5]*b[ 5] + a[ 9]*b[ 6] + a[13]*b[ 7];
	r[ 6] = a[ 2]*b[ 4] + a[ 6]*b[ 5] + a[10]*b[ 6] + a[14]*b[ 7];
	r[ 7] = a[ 3]*b[ 4] + a[ 7]*b[ 5] + a[11]*b[ 6] + a[15]*b[ 7];

	r[ 8] = a[ 0]*b[ 8] + a[ 4]*b[ 9] + a[ 8]*b[10] + a[12]*b[11];
	r[ 9] = a[ 1]*b[ 8] + a[ 5]*b[ 9] + a[ 9]*b[10] + a[13]*b[11];
	r[10] = a[ 2]*b[ 8] + a[ 6]*b[ 9] + a[10]*b[10] + a[14]*b[11];
	r[11] = a[ 3]*b[ 8] + a[ 7]*b[ 9] + a[11]*b[10] + a[15]*b[11];

	r[12] = a[ 0]*b[12] + a[ 4]*b[13] + a[ 8]*b[14] + a[12]*b[15];
	r[13] = a[ 1]*b[12] + a[ 5]*b[13] + a[ 9]*b[14] + a[13]*b[15];
	r[14] = a[ 2]*b[12] + a[ 6]*b[13] + a[10]*b[14] + a[14]*b[15];
	r[15] = a[ 3]*b[12] + a[ 7]*b[13] + a[11]*b[14] + a[15]*b[15];
	*/

	return r;

	/*
	var r = SGL.undefM4();

	r[ 0] = a[ 0]*b[ 0] + a[ 4]*b[ 1] + a[ 8]*b[ 2] + a[12]*b[ 3];
	r[ 1] = a[ 1]*b[ 0] + a[ 5]*b[ 1] + a[ 9]*b[ 2] + a[13]*b[ 3];
	r[ 2] = a[ 2]*b[ 0] + a[ 6]*b[ 1] + a[10]*b[ 2] + a[14]*b[ 3];
	r[ 3] = a[ 3]*b[ 0] + a[ 7]*b[ 1] + a[11]*b[ 2] + a[15]*b[ 3];

	r[ 4] = a[ 0]*b[ 4] + a[ 4]*b[ 5] + a[ 8]*b[ 6] + a[12]*b[ 7];
	r[ 5] = a[ 1]*b[ 4] + a[ 5]*b[ 5] + a[ 9]*b[ 6] + a[13]*b[ 7];
	r[ 6] = a[ 2]*b[ 4] + a[ 6]*b[ 5] + a[10]*b[ 6] + a[14]*b[ 7];
	r[ 7] = a[ 3]*b[ 4] + a[ 7]*b[ 5] + a[11]*b[ 6] + a[15]*b[ 7];

	r[ 8] = a[ 0]*b[ 8] + a[ 4]*b[ 9] + a[ 8]*b[10] + a[12]*b[11];
	r[ 9] = a[ 1]*b[ 8] + a[ 5]*b[ 9] + a[ 9]*b[10] + a[13]*b[11];
	r[10] = a[ 2]*b[ 8] + a[ 6]*b[ 9] + a[10]*b[10] + a[14]*b[11];
	r[11] = a[ 3]*b[ 8] + a[ 7]*b[ 9] + a[11]*b[10] + a[15]*b[11];

	r[12] = a[ 0]*b[12] + a[ 4]*b[13] + a[ 8]*b[14] + a[12]*b[15];
	r[13] = a[ 1]*b[12] + a[ 5]*b[13] + a[ 9]*b[14] + a[13]*b[15];
	r[14] = a[ 2]*b[12] + a[ 6]*b[13] + a[10]*b[14] + a[14]*b[15];
	r[15] = a[ 3]*b[12] + a[ 7]*b[13] + a[11]*b[14] + a[15]*b[15];

	return r;
	*/
}

SGL.mulM4S = function(m, s) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] * s;
	}
	return r;
}

SGL.mulSM4 = function(s, m) {
	return SGL.mulM4S(m, s);
}

SGL.mulM4V3 = function(m, v, w) {
	return [
		m[ 0] * v[0] + m[ 4] * v[1] + m[ 8] * v[2] + m[12] * w,
		m[ 1] * v[0] + m[ 5] * v[1] + m[ 9] * v[2] + m[13] * w,
		m[ 2] * v[0] + m[ 6] * v[1] + m[10] * v[2] + m[14] * w /* ,
		m[ 3] * v[0] + m[ 7] * v[1] + m[11] * v[2] + m[15] * w*/
	];
}

SGL.mulM4V4 = function(m, v) {
	return [
		m[ 0] * v[0] + m[ 4] * v[1] + m[ 8] * v[2] + m[12] * v[3],
		m[ 1] * v[0] + m[ 5] * v[1] + m[ 9] * v[2] + m[13] * v[3],
		m[ 2] * v[0] + m[ 6] * v[1] + m[10] * v[2] + m[14] * v[3],
		m[ 3] * v[0] + m[ 7] * v[1] + m[11] * v[2] + m[15] * v[3]
	];
}

SGL.divM4S = function(m, s) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] / s;
	}
	return r;
}

SGL.divSM4 = function(s, m) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = s / m[i];
	}
	return r;
}

SGL.rcpM4 = function(m) {
	return SGL.divSM4(1.0, m);
}

SGL.compMulM4 = function(a, b) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] * b[i];
	}
	return r;
}

SGL.compDivM4 = function(a, b) {
	var r = SGL.undefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] / b[i];
	}
	return r;
}

SGL.transposeM4 = function(m) {
	var r = SGL.undefM4();

	r[ 0] = m[ 0];
	r[ 1] = m[ 4];
	r[ 2] = m[ 8];
	r[ 3] = m[12];
	
	r[ 4] = m[ 1];
	r[ 5] = m[ 5];
	r[ 6] = m[ 9];
	r[ 7] = m[13];
	
	r[ 8] = m[ 2];
	r[ 9] = m[ 6];
	r[10] = m[10];
	r[11] = m[14];
	
	r[12] = m[ 3];
	r[13] = m[ 7];
	r[14] = m[11];
	r[15] = m[15];
	
	return r;
}

SGL.determinantM4 = function(m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15]

	return (
		m12 * m9 * m6 * m3 - m8 * m13 * m6 * m3 - m12 * m5 * m10 * m3 + m4 * m13 * m10 * m3 +
		m8 * m5 * m14 * m3 - m4 * m9 * m14 * m3 - m12 * m9 * m2 * m7 + m8 * m13 * m2 * m7 +
		m12 * m1 * m10 * m7 - m0 * m13 * m10 * m7 - m8 * m1 * m14 * m7 + m0 * m9 * m14 * m7 +
		m12 * m5 * m2 * m11 - m4 * m13 * m2 * m11 - m12 * m1 * m6 * m11 + m0 * m13 * m6 * m11 +
		m4 * m1 * m14 * m11 - m0 * m5 * m14 * m11 - m8 * m5 * m2 * m15 + m4 * m9 * m2 * m15 +
		m8 * m1 * m6 * m15 - m0 * m9 * m6 * m15 - m4 * m1 * m10 * m15 + m0 * m5 * m10 * m15
	);
}

SGL.inverseM4 = function(m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15]

	var t = SGL.undefM4();

	t[ 0] = (m9*m14*m7-m13*m10*m7+m13*m6*m11-m5*m14*m11-m9*m6*m15+m5*m10*m15);
	t[ 1] = (m13*m10*m3-m9*m14*m3-m13*m2*m11+m1*m14*m11+m9*m2*m15-m1*m10*m15);
	t[ 2] = (m5*m14*m3-m13*m6*m3+m13*m2*m7-m1*m14*m7-m5*m2*m15+m1*m6*m15);
	t[ 3] = (m9*m6*m3-m5*m10*m3-m9*m2*m7+m1*m10*m7+m5*m2*m11-m1*m6*m11);

	t[ 4] = (m12*m10*m7-m8*m14*m7-m12*m6*m11+m4*m14*m11+m8*m6*m15-m4*m10*m15);
	t[ 5] = (m8*m14*m3-m12*m10*m3+m12*m2*m11-m0*m14*m11-m8*m2*m15+m0*m10*m15);
	t[ 6] = (m12*m6*m3-m4*m14*m3-m12*m2*m7+m0*m14*m7+m4*m2*m15-m0*m6*m15);
	t[ 7] = (m4*m10*m3-m8*m6*m3+m8*m2*m7-m0*m10*m7-m4*m2*m11+m0*m6*m11);

	t[ 8] = (m8*m13*m7-m12*m9*m7+m12*m5*m11-m4*m13*m11-m8*m5*m15+m4*m9*m15);
	t[ 9] = (m12*m9*m3-m8*m13*m3-m12*m1*m11+m0*m13*m11+m8*m1*m15-m0*m9*m15);
	t[10] = (m4*m13*m3-m12*m5*m3+m12*m1*m7-m0*m13*m7-m4*m1*m15+m0*m5*m15);
	t[11] = (m8*m5*m3-m4*m9*m3-m8*m1*m7+m0*m9*m7+m4*m1*m11-m0*m5*m11);

	t[12] = (m12*m9*m6-m8*m13*m6-m12*m5*m10+m4*m13*m10+m8*m5*m14-m4*m9*m14);
	t[13] = (m8*m13*m2-m12*m9*m2+m12*m1*m10-m0*m13*m10-m8*m1*m14+m0*m9*m14);
	t[14] = (m12*m5*m2-m4*m13*m2-m12*m1*m6+m0*m13*m6+m4*m1*m14-m0*m5*m14);
	t[15] = (m4*m9*m2-m8*m5*m2+m8*m1*m6-m0*m9*m6-m4*m1*m10+m0*m5*m10);

	var s = 1.0 / (
		m12 * m9 * m6 * m3 - m8 * m13 * m6 * m3 - m12 * m5 * m10 * m3 + m4 * m13 * m10 * m3 +
		m8 * m5 * m14 * m3 - m4 * m9 * m14 * m3 - m12 * m9 * m2 * m7 + m8 * m13 * m2 * m7 +
		m12 * m1 * m10 * m7 - m0 * m13 * m10 * m7 - m8 * m1 * m14 * m7 + m0 * m9 * m14 * m7 +
		m12 * m5 * m2 * m11 - m4 * m13 * m2 * m11 - m12 * m1 * m6 * m11 + m0 * m13 * m6 * m11 +
		m4 * m1 * m14 * m11 - m0 * m5 * m14 * m11 - m8 * m5 * m2 * m15 + m4 * m9 * m2 * m15 +
		m8 * m1 * m6 * m15 - m0 * m9 * m6 * m15 - m4 * m1 * m10 * m15 + m0 * m5 * m10 * m15
	);

	for (var i=0; i<16; ++i) t[i] *= s;

	return t;
}

SGL.traceM4 = function(m) {
	return (m[0] + m[5] + m[10] + m[15]);
}

SGL.translationM4V = function(v) {
	var m = SGL.identityM4();
	m[12] = v[0];
	m[13] = v[1];
	m[14] = v[2];
	return m;
}

SGL.translationM4C = function(x, y, z) {
	return SGL.translationM4V([x, y, z]);
}

SGL.translationM4S = function(s) {
	return SGL.translationM4C(s, s, s);
}

SGL.rotationAngleAxisM4V = function(angleRad, axis) {
	var ax = SGL.normalizedV3(axis);
	var s  = SGL.sin(angleRad);
	var c  = SGL.cos(angleRad);
	var q   = 1.0 - c;

	var x = ax[0];
	var y = ax[1];
	var z = ax[2];

	var xx, yy, zz, xy, yz, zx, xs, ys, zs;

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;

	var m = SGL.undefM4();

	m[ 0] = (q * xx) + c;
	m[ 1] = (q * xy) + zs;
	m[ 2] = (q * zx) - ys;
	m[ 3] = 0.0;

	m[ 4] = (q * xy) - zs;
	m[ 5] = (q * yy) + c;
	m[ 6] = (q * yz) + xs;
	m[ 7] = 0.0;

	m[ 8] = (q * zx) + ys;
	m[ 9] = (q * yz) - xs;
	m[10] = (q * zz) + c;
	m[11] = 0.0;

	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;

	return m;
}

SGL.rotationAngleAxisM4C = function(angleRad, ax, ay, az) {
	return SGL.rotationAngleAxisM4V(angleRad, [ax, ay, az]);
}

SGL.scalingM4V = function(v) {
	var m = SGL.identityM4();
	m[ 0] = v[0];
	m[ 5] = v[1];
	m[10] = v[2];
	return m;
}

SGL.scalingM4C = function(x, y, z) {
	return SGL.scalingM4V([x, y, z]);
}

SGL.scalingM4S = function(s) {
	return SGL.scalingM4C(s, s, s);
}

SGL.lookAtM4V = function(position, target, up) {
	var v = SGL.normalizedV3(sglSubV3(target, position));
	var u = SGL.normalizedV3(up);
	var s = SGL.normalizedV3(sglCrossV3(v, u));

	u = SGL.normalizedV3(sglCrossV3(s, v));

	var m = SGL.undefM4();

	m[ 0] =  s[0];
	m[ 1] =  u[0];
	m[ 2] = -v[0];
	m[ 3] =   0.0;

	m[ 4] =  s[1];
	m[ 5] =  u[1];
	m[ 6] = -v[1];
	m[ 7] =   0.0;

	m[ 8] =  s[2];
	m[ 9] =  u[2];
	m[10] = -v[2];
	m[11] =   0.0;

	m[12] =   0.0;
	m[13] =   0.0;
	m[14] =   0.0;
	m[15] =   1.0;

	m = SGL.mulM4(m, SGL.translationM4V(sglNegV3(position)));

	return m;
}

SGL.lookAtM4C = function(positionX, positionY, positionZ, targetX, targetY, targetZ, upX, upY, upZ) {
	return SGL.lookAtM4V([positionX, positionY, positionZ], [targetX, targetY, targetZ], [upX, upY, upZ]);
}

SGL.orthoM4V = function(omin, omax) {
	var sum   = SGL.addV3(omax, omin);
	var dif   = SGL.subV3(omax, omin);

	var m = SGL.undefM4();

	m[ 0] =      2.0 / dif[0];
	m[ 1] =               0.0;
	m[ 2] =               0.0;
	m[ 3] =               0.0;

	m[ 4] =               0.0;
	m[ 5] =      2.0 / dif[1];
	m[ 6] =               0.0;
	m[ 7] =               0.0;

	m[ 8] =               0.0;
	m[ 9] =               0.0;
	m[10] =     -2.0 / dif[2];
	m[11] =                 0.0;

	m[12] =  -sum[0] / dif[0];
	m[13] =  -sum[1] / dif[1];
	m[14] =  -sum[2] / dif[2];
	m[15] =               1.0;

	return m;
}

SGL.orthoM4C = function(left, right, bottom, top, zNear, zFar) {
	return SGL.orthoM4V([left, bottom, zNear], [right, top, zFar]);
}

SGL.frustumM4V = function(fmin, fmax) {
	var sum   = SGL.addV3(fmax, fmin);
	var dif   = SGL.subV3(fmax, fmin);
	var t     = 2.0 * fmin[2];

	var m = SGL.undefM4();
	
	m[ 0] =            t / dif[0];
	m[ 1] =                   0.0;
	m[ 2] =                   0.0;
	m[ 3] =                   0.0;

	m[ 4] =                   0.0;
	m[ 5] =            t / dif[1];
	m[ 6] =                   0.0;
	m[ 7] =                   0.0;

	m[ 8] =       sum[0] / dif[0];
	m[ 9] =       sum[1] / dif[1];
	m[10] =      -sum[2] / dif[2];
	m[11] =                  -1.0;

	m[12] =                   0.0;
	m[13] =                   0.0;
	m[14] = -t * fmax[2] / dif[2];
	m[15] =                   0.0;

	return m;
}

SGL.frustumM4C = function(left, right, bottom, top, zNear, zFar) {
	return SGL.frustumM4V([left, bottom, zNear], [right, top, zFar]);
}

SGL.perspectiveM4 = function(fovYRad, aspectRatio, zNear, zFar) {
	var pmin = SGL.undefV4();
	var pmax = SGL.undefV4();

	pmin[2] = zNear;
	pmax[2] = zFar;

	pmax[1] = pmin[2] * SGL.tan(fovYRad / 2.0);
	pmin[1] = -pmax[1];

	pmax[0] = pmax[1] * aspectRatio;
	pmin[0] = -pmax[0];

	return SGL.frustumM4V(pmin, pmax);
}
/***********************************************************************/


// quaternion
/***********************************************************************/
SGL.undefQuat = function(v) {
	return new Array(4);
}

SGL.quatV = function(v) {
	return v.slice(0, 4);
}

SGL.identityQuat = function() {
	return [ 0.0, 0.0, 0.0, 1.0 ];
}

SGL.angleAxisQuat = function(angleRad, axis) {
	var halfAngle = angleRad / 2.0;
	var fsin = SGL.sin(halfAngle);
	return [
		fsin * axis[0],
		fsin * axis[1],
		fsin * axis[2], SGL.cos(halfAngle)
	];
}

SGL.m4Quat = function(m) {
	var trace = SGL.getElemM4(m, 0, 0) + SGL.getElemM4(m, 1, 1) + SGL.getElemM4(m, 2, 2);
	var root = null;
	var q = SGL.undefQuat();

	if (trace > 0.0) {
		root = SGL.sqrt(trace + 1.0);
		q[3] = root / 2.0;
		root = 0.5 / root;
		q[0] = (sglGetElemM4(m, 2, 1) - SGL.getElemM4(m, 1, 2)) * root;
		q[1] = (sglGetElemM4(m, 0, 2) - SGL.getElemM4(m, 2, 0)) * root;
		q[2] = (sglGetElemM4(m, 1, 0) - SGL.getElemM4(m, 0, 1)) * root;
	}
	else {
		var i = 0;

		if (sglGetElemM4(m, 1, 1) > SGL.getElemM4(m, 0, 0)) i = 1;
		if (sglGetElemM4(m, 2, 2) > SGL.getElemM4(m, i, i)) i = 2;

		var j = (i + 1) % 3;
		var k = (j + 1) % 3;

		root = SGL.sqrt(sglGetElemM4(m, i, i) - SGL.getElemM4(m, j, j) - SGL.getElemM4(m, k, k) + 1.0);

		q[i] = root / 2.0;
		root = 0.5 / root;
		q[3] = (sglGetElemM4(m, k, j) - SGL.getElemM4(m, j, k)) * root;
		q[j] = (sglGetElemM4(m, j, i) + SGL.getElemM4(m, i, j)) * root;
		q[k] = (sglGetElemM4(m, k, i) + SGL.getElemM4(m, i, k)) * root;
	}
	return q;
}

SGL.getQuatAngleAxis = function(q) {
	var v = new Array(4);

	var sqLen = SGL.sqLengthV4(q);
	var angle = null;

	if (sqLen > 0.0) {
		var invLen = 1.0 / SGL.sqrt(sqLen);
		v[0] = q[0] * invLen;
		v[1] = q[1] * invLen;
		v[2] = q[2] * invLen;
		v[3] = 2.0 * aglAcos(q[3]);
	}
	else
	{
		v[0] = 0.0;
		v[1] = 0.0;
		v[2] = 1.0;
		v[3] = 0.0;
	}

	return v;
}

SGL.getQuatRotationM4 = function(q) {
	var tx  = 2.0 * q[0];
	var ty  = 2.0 * q[1];
	var tz  = 2.0 * q[2];
	var twx = tx * q[3];
	var twy = ty * q[3];
	var twz = tz * q[3];
	var txx = tx * q[0];
	var txy = ty * q[0];
	var txz = tz * q[0];
	var tyy = ty * q[1];
	var tyz = tz * q[1];
	var tzz = tz * q[2];

	var m = SGL.identityM4(); SGL.setElemM4(m, 0, 0, 1.0 - (tyy + tzz)); SGL.setElemM4(m, 0, 1, txy - twz); SGL.setElemM4(m, 0, 2, txz + twy); SGL.setElemM4(m, 1, 0, txy + twz); SGL.setElemM4(m, 1, 1, 1.0 - (txx + tzz)); SGL.setElemM4(m, 1, 2, tyz - twx); SGL.setElemM4(m, 2, 0, txz - twy); SGL.setElemM4(m, 2, 1, tyz + twx); SGL.setElemM4(m, 2, 2, 1.0 - (txx + tyy));

	return m;
}

SGL.normalizedQuat = function(q) {
	return SGL.normalizedV4(q);
}

SGL.mulQuat = function(q1, q2) {
	var r = SGL.undefQuat();

	r[0] = p[3] * q[0] + p[0] * q[3] + p[1] * q[2] - p[2] * q[1];
	r[1] = p[3] * q[1] + p[1] * q[3] + p[2] * q[0] - p[0] * q[2];
	r[2] = p[3] * q[2] + p[2] * q[3] + p[0] * q[1] - p[1] * q[0];
	r[3] = p[3] * q[3] - p[0] * q[0] - p[1] * q[1] - p[2] * q[2];
}
/***********************************************************************/
