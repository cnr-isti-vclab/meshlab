
function sglUndefV2(v) {
	return new Array(2);
}

function sglV2V(v) {
	return v.slice(0, 2);
}

function sglV2S(s) {
	return [ s, s ];
}

function sglV2C(x, y) {
	return [ x, y ];
}

function sglZeroV2() {
	return sglV2S(0.0);
}

function sglOneV2() {
	return sglV2S(1.0);
}

function sglV2() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return sglV2V(arguments[0]);
			}
			return sglV2S(arguments[0]);
		break;

		case 2:
			return sglV2V(arguments);
		break;

		default:
			return sglZeroV2();
		break;
	}
	return null;
}

function sglDupV2(v) {
	return v.slice(0, 2);
}

function sglNegV2(v) {
	return sglV2C(-v[0], -v[1]);
}

function sglAddV2(u, v) {
	return sglV2C(u[0]+v[0], u[1]+v[1]);
}

function sglAddV2S(v, s) {
	return sglV2C(v[0]+s, v[1]+s);
}

function sglAddSV2(s, v) {
	return sglAddV2S(v, s)
}

function sglSubV2(u, v) {
	return sglV2C(u[0]-v[0], u[1]-v[1]);
}

function sglSubV2S(v, s) {
	return sglV2C(v[0]-s, v[1]-s);
}

function sglSubSV2(v, s) {
	return sglV2C(s-v[0], s-v[1]);
}

function sglMulV2(u, v) {
	return sglV2C(u[0]*v[0], u[1]*v[1]);
}

function sglMulV2S(v, s) {
	return sglV2C(v[0]*s, v[1]*s);
}

function sglMulSV2(s, v) {
	return sglMulV2S(v, s);
}

function sglDivV2(u, v) {
	return sglV2C(u[0]/v[0], u[1]/v[1]);
}

function sglDivV2S(v, s) {
	return sglV2C(v[0]/s, v[1]/s);
}

function sglDivSV2(s, v) {
	return sglV2C(s/v[0], s/v[1]);
}

function sglRcpV2(v) {
	return sglDivSV2(1.0, v);
}

function sglDotV2(u, v) {
	return (u[0]*v[0] + u[1]*v[1]);
}

function sglCrossV2(u, v) {
	return (u[0]*v[1] - u[1]*v[0]);
}

function sglSqLengthV2(v) {
	return sglDotV2(v, v);
}

function sglLengthV2(v) {
	return sglSqrt(sglSqLengthV2(v));
}

function sglNormalizedV2(v) {
	var f = 1.0 / sglLengthV2(v);
	return sglMulV2S(v, f);
}

function sglSelfNegV2(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	return v;
}

function sglSelfAddV2(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	return u;
}

function sglSelfAddV2S(v, s) {
	v[0] += s;
	v[1] += s;
	return v;
}

function sglSelfAddSV2(s, v) {
	v[0] += s;
	v[1] += s;
	return v;
}

function sglSelfSubV2(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	return u;
}

function sglSelfSubV2S(v, s) {
	v[0] -= s;
	v[1] -= s;
	return v;
}

function sglSelfSubSV2(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	return v;
}

function sglSelfMulV2(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	return u;
}

function sglSelfMulV2S(v, s) {
	v[0] *= s;
	v[1] *= s;
	return v;
}

function sglSelfMulSV2(s, v) {
	v[0] *= s;
	v[1] *= s;
	return v;
}

function sglSelfDivV2(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	return u;
}

function sglSelfDivV2S(v, s) {
	u[0] /= s;
	u[1] /= s;
	return u;
}

function sglSelfDivSV2(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	return v;
}

function sglSelfRcpV2(v) {
	return sglSelfDivSV2(1.0, v);
}

function sglSelfNormalizeV2(v) {
	var f = 1.0 / sglLengthV2(v);
	return sglSelfMulV2S(v, f);
}

function sglMinV2(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1]))
	];
}

function sglMaxV2(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1]))
	];
}

function sglV2toV3(v, z) {
	return sglV3C(v[0], v[1], z);
}

function sglV2toV4(v, z, w) {
	return sglV4C(v[0], v[1], z, w);
}
/***********************************************************************/


// 3-dimensional vector
/***********************************************************************/
function sglUndefV3(v) {
	return new Array(3);
}

function sglV3V(v) {
	return v.slice(0, 3);
}

function sglV3S(s) {
	return [ s, s, s ];
}

function sglV3C(x, y, z) {
	return [ x, y, z ];
}

function sglZeroV3() {
	return sglV3S(0.0);
}

function sglOneV3() {
	return sglV3S(1.0);
}

function sglV3() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return sglV3V(arguments[0]);
			}
			return sglV3S(arguments[0]);
		break;

		case 3:
			return sglV3V(arguments);
		break;

		default:
			return sglZeroV3();
		break;
	}
	return null;
}

function sglDupV3(v) {
	return v.slice(0, 3);
}

function sglNegV3(v) {
	return sglV3C(-v[0], -v[1], -v[2]);
}

function sglAddV3(u, v) {
	return sglV3C(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

function sglAddV3S(v, s) {
	return sglV3C(v[0]+s, v[1]+s, v[2]+s);
}

function sglAddSV3(s, v) {
	return sglAddV3S(v, s)
}

function sglSubV3(u, v) {
	return sglV3C(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

function sglSubV3S(v, s) {
	return sglV3C(v[0]-s, v[1]-s, v[2]-s);
}

function sglSubSV3(v, s) {
	return sglV3C(s-v[0], s-v[1], s-v[2]);
}

function sglMulV3(u, v) {
	return sglV3C(u[0]*v[0], u[1]*v[1], u[2]*v[2]);
}

function sglMulV3S(v, s) {
	return sglV3C(v[0]*s, v[1]*s, v[2]*s);
}

function sglMulSV3(s, v) {
	return sglMulV3S(v, s);
}

function sglDivV3(u, v) {
	return sglV3C(u[0]/v[0], u[1]/v[1], u[2]/v[2]);
}

function sglDivV3S(v, s) {
	return sglV3C(v[0]/s, v[1]/s, v[2]/s);
}

function sglDivSV3(s, v) {
	return sglV3C(s/v[0], s/v[1], s/v[2]);
}

function sglRcpV3(v) {
	return sglDivSV3(1.0, v);
}

function sglDotV3(u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2]);
}

function sglCrossV3(u, v) {
	return sglV3C(u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]);
}

function sglSqLengthV3(v) {
	return sglDotV3(v, v);
}

function sglLengthV3(v) {
	return sglSqrt(sglSqLengthV3(v));
}

function sglNormalizedV3(v) {
	var f = 1.0 / sglLengthV3(v);
	return sglMulV3S(v, f);
}

function sglSelfNegV3(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	return v;
}

function sglSelfAddV3(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	return u;
}

function sglSelfAddV3S(v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	return v;
}

function sglSelfAddSV3(s, v) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	return v;
}

function sglSelfSubV3(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	return u;
}

function sglSelfSubV3S(v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	return v;
}

function sglSelfSubSV3(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	return v;
}

function sglSelfMulV3(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	return u;
}

function sglSelfMulV3S(v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	return v;
}

function sglSelfMulSV3(s, v) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	return v;
}

function sglSelfDivV3(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	return u;
}

function sglSelfDivV3S(v, s) {
	u[0] /= s;
	u[1] /= s;
	u[2] /= s;
	return u;
}

function sglSelfDivSV3(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	return v;
}

function sglSelfRcpV3(v) {
	return sglSelfDivSV3(1.0, v);
}

function sglSelfCrossV3(u, v) {
	var t = sglV3C(u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]);
	u[0] = t[0];
	u[1] = t[1];
	u[2] = t[2];
	return u;
}

function sglSelfNormalizeV3(v) {
	var f = 1.0 / sglLengthV3(v);
	return sglSelfMulV3S(v, f);
}

function sglMinV3(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1])),
		((u[2] < v[2]) ? (u[2]) : (v[2]))
	];
}

function sglMaxV3(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1])),
		((u[2] > v[2]) ? (u[2]) : (v[2]))
	];
}

function sglV3toV2(v) {
	return v.slice(0, 2);
}

function sglV3toV4(v, w) {
	return sglV4C(v[0], v[1], v[2], w);
}
/***********************************************************************/


// 4-dimensional vector
/***********************************************************************/
function sglUndefV4(v) {
	return new Array(4);
}

function sglV4V(v) {
	return v.slice(0, 4);
}

function sglV4S(s) {
	return [ s, s, s, s ];
}

function sglV4C(x, y, z, w) {
	return [ x, y, z, w ];
}

function sglZeroV4() {
	return sglV4S(0.0);
}

function sglOneV4() {
	return sglV4S(1.0);
}

function sglV4() {
	var n = arguments.length;
	var s;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				return sglV4V(arguments[0]);
			}
			return sglV4S(arguments[0]);
		break;

		case 4:
			return sglV4V(arguments);
		break;

		default:
			return sglZeroV4();
		break;
	}
	return null;
}

function sglDupV4(v) {
	return v.slice(0, 4);
}

function sglNegV4(v) {
	return sglV4C(-v[0], -v[1], -v[2], -v[3]);
}

function sglAddV4(u, v) {
	return sglV4C(u[0]+v[0], u[1]+v[1], u[2]+v[2], u[3]+v[3]);
}

function sglAddV4S(v, s) {
	return sglV4C(v[0]+s, v[1]+s, v[2]+s, v[3]+s);
}

function sglAddSV4(s, v) {
	return sglAddV4S(v, s)
}

function sglSubV4(u, v) {
	return sglV4C(u[0]-v[0], u[1]-v[1], u[2]-v[2], u[3]-v[3]);
}

function sglSubV4S(v, s) {
	return sglV4C(v[0]-s, v[1]-s, v[2]-s, v[3]-s);
}

function sglSubSV4(v, s) {
	return sglV4C(s-v[0], s-v[1], s-v[2], s-v[3]);
}

function sglMulV4(u, v) {
	return sglV4C(u[0]*v[0], u[1]*v[1], u[2]*v[2], u[3]*v[3]);
}

function sglMulV4S(v, s) {
	return sglV4C(v[0]*s, v[1]*s, v[2]*s, v[3]*s);
}

function sglMulSV4(s, v) {
	return sglMulV4S(v, s);
}

function sglDivV4(u, v) {
	return sglV4C(u[0]/v[0], u[1]/v[1], u[2]/v[2], u[3]/v[3]);
}

function sglDivV4S(v, s) {
	return sglV4C(v[0]/s, v[1]/s, v[2]/s, v[3]/s);
}

function sglDivSV4(s, v) {
	return sglV4C(s/v[0], s/v[1], s/v[2], s/v[3]);
}

function sglRcpV4(v) {
	return sglDivSV4(1.0, v);
}

function sglDotV4(u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2] + u[3]*v[3]);
}

function sglSqLengthV4(v) {
	return sglDotV4(v, v);
}

function sglLengthV4(v) {
	return sglSqrt(sglSqLengthV4(v));
}

function sglNormalizedV4(v) {
	var f = 1.0 / sglLengthV4(v);
	return sglMulV4S(v, f);
}

function sglProjectV4(v) {
	var f = 1.0 / v[3];
	return sglV4C(v[0]*f, v[1]*f, v[2]*f, 1.0);
}

function sglSelfNegV4(v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	v[3] = -v[3];
	return v;
}

function sglSelfAddV4(u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	u[3] += v[3];
	return u;
}

function sglSelfAddV4S(v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return v;
}

function sglSelfAddSV4(s, v) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return v;
}

function sglSelfSubV4(u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	u[3] -= v[3];
	return u;
}

function sglSelfSubV4S(v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	v[3] -= s;
	return v;
}

function sglSelfSubSV4(v, s) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	v[3] = s - v[3];
	return v;
}

function sglSelfMulV4(u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	u[3] *= v[3];
	return u;
}

function sglSelfMulV4S(v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return v;
}

function sglSelfMulSV4(s, v) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return v;
}

function sglSelfDivV4(u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	u[3] /= v[3];
	return u;
}

function sglSelfDivV4S(v, s) {
	u[0] /= s;
	u[1] /= s;
	u[2] /= s;
	u[3] /= s;
	return u;
}

function sglSelfDivSV4(s, v) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	v[3] = s / v[3];
	return v;
}

function sglSelfRcpV4(v) {
	return sglSelfDivSV4(1.0, v);
}

function sglSelfNormalizeV4(v) {
	var f = 1.0 / sglLengthV4(v);
	return sglSelfMulV4S(v, f);
}

function sglSelfProjectV4(v) {
	var f = 1.0 / v[3];
	v[0] *= f;
	v[1] *= f;
	v[2] *= f;
	v[3]  = 1.0;
	return v;
}

function sglMinV4(u, v) {
	return [
		((u[0] < v[0]) ? (u[0]) : (v[0])),
		((u[1] < v[1]) ? (u[1]) : (v[1])),
		((u[2] < v[2]) ? (u[2]) : (v[2])),
		((u[3] < v[3]) ? (u[3]) : (v[3]))
	];
}

function sglMaxV4(u, v) {
	return [
		((u[0] > v[0]) ? (u[0]) : (v[0])),
		((u[1] > v[1]) ? (u[1]) : (v[1])),
		((u[2] > v[2]) ? (u[2]) : (v[2])),
		((u[3] > v[3]) ? (u[3]) : (v[3]))
	];
}

function sglV4toV2(v) {
	return v.slice(0, 2);
}

function sglV4toV3(v) {
	return v.slice(0, 3);
}
/***********************************************************************/


// 4x4 matrix
/***********************************************************************/
function sglUndefM4() {
	return new Array(16);
}

function sglM4V(v) {
	return v.slice(0, 16);
}

function sglM4S(s) {
	var m = sglUndefM4();
	for (var i=0; i<16; ++i) {
		m[i] = s;
	}
	return m;
}

function sglDiagM4V(d) {
	var m = sglM4S(0.0);
	m[ 0] = d[0];
	m[ 5] = d[1];
	m[10] = d[2];
	m[15] = d[3];
	return m;
}

function sglDiagM4S(s) {
	var m = sglM4S(0.0);
	m[ 0] = s;
	m[ 5] = s;
	m[10] = s;
	m[15] = s;
	return m;
}

function sglDiagM4C(m00, m11, m22, m33) {
	return sglDiagM4V(arguments);
}

function sglZeroM4() {
	return sglM4S(0.0);
}

function sglOneM4() {
	return sglM4S(1.0);
}

function sglIdentityM4() {
	return sglDiagM4S(1.0);
}

function sglM4() {
	var n = arguments.length;
	switch (n) {
		case 1:
			if (arguments[0] instanceof Array) {
				switch (arguments[0].length) {
					case 1:
						return sglDiagM4S(arguments[0]);
					break;
					case 4:
						return sglDiagM4V(arguments[0]);
					break;
					case 16:
						return sglM4V(arguments[0]);
					break;
					default:
						return sglIdentityM4();
					break;
				}
			}
			return sglM4S(arguments[0]);
		break;

		case 4:
			return sglDiagM4V(arguments);
		break;

		case 16:
			return sglM4V(arguments);
		break;

		default:
			return sglIdentityM4();
		break;
	}
	return null;
}

function sglDupM4(m) {
	var r = sglUndefM4();

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

function sglGetElemM4(m, row, col) {
	return m[row+col*4];
}

function sglSetElemM4(m, row, col, value) {
	m[row+col*4] = value;
}

function sglGetRowM4(m, r) {
	return sglV4C(m[r+0], m[r+4], m[r+8], m[r+12]);
}

function sglSetRowM4V(m, r, v) {
	m[r+ 0] = v[0];
	m[r+ 4] = v[1];
	m[r+ 8] = v[2];
	m[r+12] = v[3];
}

function sglSetRowM4S(m, r, s) {
	m[r+ 0] = s;
	m[r+ 4] = s;
	m[r+ 8] = s;
	m[r+12] = s;
}

function sglSetRowM4C(m, r, x, y, z, w) {
	m[r+ 0] = x;
	m[r+ 4] = y;
	m[r+ 8] = z;
	m[r+12] = w;
}

function sglGetColM4(m, c) {
	var i = c * 4;
	return sglV4C(m[i+0], m[i+1], m[i+2], m[i+3]);
}

function sglSetColM4V(m, c, v) {
	var i = c * 4;
	m[i+0] = v[0];
	m[i+1] = v[1];
	m[i+2] = v[2];
	m[i+3] = v[3];
}

function sglSetColM4S(m, c, s) {
	var i = c * 4;
	m[i+0] = s;
	m[i+1] = s;
	m[i+2] = s;
	m[i+3] = s;
}

function sglSetColM4C(m, c, x, y, z, w) {
	var i = c * 4;
	m[i+0] = x;
	m[i+1] = y;
	m[i+2] = z;
	m[i+3] = w;
}

function sglM4toM3(m) {
	return [
		m[ 0], m[ 1], m[ 2],
		m[ 4], m[ 5], m[ 6],
		m[ 8], m[ 9], m[10]
	];
}

function sglIsIdentityM4(m) {
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

function sglNegM4(m) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = -m[i];
	}
	return r;
}

function sglAddM4(a, b) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] + b[i];
	}
	return r;
}

function sglAddM4S(m, s)
{
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] + s;
	}
	return r;
}

function sglAddSM4(s, m) {
	return sglAddM4S(m, s);
}

function sglSubM4(a, b) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] - b[i];
	}
	return r;
}

function sglSubM4S(m, s) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] - s;
	}
	return r;
}

function sglSubSM4(s, m) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = s - m[i];
	}
	return r;
}

function sglMulM4(a, b) {
	var a0  = a[ 0], a1  = a[ 1],  a2 = a[ 2], a3  = a[ 3],
	    a4  = a[ 4], a5  = a[ 5],  a6 = a[ 6], a7  = a[ 7],
	    a8  = a[ 8], a9  = a[ 9], a10 = a[10], a11 = a[11],
	    a12 = a[12], a13 = a[13], a14 = a[14], a15 = a[15],

	    b0  = b[ 0], b1  = b[ 1], b2  = b[ 2], b3  = b[ 3],
	    b4  = b[ 4], b5  = b[ 5], b6  = b[ 6], b7  = b[ 7],
	    b8  = b[ 8], b9  = b[ 9], b10 = b[10], b11 = b[11],
	    b12 = b[12], b13 = b[13], b14 = b[14], b15 = b[15];

	var r = sglUndefM4();

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
	var r = sglUndefM4();

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

function sglMulM4S(m, s) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] * s;
	}
	return r;
}

function sglMulSM4(s, m) {
	return sglMulM4S(m, s);
}

function sglMulM4V3(m, v, w) {
	return [
		m[ 0] * v[0] + m[ 4] * v[1] + m[ 8] * v[2] + m[12] * w,
		m[ 1] * v[0] + m[ 5] * v[1] + m[ 9] * v[2] + m[13] * w,
		m[ 2] * v[0] + m[ 6] * v[1] + m[10] * v[2] + m[14] * w /* ,
		m[ 3] * v[0] + m[ 7] * v[1] + m[11] * v[2] + m[15] * w*/
	];
}

function sglMulM4V4(m, v) {
	return [
		m[ 0] * v[0] + m[ 4] * v[1] + m[ 8] * v[2] + m[12] * v[3],
		m[ 1] * v[0] + m[ 5] * v[1] + m[ 9] * v[2] + m[13] * v[3],
		m[ 2] * v[0] + m[ 6] * v[1] + m[10] * v[2] + m[14] * v[3],
		m[ 3] * v[0] + m[ 7] * v[1] + m[11] * v[2] + m[15] * v[3]
	];
}

function sglDivM4S(m, s) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = m[i] / s;
	}
	return r;
}

function sglDivSM4(s, m) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = s / m[i];
	}
	return r;
}

function sglRcpM4(m) {
	return sglDivSM4(1.0, m);
}

function sglCompMulM4(a, b) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] * b[i];
	}
	return r;
}

function sglCompDivM4(a, b) {
	var r = sglUndefM4();
	for (var i=0; i<16; ++i) {
		r[i] = a[i] / b[i];
	}
	return r;
}

function sglTransposeM4(m) {
	var r = sglUndefM4();

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

function sglDeterminantM4(m) {
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

function sglInverseM4(m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15]

	var t = sglUndefM4();

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

function sglTraceM4(m) {
	return (m[0] + m[5] + m[10] + m[15]);
}

function sglTranslationM4V(v) {
	var m = sglIdentityM4();
	m[12] = v[0];
	m[13] = v[1];
	m[14] = v[2];
	return m;
}

function sglTranslationM4C(x, y, z) {
	return sglTranslationM4V([x, y, z]);
}

function sglTranslationM4S(s) {
	return sglTranslationM4C(s, s, s);
}

function sglRotationAngleAxisM4V(angleRad, axis) {
	var ax = sglNormalizedV3(axis);
	var s  = sglSin(angleRad);
	var c  = sglCos(angleRad);
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

	var m = sglUndefM4();

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

function sglRotationAngleAxisM4C(angleRad, ax, ay, az) {
	return sglRotationAngleAxisM4V(angleRad, [ax, ay, az]);
}

function sglScalingM4V(v) {
	var m = sglIdentityM4();
	m[ 0] = v[0];
	m[ 5] = v[1];
	m[10] = v[2];
	return m;
}

function sglScalingM4C(x, y, z) {
	return sglScalingM4V([x, y, z]);
}

function sglScalingM4S(s) {
	return sglScalingM4C(s, s, s);
}

function sglLookAtM4V(position, target, up) {
	var v = sglNormalizedV3(sglSubV3(target, position));
	var u = sglNormalizedV3(up);
	var s = sglNormalizedV3(sglCrossV3(v, u));

	u = sglNormalizedV3(sglCrossV3(s, v));

	var m = sglUndefM4();

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

	m = sglMulM4(m, sglTranslationM4V(sglNegV3(position)));

	return m;
}

function sglLookAtM4C(positionX, positionY, positionZ, targetX, targetY, targetZ, upX, upY, upZ) {
	return sglLookAtM4V([positionX, positionY, positionZ], [targetX, targetY, targetZ], [upX, upY, upZ]);
}

function sglOrthoM4V(omin, omax) {
	var sum   = sglAddV3(omax, omin);
	var dif   = sglSubV3(omax, omin);

	var m = sglUndefM4();

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

function sglOrthoM4C(left, right, bottom, top, zNear, zFar) {
	return sglOrthoM4V([left, bottom, zNear], [right, top, zFar]);
}

function sglFrustumM4V(fmin, fmax) {
	var sum   = sglAddV3(fmax, fmin);
	var dif   = sglSubV3(fmax, fmin);
	var t     = 2.0 * fmin[2];

	var m = sglUndefM4();
	
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

function sglFrustumM4C(left, right, bottom, top, zNear, zFar) {
	return sglFrustumM4V([left, bottom, zNear], [right, top, zFar]);
}

function sglPerspectiveM4(fovYRad, aspectRatio, zNear, zFar) {
	var pmin = sglUndefV4();
	var pmax = sglUndefV4();

	pmin[2] = zNear;
	pmax[2] = zFar;

	pmax[1] = pmin[2] * sglTan(fovYRad / 2.0);
	pmin[1] = -pmax[1];

	pmax[0] = pmax[1] * aspectRatio;
	pmin[0] = -pmax[0];

	return sglFrustumM4V(pmin, pmax);
}
/***********************************************************************/


// quaternion
/***********************************************************************/
function sglUndefQuat(v) {
	return new Array(4);
}

function sglQuatV(v) {
	return v.slice(0, 4);
}

function sglIdentityQuat() {
	return [ 0.0, 0.0, 0.0, 1.0 ];
}

function sglAngleAxisQuat(angleRad, axis) {
	var halfAngle = angleRad / 2.0;
	var fsin = sglSin(halfAngle);
	return [
		fsin * axis[0],
		fsin * axis[1],
		fsin * axis[2],
		sglCos(halfAngle)
	];
}

function sglM4Quat(m) {
	var trace = sglGetElemM4(m, 0, 0) + sglGetElemM4(m, 1, 1) + sglGetElemM4(m, 2, 2);
	var root = null;
	var q = sglUndefQuat();

	if (trace > 0.0) {
		root = sglSqrt(trace + 1.0);
		q[3] = root / 2.0;
		root = 0.5 / root;
		q[0] = (sglGetElemM4(m, 2, 1) - sglGetElemM4(m, 1, 2)) * root;
		q[1] = (sglGetElemM4(m, 0, 2) - sglGetElemM4(m, 2, 0)) * root;
		q[2] = (sglGetElemM4(m, 1, 0) - sglGetElemM4(m, 0, 1)) * root;
	}
	else {
		var i = 0;

		if (sglGetElemM4(m, 1, 1) > sglGetElemM4(m, 0, 0)) i = 1;
		if (sglGetElemM4(m, 2, 2) > sglGetElemM4(m, i, i)) i = 2;

		var j = (i + 1) % 3;
		var k = (j + 1) % 3;

		root = sglSqrt(sglGetElemM4(m, i, i) - sglGetElemM4(m, j, j) - sglGetElemM4(m, k, k) + 1.0);

		q[i] = root / 2.0;
		root = 0.5 / root;
		q[3] = (sglGetElemM4(m, k, j) - sglGetElemM4(m, j, k)) * root;
		q[j] = (sglGetElemM4(m, j, i) + sglGetElemM4(m, i, j)) * root;
		q[k] = (sglGetElemM4(m, k, i) + sglGetElemM4(m, i, k)) * root;
	}
	return q;
}

function sglGetQuatAngleAxis(q) {
	var v = new Array(4);

	var sqLen = sglSqLengthV4(q);
	var angle = null;

	if (sqLen > 0.0) {
		var invLen = 1.0 / sglSqrt(sqLen);
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

function sglGetQuatRotationM4(q) {
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

	var m = sglIdentityM4();

	sglSetElemM4(m, 0, 0, 1.0 - (tyy + tzz));
	sglSetElemM4(m, 0, 1, txy - twz);
	sglSetElemM4(m, 0, 2, txz + twy);
	sglSetElemM4(m, 1, 0, txy + twz);
	sglSetElemM4(m, 1, 1, 1.0 - (txx + tzz));
	sglSetElemM4(m, 1, 2, tyz - twx);
	sglSetElemM4(m, 2, 0, txz - twy);
	sglSetElemM4(m, 2, 1, tyz + twx);
	sglSetElemM4(m, 2, 2, 1.0 - (txx + tyy));

	return m;
}

function sglNormalizedQuat(q) {
	return sglNormalizedV4(q);
}

function sglMulQuat(q1, q2) {
	var r = sglUndefQuat();

	r[0] = p[3] * q[0] + p[0] * q[3] + p[1] * q[2] - p[2] * q[1];
	r[1] = p[3] * q[1] + p[1] * q[3] + p[2] * q[0] - p[0] * q[2];
	r[2] = p[3] * q[2] + p[2] * q[3] + p[0] * q[1] - p[1] * q[0];
	r[3] = p[3] * q[3] - p[0] * q[0] - p[1] * q[1] - p[2] * q[2];
}
/***********************************************************************/
