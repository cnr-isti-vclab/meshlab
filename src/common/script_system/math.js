/**
 * @fileOverview Math
 */

/**
 * The SpiderGL.Math namespace.
 *
 * @namespace The SpiderGL.Math namespace.
 */
SpiderGL.Math = { };

// constants
/*---------------------------------------------------------*/

/**
 * Scale factor for degrees to radians conversion.
 * It is equal to PI / 180.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.DEG_TO_RAD = (Math.PI / 180.0);

/**
 * Alias for Math.E.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.E = Math.E;

/**
 * Alias for Math.LN2.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.LN2 = Math.LN2;

/**
 * Alias for Math.LN10.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.LN10 = Math.LN10;

/**
 * Alias for Math.LOG2E.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.LOG2E = Math.LOG2E;

/**
 * Alias for Math.LOG10E.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.LOG10E = Math.LOG10E;

/**
 * Alias for Math.PI.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.PI = Math.PI;

/**
 * Scale factor for radians to degrees conversion.
 * It is equal to 180 / PI.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.RAD_TO_DEG = (180.0 / Math.PI);

/**
 * Alias for Math.SQRT2.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.SQRT2 = Math.SQRT2;


/**
 * Alias for Number.MAX_VALUE.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.MAX_VALUE = Number.MAX_VALUE;

/**
 * Alias for Number.MIN_VALUE.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.MIN_VALUE = Number.MIN_VALUE;

/**
 * Alias for SpiderGL.Math.MAX_VALUE.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.MAX_NUMBER = SpiderGL.Math.MAX_VALUE;

/**
 * Alias for -SpiderGL.Math.MAX_VALUE.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.MIN_NUMBER = -SpiderGL.Math.MAX_VALUE;

/**
 * Alias for Number.NaN.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.NAN = Number.NaN;

/**
 * Alias for global Infinity.
 *
 * @constant
 *
 * @type number
 */
SpiderGL.Math.INFINITY = Infinity;

/*---------------------------------------------------------*/



// functions on scalars
/*---------------------------------------------------------*/

/**
 * Alias for Math.abs.
 *
 * @param {number} x A number.
 *
 * @returns {number} The absolute value of x.
 */
SpiderGL.Math.abs = function (x) {
	return Math.abs(x);
}

/**
 * Alias for Math.acos.
 *
 * @param {number} x The input cosine.
 *
 * @returns {number} The arccosine of x, in radians.
 */
SpiderGL.Math.acos = function (x) {
	return Math.acos(x);
}

/**
 * Alias for Math.asin.
 *
 * @param {number} x The input sine.
 *
 * @returns {number} The arcsine of x, in radians.
 */
SpiderGL.Math.asin = function (x) {
	return Math.asin(x);
}

/**
 * Alias for Math.atan2.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The arctangent of x as a numeric value between -PI/2 and PI/2 radians.
 */
SpiderGL.Math.atan = function (x) {
	return Math.atan(x);
}

/**
 * Alias for Math.atan2.
 *
 * @param {number} y A number.
 * @param {number} x A number.
 *
 * @returns {number} The the arctangent of the quotient of its arguments as a numeric value between PI and -PI.
 */
SpiderGL.Math.atan2 = function (y, x) {
	return Math.atan2(y, x);
}

/**
 * Alias for Math.ceil.
 *
 * @param {number} x The input number.
 *
 * @returns {number} x rounded upwards to the nearest integer.
 */
SpiderGL.Math.ceil = function (x) {
	return Math.ceil(x);
}

/**
 * Clamps a number over a range.
 *
 * @param {number} x The number to clamp.
 * @param {number} min The lower bound.
 * @param {number} max The upper bound.
 *
 * @returns {number} min if x < min, max if x > max, x otherwise.
 */
SpiderGL.Math.clamp = function (x, min, max) {
	return ((x <= min) ? (min) : ((x >= max) ? (max) : (x)));
}

/**
 * Alias for Math.cos.
 *
 * @param {number} x The input angle, in radians.
 *
 * @returns {number} The cosine of x.
 */
SpiderGL.Math.cos = function (x) {
	return Math.cos(x);
}

/**
 * Converts degrees to radians.
 *
 * @param {number} x The input angle, in degrees.
 *
 * @returns {number} x in radians.
 */
SpiderGL.Math.degToRad = function (x) {
	return (x * SpiderGL.Math.DEG_TO_RAD);
}

/**
 * Alias for Math.exp.
 *
 * @param {number} x The input number.
 *
 * @returns {number} E raised to x.
 */
SpiderGL.Math.exp = function (x) {
	return Math.exp(x);
}

/**
 * Alias for Math.floor.
 *
 * @param {number} x The input number.
 *
 * @returns {number} x rounded downwards to the nearest integer.
 */
SpiderGL.Math.floor = function (x) {
	return Math.floor(x);
}

/**
 * Linear interpolation between two numbers.
 *
 * @param {number} x The start interpolation bound.
 * @param {number} y The stop interpolation bound.
 * @param {number} t The interpolation factor, between 0 and 1..
 *
 * @returns {number} The interpolated value (1-t)*x + t*y.
 */
SpiderGL.Math.lerp = function (x, y, t) {
	return x + t * (y - x);
}

/**
 * Alias for Math.log.
 * Same as {@link SpiderGL.Math.log}.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The natural logarithm (base E) of x.
 */
SpiderGL.Math.ln = function (x) {
	return Math.log(x);
}

/**
 * Alias for Math.log.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The natural logarithm (base E) of x.
 */
SpiderGL.Math.log = function (x) {
	return Math.log(x);
}

/**
 * Logarithm base 2.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The base 2 logarithm of x.
 */
SpiderGL.Math.log2 = function (x) {
	return (SpiderGL.Math.log(x) / SpiderGL.Math.LN2);
}

/**
 * Logarithm base 10.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The base 10 logarithm of x.
 */
SpiderGL.Math.log10 = function (x) {
	return (SpiderGL.Math.log(x) / SpiderGL.Math.LN10);
}

/**
 * Alias for Math.max.
 *
 * @param {number} args Variable number of arguments
 *
 * @returns {number} The number with the highest value.
 *
 * @example
 * var x = SpiderGL.Math.max(3, 1.56, 2.1); // x == 3
 */
SpiderGL.Math.max = function (args) {
	return Math.max.apply(Math, arguments);
}

/**
 * Alias for Math.min.
 *
 * @param {number} args Variable number of arguments
 *
 * @returns {number} The number with the lowest value.
 *
 * @example
 * var x = SpiderGL.Math.min(3, 1.56, 2.1); // x == 1.56
 */
SpiderGL.Math.min = function (args) {
	return Math.min.apply(Math, arguments);
}

/**
 * Alias for Math.pow.
 *
 * @param {number} x The base number.
 * @param {number} x The exponent number.
 *
 * @returns {number} The value of x to the power of y.
 */
SpiderGL.Math.pow = function (x, y) {
	return Math.pow(x, y);
}

/**
 * Converts radians to degrees.
 *
 * @param {number} x The input angle, in radians.
 *
 * @returns {number} x in degrees.
 */
SpiderGL.Math.radToDeg = function (x) {
	return (x * SpiderGL.Math.RAD_TO_DEG);
}

/**
 * Alias for Math.random.
 *
 * @returns {number} A random number between 0.0 and 1.0, inclusive.
 */
SpiderGL.Math.random = function () {
	return Math.random();
}

/**
 * Alias for SpiderGL.Math.random.
 *
 * @returns {number} A random number between 0.0 and 1.0, inclusive.
 */
SpiderGL.Math.random01 = function () {
	return SpiderGL.Math.random();
}

/**
 * Generates a random number between -1.0 and 1.0.
 *
 * @returns {number} A random number between -1.0 and 1.0, inclusive.
 */
SpiderGL.Math.random11 = function () {
	return (SpiderGL.Math.random() * 2 - 1);
}

/**
 * Generates a random number between a and b.
 *
 * @param {number} min The range low end-point.
 * @param {number} max The range high end-point.
 *
 * @returns {number} A random number between min and max, inclusive.
 */
SpiderGL.Math.randomRange = function (min, max) {
	return (min + SpiderGL.Math.random() * (max - min));
}

/**
 * Alias for Math.round.
 *
 * @param {number} x The input number.
 *
 * @returns {number} x rounded to the nearest integer.
 *
 * @example
 * var a = SpiderGL.Math.round(    3); // a ==  3
 * var b = SpiderGL.Math.round(   -4); // b == -4
 * var c = SpiderGL.Math.round( 7.21); // c ==  7
 * var d = SpiderGL.Math.round( 7.56); // d ==  8
 * var e = SpiderGL.Math.round(-7.56); // e == -8
 * var f = SpiderGL.Math.round(-7.21); // f == -7
 */
SpiderGL.Math.round = function (x) {
	return Math.sqrt(x);
}

/**
 * Alias for Math.sin.
 *
 * @param {number} x The input angle, in radians.
 *
 * @returns {number} The sine of x.
 */
SpiderGL.Math.sin = function (x) {
	return Math.sin(x);
}

/**
 * Alias for Math.sqrt.
 *
 * @param {number} x The input number.
 *
 * @returns {number} The square root of x.
 */
SpiderGL.Math.sqrt = function (x) {
	return Math.sqrt(x);
}

/**
 * Alias for Math.tan.
 *
 * @param {number} x The input angle, in radians.
 *
 * @returns {number} The tangent of x.
 */
SpiderGL.Math.tan = function (x) {
	return Math.tan(x);
}

/*---------------------------------------------------------*/



// 2-dimensional vector
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Vec2 namespace.
 * The provided functions operate on 2-dimensional vectors, represented as standard JavaScript arrays of length 2.
 * In general, vectors are considered as column vectors.
 *
 * @namespace The SpiderGL.Math.Vec2 namespace defines operations on 2-dimensional vectors.
 */
SpiderGL.Math.Vec2 = { };

/**
 * Duplicates the input 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = v[i] (same as v.slice(0, 2)).
 */
SpiderGL.Math.Vec2.dup = function (v) {
	return v.slice(0, 2);
}

/**
 * Creates a 2-dimensional vector initialized with a scalar.
 *
 * @param {number} s The input scalar.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = s.
 */
SpiderGL.Math.Vec2.scalar = function (s) {
	return [s, s];
}

/**
 * Creates a 2-dimensional vector initialized with zero.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = 0.
 */
SpiderGL.Math.Vec2.zero = function () {
	return [0, 0];
}

/**
 * Creates a 2-dimensional vector initialized with one.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = 1.0.
 */
SpiderGL.Math.Vec2.one = function () {
	return [1, 1];
}

/**
 * Creates a 2-dimensional vector initialized with SpiderGL.Math.MAX_NUMBER.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = SpiderGL.Math.MAX_NUMBER.
 */
SpiderGL.Math.Vec2.maxNumber = function () {
	return [SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER];
}

/**
 * Creates a 2-dimensional vector initialized with SpiderGL.Math.MIN_NUMBER.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = SpiderGL.Math.MIN_NUMBER.
 */
SpiderGL.Math.Vec2.minNumber = function () {
	return [SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER];
}

/**
 * Creates a 3-dimensional vector from a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 * @param {number} [z=0.0] The 3th component.
 *
 * @returns {array} A 4-dimensional vector r equal to v extended with z as 3rd component.
 */
SpiderGL.Math.Vec2.to3 = function (v, z) {
	return [v[0], v[1], (z != undefined) ? z : 0];
}

/**
 * Creates a 4-dimensional vector from a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 * @param {number} [z=0.0] The 3th component.
 * @param {number} [w=1.0] The 4th component.
 *
 * @returns {array} A 4-dimensional vector r equal to v extended with z as 3rd component and w as 4th component.
 */
SpiderGL.Math.Vec2.to4 = function (v, z, w) {
	return [v[0], v[1], v[2], (z != undefined) ? z : 0, (w != undefined) ? w : 1];
}

/**
 * Component-wise negation of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = -v[i].
 */
SpiderGL.Math.Vec2.neg = function (v) {
	return [-v[0], -v[1]];
}

/**
 * Component-wise addition of two 2-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec2.add = function (u, v) {
	return [u[0]+v[0], u[1]+v[1]];
}

/**
 * Component-wise addition of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = v[i] + s.
 */
SpiderGL.Math.Vec2.adds = function (v, s) {
	return [v[0]+s, v[1]+s];
}

/**
 * Component-wise subtraction of two 2-dimensional vectors.
 *
 * @param {array} u The first subtraction operand.
 * @param {array} v The second subtraction operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec2.sub = function (u, v) {
	return [u[0]-v[0], u[1]-v[1]];
}

/**
 * Component-wise subtraction of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = v[i] - s.
 */
SpiderGL.Math.Vec2.subs = function (v, s) {
	return [v[0]-s, v[1]-s];
}

/**
 * Component-wise subtraction of a scalar and a 2-dimensional.
 *
 * @param {number} s The scalar subtraction operand.
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = s - v[i].
 */
SpiderGL.Math.Vec2.ssub = function (s, v) {
	return [s-v[0], s-v[1]];
}

/**
 * Component-wise multiplication of two 2-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec2.mul = function (u, v) {
	return [u[0]*v[0], u[1]*v[1]];
}

/**
 * Component-wise multiplication of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The vector multiplication operand.
 * @param {number} s The scalar multiplication operand.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = v[i] * s.
 */
SpiderGL.Math.Vec2.muls = function (v, s) {
	return [v[0]*s, v[1]*s];
}

/**
 * Component-wise division of two 2-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec2.div = function (u, v) {
	return [u[0]/v[0], u[1]/v[1]];
}

/**
 * Component-wise division of a 2-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = v[i] / s.
 */
SpiderGL.Math.Vec2.divs = function (v, s) {
	return [v[0]/s, v[1]/s];
}

/**
 * Component-wise division of a scalar by a 2-dimensional vector.
 *
 * @param {number} s The denominator scalar.
 * @param {array} v The numerator vector.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = s / v[i].
 */
SpiderGL.Math.Vec2.sdiv = function (s, v) {
	return [s/v[0], s/v[1]];
}

/**
 * Component-wise reciprocal of a 2-dimensional vector.
 *
 * @param {array} v The input (denominator) vector.
 *
 * @returns {array} A 2-dimensionals vector r, where r[i] = 1.0 / v[i].
 */
SpiderGL.Math.Vec2.rcp = function (v) {
	return [1.0/v[0], 1.0/v[1]];
}

/**
 * Dot product of two 2-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 *
 * @returns {number} The dot product of u and v.
 */
SpiderGL.Math.Vec2.dot = function (u, v) {
	return (u[0]*v[0] + u[1]*v[1]);
}

/**
 * Cross product of two 2-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 *
 * @returns {array} A 2-dimensional vector equal to the cross product of u and v.
 */
SpiderGL.Math.Vec2.cross = function (u, v) {
	return (u[0]*v[1] - u[1]*v[0]);
}

/**
 * Perp operation.
 * Returns a 2-dimensional vector which is orthogonal to the input vector and lies in the right halfspace.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r = [v[1], -v[0]].
 */
SpiderGL.Math.Vec2.perp = function (v) {
	return [v[1], -v[0]];
}

/**
 * Squared length of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The squared length of v, same as the dot product of v with itself.
 */
SpiderGL.Math.Vec2.sqLength = function (v) {
	return SpiderGL.Math.Vec2.dot(v, v);
}

/**
 * Length of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The length of v.
 */
SpiderGL.Math.Vec2.length = function (v) {
	return SpiderGL.Math.sqrt(SpiderGL.Math.Vec2.sqLength(v));
}

/**
 * Creates a normalized 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r representing the normalized v, where r[i] = v[i] / {@link SpiderGL.Math.Vec2.length}(v).
 */
SpiderGL.Math.Vec2.normalize = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec2.length(v);
	return SpiderGL.Math.Vec2.muls(v, f);
}

/**
 * Component-wise absolute value of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.abs}(v[i]).
 */
SpiderGL.Math.Vec2.abs = function (v) {
	return [SpiderGL.Math.abs(v[0]), SpiderGL.Math.abs(v[1])];
}

/**
 * Component-wise arccosine of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.acos}(v[i]), in radians.
 */
SpiderGL.Math.Vec2.acos = function (v) {
	return [SpiderGL.Math.acos(v[0]), SpiderGL.Math.acos(v[1])];
}

/**
 * Component-wise arcsine of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.asin}(v[i]), in radians.
 */
SpiderGL.Math.Vec2.asin = function (v) {
	return [SpiderGL.Math.asin(v[0]), SpiderGL.Math.asin(v[1])];
}

/**
 * Component-wise arctangent of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan}(v[i]), between -PI/2 and PI/2 radians.
 */
SpiderGL.Math.Vec2.atan = function (v) {
	return [SpiderGL.Math.atan(v[0]), SpiderGL.Math.atan(v[1])];
}

/**
 * Component-wise arctangent of the quotient of two 2-dimensional vectors.
 *
 * @param {array} y The numerator vector.
 * @param {array} x The denominator vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan2}(y[i], x[i]), between PI and -PI radians.
 */
SpiderGL.Math.Vec2.atan2 = function (y, x) {
	return [SpiderGL.Math.atan2(y[0], x[0]), SpiderGL.Math.atan2(y[1], x[1])];
}

/**
 * Component-wise ceil of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.ceil}(v[i]).
 */
SpiderGL.Math.Vec2.ceil = function (v) {
	return [SpiderGL.Math.ceil(v[0]), SpiderGL.Math.ceil(v[1])];
}

/**
 * Component-wise clamp of a 2-dimensional vector with vector bounds.
 *
 * @param {array} v The input vector.
 * @param {array} min The lower 2-dimensional bound.
 * @param {array} max The upper 2-dimensional bound.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.clamp}(v[i], min[i], max[i]).
 */
SpiderGL.Math.Vec2.clamp = function (v, min, max) {
	return [SpiderGL.Math.clamp(v[0], min[0], max[0]), SpiderGL.Math.clamp(v[1], min[1], max[1])];
}

/**
 * Component-wise cosine of a 2-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.cos}(v[i]).
 */
SpiderGL.Math.Vec2.cos = function (v) {
	return [SpiderGL.Math.cos(v[0]), SpiderGL.Math.cos(v[1])];
}

/**
 * Component-wise conversion of a 2-dimensional vector from degrees to radians.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.degToRad}(v[i]).
 */
SpiderGL.Math.Vec2.degToRad = function (v) {
	return [SpiderGL.Math.degToRad(v[0]), SpiderGL.Math.degToRad(v[1])];
}

/**
 * Component-wise exponential of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.exp}(v[i]).
 */
SpiderGL.Math.Vec2.exp = function (v) {
	return [SpiderGL.Math.exp(v[0]), SpiderGL.Math.exp(v[1])];
}

/**
 * Component-wise floor of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.floor}(v[i]).
 */
SpiderGL.Math.Vec2.floor = function (v) {
	return [SpiderGL.Math.floor(v[0]), SpiderGL.Math.floor(v[1])];
}

/**
 * Linear interpolation between two 2-dimensional vectors.
 *
 * @param {array} u The start interpolation bound.
 * @param {array} v The stop interpolation bound.
 * @param {number} t The interpolation factor, between 0 and 1.
 *
 * @returns {array} The interpolated vector r, where r[i] = (1-t)*u[i] + t*v[i].
 */
SpiderGL.Math.Vec2.lerp = function (u, v, t) {
	return [
		SpiderGL.Math.lerp(u[0], v[0], t),
		SpiderGL.Math.lerp(u[1], v[1], t)
	];
}

/**
 * Component-wise natural (base E) logarithm of a 2-dimensional vector.
 * Same as {@link SpiderGL.Math.Vec2.log}.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.ln}(v[i]).
 */
SpiderGL.Math.Vec2.ln = function (v) {
	return [SpiderGL.Math.ln(v[0]), SpiderGL.Math.ln(v[1])];
}

/**
 * Component-wise natural (base E) logarithm of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.log}(v[i]).
 */
SpiderGL.Math.Vec2.log = function (v) {
	return [SpiderGL.Math.log(v[0]), SpiderGL.Math.log(v[1])];
}

/**
 * Component-wise base 2 logarithm of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.log2}(v[i]).
 */
SpiderGL.Math.Vec2.log2 = function (v) {
	return [SpiderGL.Math.log2(v[0]), SpiderGL.Math.log2(v[1])];
}

/**
 * Component-wise base 10 logarithm of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.log10}(v[i]).
 */
SpiderGL.Math.Vec2.log10 = function (v) {
	return [SpiderGL.Math.log10(v[0]), SpiderGL.Math.log10(v[1])];
}

/**
 * Component-wise maximum of two 2-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.max}(u[i], v[i]).
 */
SpiderGL.Math.Vec2.max = function (u, v) {
	return [SpiderGL.Math.max(u[0], v[0]), SpiderGL.Math.max(u[1], v[1])];
}

/**
 * Component-wise minimum of two 2-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.min}(u[i], v[i]).
 */
SpiderGL.Math.Vec2.min = function (u, v) {
	return [SpiderGL.Math.min(u[0], v[0]), SpiderGL.Math.min(u[1], v[1])];
}

/**
 * Component-wise power of two 2-dimensional vectors.
 *
 * @param {array} u The base vector.
 * @param {array} v The exponent vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(u[i], v[i]).
 */
SpiderGL.Math.Vec2.pow = function (u, v) {
	return [SpiderGL.Math.pow(u[0], v[0]), SpiderGL.Math.pow(u[1], v[1])];
}

/**
 * Component-wise conversion of a 2-dimensional vector from radians to degrees.
 *
 * @param {array} v The input vector, in degrees.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(v[i]).
 */
SpiderGL.Math.Vec2.radToDeg = function (v) {
	return [SpiderGL.Math.radToDeg(v[0]), SpiderGL.Math.radToDeg(v[1])];
}

/**
 * Creates a random 2-dimensional vector between 0 and 1.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.random}().
 */
SpiderGL.Math.Vec2.random = function () {
	return [SpiderGL.Math.random(), SpiderGL.Math.random()];
}

/**
 * Creates a random 2-dimensional vector between 0 and 1.
 * Same as {@link SpiderGL.Math.Vec2.random}.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.random01}().
 */
SpiderGL.Math.Vec2.random01 = function () {
	return [SpiderGL.Math.random01(), SpiderGL.Math.random01()];
}

/**
 * Creates a random 2-dimensional vector between -1 and 1.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.random11}().
 */
SpiderGL.Math.Vec2.random11 = function () {
	return [SpiderGL.Math.random11(), SpiderGL.Math.random11()];
}

/**
 * Creates a random 2-dimensional vector inside a range.
 *
 * @param {array} min The range vector lower bound.
 * @param {array} max The range vector upper bound.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.randomRange}(min[i], max[i]).
 */
SpiderGL.Math.Vec2.randomRange = function (min, max) {
	return [SpiderGL.Math.randomRange(min[0], max[0]), SpiderGL.Math.randomRange(min[1], max[1])];
}

/**
 * Component-wise round of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.round}(v[i]).
 */
SpiderGL.Math.Vec2.round = function (v) {
	return [SpiderGL.Math.round(v[0]), SpiderGL.Math.round(v[1])];
}

/**
 * Component-wise sine of a 2-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.sin}(v[i]).
 */
SpiderGL.Math.Vec2.sin = function (v) {
	return [SpiderGL.Math.sin(v[0]), SpiderGL.Math.sin(v[1])];
}

/**
 * Component-wise square root of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.sqrt}(v[i]).
 */
SpiderGL.Math.Vec2.sqrt = function (v) {
	return [SpiderGL.Math.sqrt(v[0]), SpiderGL.Math.sqrt(v[1])];
}

/**
 * Component-wise tangent root of a 2-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 2-dimensional vector r, where r[i] = {@link SpiderGL.Math.tan}(v[i]).
 */
SpiderGL.Math.Vec2.tan = function (v) {
	return [SpiderGL.Math.tan(v[0]), SpiderGL.Math.tan(v[1])];
}

/**
 * In-place component-wise copy of two 2-dimensional vectors.
 *
 * @param {array} u The destination vector.
 * @param {array} v The source vector.
 *
 * @returns {array} The destination vector u, where u[i] = v[i].
 */
SpiderGL.Math.Vec2.copy$ = function (u, v) {
	u[0] = v[0];
	u[1] = v[1];
	return u;
}

/**
 * In-place component-wise negation of a 2-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = -v[i].
 */
SpiderGL.Math.Vec2.neg$ = function (v) {
	v[0] = -v[0];
	v[1] = -v[1];
	return v;
}

/**
 * In-place component-wise addition of two 2-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec2.add$ = function (u, v) {
	u[0] += v[0];
	u[1] += v[1];
	return u;
}

/**
 * In-place component-wise addition of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] + s.
 */
SpiderGL.Math.Vec2.adds$ = function (v, s) {
	v[0] += s;
	v[1] += s;
	return v;
}

/**
 * In-place component-wise subtraction of two 2-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec2.sub$ = function (u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	return u;
}

/**
 * In-place component-wise subtraction of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] - s.
 */
SpiderGL.Math.Vec2.subs$ = function (v, s) {
	v[0] -= s;
	v[1] -= s;
	return v;
}

/**
 * In-place component-wise subtraction of a scalar and a 2-dimensional vector.
 *
 * @param {number} s The scalar subtraction operand
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} The input vector v, where v[i] = s - v[i].
 */
SpiderGL.Math.Vec2.ssub$ = function (s, v) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	return v;
}

/**
 * In-place component-wise multiplication of two 2-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec2.mul$ = function (u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	return u;
}

/**
 * In-place component-wise multiplication of a 2-dimensional vector and a scalar.
 *
 * @param {array} v The first multiplication operand.
 * @param {number} s The second multiplication operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] * s.
 */
SpiderGL.Math.Vec2.muls$ = function (v, s) {
	v[0] *= s;
	v[1] *= s;
	return v;
}

/**
 * In-place component-wise division of two 2-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector u, where u[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec2.div$ = function (u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	return u;
}

/**
 * In-place component-wise division of a 2-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / s.
 */
SpiderGL.Math.Vec2.divs$ = function (v, s) {
	v[0] /= s;
	v[1] /= s;
	return v;
}

/**
 * In-place component-wise division of a scalar by a 2-dimensional.
 *
 * @param {number} s The scalar numerator.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector v, where v[i] = s / v[i].
 */
SpiderGL.Math.Vec2.sdiv$ = function (v, s) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	return v;
}

/**
 * In-place perp operation.
 * Returns a 2-dimensional vector which is orthogonal to the input vector and lies in the right halfspace.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v = [v[1], -v[0]].
 */
SpiderGL.Math.Vec2.perp$ = function (v) {
	var v0 = v[0];
	v[0] = v[1];
	v[1] = -v0;
	return v;
}

/**
 * In-place 2-dimensional vector normalization.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / {@link SpiderGL.Math.Vec2.length}(v).
 */
SpiderGL.Math.Vec2.normalize$ = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec2.length(v);
	return SpiderGL.Math.Vec2.muls$(v, f);
}

/*---------------------------------------------------------*/



// 3-dimensional vector
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Vec3 namespace.
 * The provided functions operate on 3-dimensional vectors, represented as standard JavaScript arrays of length 3.
 * In general, vectors are considered as column vectors.
 *
 * @namespace The SpiderGL.Math.Vec3 namespace defines operations on 3-dimensional vectors.
 */
SpiderGL.Math.Vec3 = { };

/**
 * Duplicates the input 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = v[i] (same as v.slice(0, 3)).
 */
SpiderGL.Math.Vec3.dup = function (v) {
	return v.slice(0, 3);
}

/**
 * Creates a 3-dimensional vector initialized with a scalar.
 *
 * @param {number} s The input scalar.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = s.
 */
SpiderGL.Math.Vec3.scalar = function (s) {
	return [s, s, s];
}

/**
 * Creates a 3-dimensional vector initialized with zero.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = 0.
 */
SpiderGL.Math.Vec3.zero = function () {
	return [0, 0, 0];
}

/**
 * Creates a 3-dimensional vector initialized with one.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = 1.0.
 */
SpiderGL.Math.Vec3.one = function () {
	return [1, 1, 1];
}

/**
 * Creates a 3-dimensional vector initialized with SpiderGL.Math.MAX_NUMBER.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = SpiderGL.Math.MAX_NUMBER.
 */
SpiderGL.Math.Vec3.maxNumber = function () {
	return [SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER];
}

/**
 * Creates a 3-dimensional vector initialized with SpiderGL.Math.MIN_NUMBER.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = SpiderGL.Math.MIN_NUMBER.
 */
SpiderGL.Math.Vec3.minNumber = function () {
	return [SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER];
}

/**
 * Creates a 2-dimensional vector from a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r equal to v with the 3rd component dropped.
 */
SpiderGL.Math.Vec3.to2 = function (v) {
	return [v[0], v[1]];
}

/**
 * Creates a 4-dimensional vector from a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 * @param {number} [w=1.0] The 4th component.
 *
 * @returns {array} A 4-dimensional vector r equal to v extended with w as 4th component.
 */
SpiderGL.Math.Vec3.to4 = function (v, w) {
	return [v[0], v[1], v[2], (w != undefined) ? w : 1];
}

/**
 * Component-wise negation of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = -v[i].
 */
SpiderGL.Math.Vec3.neg = function (v) {
	return [-v[0], -v[1], -v[2]];
}

/**
 * Component-wise addition of two 3-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec3.add = function (u, v) {
	return [u[0]+v[0], u[1]+v[1], u[2]+v[2]];
}

/**
 * Component-wise addition of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = v[i] + s.
 */
SpiderGL.Math.Vec3.adds = function (v, s) {
	return [v[0]+s, v[1]+s, v[2]+s];
}

/**
 * Component-wise subtraction of two 3-dimensional vectors.
 *
 * @param {array} u The first subtraction operand.
 * @param {array} v The second subtraction operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec3.sub = function (u, v) {
	return [u[0]-v[0], u[1]-v[1], u[2]-v[2]];
}

/**
 * Component-wise subtraction of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = v[i] - s.
 */
SpiderGL.Math.Vec3.subs = function (v, s) {
	return [v[0]-s, v[1]-s, v[2]-s];
}

/**
 * Component-wise subtraction of a scalar and a 3-dimensional.
 *
 * @param {number} s The scalar subtraction operand.
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = s - v[i].
 */
SpiderGL.Math.Vec3.ssub = function (s, v) {
	return [s-v[0], s-v[1], s-v[2]];
}

/**
 * Component-wise multiplication of two 3-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec3.mul = function (u, v) {
	return [u[0]*v[0], u[1]*v[1], u[2]*v[2]];
}

/**
 * Component-wise multiplication of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The vector multiplication operand.
 * @param {number} s The scalar multiplication operand.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = v[i] * s.
 */
SpiderGL.Math.Vec3.muls = function (v, s) {
	return [v[0]*s, v[1]*s, v[2]*s];
}

/**
 * Component-wise division of two 3-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec3.div = function (u, v) {
	return [u[0]/v[0], u[1]/v[1], u[2]/v[2]];
}

/**
 * Component-wise division of a 3-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = v[i] / s.
 */
SpiderGL.Math.Vec3.divs = function (v, s) {
	return [v[0]/s, v[1]/s, v[2]/s];
}

/**
 * Component-wise division of a scalar by a 3-dimensional vector.
 *
 * @param {number} s The denominator scalar.
 * @param {array} v The numerator vector.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = s / v[i].
 */
SpiderGL.Math.Vec3.sdiv = function (s, v) {
	return [s/v[0], s/v[1], s/v[2]];
}

/**
 * Component-wise reciprocal of a 3-dimensional vector.
 *
 * @param {array} v The input (denominator) vector.
 *
 * @returns {array} A 3-dimensionals vector r, where r[i] = 1.0 / v[i].
 */
SpiderGL.Math.Vec3.rcp = function (v) {
	return [1.0/v[0], 1.0/v[1], 1.0/v[2]];
}

/**
 * Dot product of two 3-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 *
 * @returns {number} The dot product of u and v.
 */
SpiderGL.Math.Vec3.dot = function (u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2]);
}

/**
 * Cross product of two 3-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 *
 * @returns {array} A 3-dimensional vector equal to the cross product of u and v.
 */
SpiderGL.Math.Vec3.cross = function (u, v) {
	return [u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0]];
}

/**
 * Squared length of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The squared length of v, same as the dot product of v with itself.
 */
SpiderGL.Math.Vec3.sqLength = function (v) {
	return SpiderGL.Math.Vec3.dot(v, v);
}

/**
 * Length of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The length of v.
 */
SpiderGL.Math.Vec3.length = function (v) {
	return SpiderGL.Math.sqrt(SpiderGL.Math.Vec3.sqLength(v));
}

/**
 * Creates a normalized 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r representing the normalized v, where r[i] = v[i] / {@link SpiderGL.Math.Vec3.length}(v).
 */
SpiderGL.Math.Vec3.normalize = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec3.length(v);
	return SpiderGL.Math.Vec3.muls(v, f);
}

/**
 * Component-wise absolute value of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.abs}(v[i]).
 */
SpiderGL.Math.Vec3.abs = function (v) {
	return [SpiderGL.Math.abs(v[0]), SpiderGL.Math.abs(v[1]), SpiderGL.Math.abs(v[2])];
}

/**
 * Component-wise arccosine of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.acos}(v[i]), in radians.
 */
SpiderGL.Math.Vec3.acos = function (v) {
	return [SpiderGL.Math.acos(v[0]), SpiderGL.Math.acos(v[1]), SpiderGL.Math.acos(v[2])];
}

/**
 * Component-wise arcsine of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.asin}(v[i]), in radians.
 */
SpiderGL.Math.Vec3.asin = function (v) {
	return [SpiderGL.Math.asin(v[0]), SpiderGL.Math.asin(v[1]), SpiderGL.Math.asin(v[2])];
}

/**
 * Component-wise arctangent of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan}(v[i]), between -PI/2 and PI/2 radians.
 */
SpiderGL.Math.Vec3.atan = function (v) {
	return [SpiderGL.Math.atan(v[0]), SpiderGL.Math.atan(v[1]), SpiderGL.Math.atan(v[2])];
}

/**
 * Component-wise arctangent of the quotient of two 3-dimensional vectors.
 *
 * @param {array} y The numerator vector.
 * @param {array} x The denominator vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan2}(y[i], x[i]), between PI and -PI radians.
 */
SpiderGL.Math.Vec3.atan2 = function (y, x) {
	return [SpiderGL.Math.atan2(y[0], x[0]), SpiderGL.Math.atan2(y[1], x[1]), SpiderGL.Math.atan2(y[2], x[2])];
}

/**
 * Component-wise ceil of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.ceil}(v[i]).
 */
SpiderGL.Math.Vec3.ceil = function (v) {
	return [SpiderGL.Math.ceil(v[0]), SpiderGL.Math.ceil(v[1]), SpiderGL.Math.ceil(v[2])];
}

/**
 * Component-wise clamp of a 3-dimensional vector with vector bounds.
 *
 * @param {array} v The input vector.
 * @param {array} min The lower 3-dimensional bound.
 * @param {array} max The upper 3-dimensional bound.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.clamp}(v[i], min[i], max[i]).
 */
SpiderGL.Math.Vec3.clamp = function (v, min, max) {
	return [SpiderGL.Math.clamp(v[0], min[0], max[0]), SpiderGL.Math.clamp(v[1], min[1], max[1]), SpiderGL.Math.clamp(v[2], min[2], max[2])];
}

/**
 * Component-wise cosine of a 3-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.cos}(v[i]).
 */
SpiderGL.Math.Vec3.cos = function (v) {
	return [SpiderGL.Math.cos(v[0]), SpiderGL.Math.cos(v[1]), SpiderGL.Math.cos(v[2])];
}

/**
 * Component-wise conversion of a 3-dimensional vector from degrees to radians.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.degToRad}(v[i]).
 */
SpiderGL.Math.Vec3.degToRad = function (v) {
	return [SpiderGL.Math.degToRad(v[0]), SpiderGL.Math.degToRad(v[1]), SpiderGL.Math.degToRad(v[2])];
}

/**
 * Component-wise exponential of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.exp}(v[i]).
 */
SpiderGL.Math.Vec3.exp = function (v) {
	return [SpiderGL.Math.exp(v[0]), SpiderGL.Math.exp(v[1]), SpiderGL.Math.exp(v[2])];
}

/**
 * Component-wise floor of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.floor}(v[i]).
 */
SpiderGL.Math.Vec3.floor = function (v) {
	return [SpiderGL.Math.floor(v[0]), SpiderGL.Math.floor(v[1]), SpiderGL.Math.floor(v[2])];
}

/**
 * Linear interpolation between two 3-dimensional vectors.
 *
 * @param {array} u The start interpolation bound.
 * @param {array} v The stop interpolation bound.
 * @param {number} t The interpolation factor, between 0 and 1.
 *
 * @returns {array} The interpolated vector r, where r[i] = (1-t)*u[i] + t*v[i].
 */
SpiderGL.Math.Vec3.lerp = function (u, v, t) {
	return [
		SpiderGL.Math.lerp(u[0], v[0], t),
		SpiderGL.Math.lerp(u[1], v[1], t),
		SpiderGL.Math.lerp(u[2], v[2], t)
	];
}

/**
 * Component-wise natural (base E) logarithm of a 3-dimensional vector.
 * Same as {@link SpiderGL.Math.Vec3.log}.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.ln}(v[i]).
 */
SpiderGL.Math.Vec3.ln = function (v) {
	return [SpiderGL.Math.ln(v[0]), SpiderGL.Math.ln(v[1]), SpiderGL.Math.ln(v[2])];
}

/**
 * Component-wise natural (base E) logarithm of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.log}(v[i]).
 */
SpiderGL.Math.Vec3.log = function (v) {
	return [SpiderGL.Math.log(v[0]), SpiderGL.Math.log(v[1]), SpiderGL.Math.log(v[2])];
}

/**
 * Component-wise base 2 logarithm of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.log2}(v[i]).
 */
SpiderGL.Math.Vec3.log2 = function (v) {
	return [SpiderGL.Math.log2(v[0]), SpiderGL.Math.log2(v[1]), SpiderGL.Math.log2(v[2])];
}

/**
 * Component-wise base 10 logarithm of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.log10}(v[i]).
 */
SpiderGL.Math.Vec3.log10 = function (v) {
	return [SpiderGL.Math.log10(v[0]), SpiderGL.Math.log10(v[1]), SpiderGL.Math.log10(v[2])];
}

/**
 * Component-wise maximum of two 3-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.max}(u[i], v[i]).
 */
SpiderGL.Math.Vec3.max = function (u, v) {
	return [SpiderGL.Math.max(u[0], v[0]), SpiderGL.Math.max(u[1], v[1]), SpiderGL.Math.max(u[2], v[2])];
}

/**
 * Component-wise minimum of two 3-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.min}(u[i], v[i]).
 */
SpiderGL.Math.Vec3.min = function (u, v) {
	return [SpiderGL.Math.min(u[0], v[0]), SpiderGL.Math.min(u[1], v[1]), SpiderGL.Math.min(u[2], v[2])];
}

/**
 * Component-wise power of two 3-dimensional vectors.
 *
 * @param {array} u The base vector.
 * @param {array} v The exponent vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(u[i], v[i]).
 */
SpiderGL.Math.Vec3.pow = function (u, v) {
	return [SpiderGL.Math.pow(u[0], v[0]), SpiderGL.Math.pow(u[1], v[1]), SpiderGL.Math.pow(u[2], v[2])];
}

/**
 * Component-wise conversion of a 3-dimensional vector from radians to degrees.
 *
 * @param {array} v The input vector, in degrees.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(v[i]).
 */
SpiderGL.Math.Vec3.radToDeg = function (v) {
	return [SpiderGL.Math.radToDeg(v[0]), SpiderGL.Math.radToDeg(v[1]), SpiderGL.Math.radToDeg(v[2])];
}

/**
 * Creates a random 3-dimensional vector between 0 and 1.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.random}().
 */
SpiderGL.Math.Vec3.random = function () {
	return [SpiderGL.Math.random(), SpiderGL.Math.random(), SpiderGL.Math.random()];
}

/**
 * Creates a random 3-dimensional vector between 0 and 1.
 * Same as {@link SpiderGL.Math.Vec3.random}.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.random01}().
 */
SpiderGL.Math.Vec3.random01 = function () {
	return [SpiderGL.Math.random01(), SpiderGL.Math.random01(), SpiderGL.Math.random01()];
}

/**
 * Creates a random 3-dimensional vector between -1 and 1.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.random11}().
 */
SpiderGL.Math.Vec3.random11 = function () {
	return [SpiderGL.Math.random11(), SpiderGL.Math.random11(), SpiderGL.Math.random11()];
}

/**
 * Creates a random 3-dimensional vector inside a range.
 *
 * @param {array} min The range vector lower bound.
 * @param {array} max The range vector upper bound.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.randomRange}(min[i], max[i]).
 */
SpiderGL.Math.Vec3.randomRange = function (min, max) {
	return [SpiderGL.Math.randomRange(min[0], max[0]), SpiderGL.Math.randomRange(min[1], max[1]), SpiderGL.Math.randomRange(min[2], max[2])];
}

/**
 * Component-wise round of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.round}(v[i]).
 */
SpiderGL.Math.Vec3.round = function (v) {
	return [SpiderGL.Math.round(v[0]), SpiderGL.Math.round(v[1]), SpiderGL.Math.round(v[2])];
}

/**
 * Component-wise sine of a 3-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.sin}(v[i]).
 */
SpiderGL.Math.Vec3.sin = function (v) {
	return [SpiderGL.Math.sin(v[0]), SpiderGL.Math.sin(v[1]), SpiderGL.Math.sin(v[2])];
}

/**
 * Component-wise square root of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.sqrt}(v[i]).
 */
SpiderGL.Math.Vec3.sqrt = function (v) {
	return [SpiderGL.Math.sqrt(v[0]), SpiderGL.Math.sqrt(v[1]), SpiderGL.Math.sqrt(v[2])];
}

/**
 * Component-wise tangent root of a 3-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 3-dimensional vector r, where r[i] = {@link SpiderGL.Math.tan}(v[i]).
 */
SpiderGL.Math.Vec3.tan = function (v) {
	return [SpiderGL.Math.tan(v[0]), SpiderGL.Math.tan(v[1]), SpiderGL.Math.tan(v[2])];
}

/**
 * In-place component-wise copy of two 3-dimensional vectors.
 *
 * @param {array} u The destination vector.
 * @param {array} v The source vector.
 *
 * @returns {array} The destination vector u, where u[i] = v[i].
 */
SpiderGL.Math.Vec3.copy$ = function (u, v) {
	u[0] = v[0];
	u[1] = v[1];
	u[2] = v[2];
	return u;
}

/**
 * In-place component-wise negation of a 3-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = -v[i].
 */
SpiderGL.Math.Vec3.neg$ = function (v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	return v;
}

/**
 * In-place component-wise addition of two 3-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec3.add$ = function (u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	return u;
}

/**
 * In-place component-wise addition of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] + s.
 */
SpiderGL.Math.Vec3.adds$ = function (v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	return v;
}

/**
 * In-place component-wise subtraction of two 3-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec3.sub$ = function (u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	return u;
}

/**
 * In-place component-wise subtraction of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] - s.
 */
SpiderGL.Math.Vec3.subs$ = function (v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	return v;
}

/**
 * In-place component-wise subtraction of a scalar and a 3-dimensional vector.
 *
 * @param {number} s The scalar subtraction operand
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} The input vector v, where v[i] = s - v[i].
 */
SpiderGL.Math.Vec3.ssub$ = function (s, v) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	return v;
}

/**
 * In-place component-wise multiplication of two 3-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec3.mul$ = function (u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	return u;
}

/**
 * In-place component-wise multiplication of a 3-dimensional vector and a scalar.
 *
 * @param {array} v The first multiplication operand.
 * @param {number} s The second multiplication operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] * s.
 */
SpiderGL.Math.Vec3.muls$ = function (v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	return v;
}

/**
 * In-place component-wise division of two 3-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector u, where u[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec3.div$ = function (u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	return u;
}

/**
 * In-place component-wise division of a 3-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / s.
 */
SpiderGL.Math.Vec3.divs$ = function (v, s) {
	v[0] /= s;
	v[1] /= s;
	v[2] /= s;
	return v;
}

/**
 * In-place component-wise division of a scalar by a 3-dimensional.
 *
 * @param {number} s The scalar numerator.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector v, where v[i] = s / v[i].
 */
SpiderGL.Math.Vec3.sdiv$ = function (v, s) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	return v;
}

/**
 * In-place 3-dimensional vector normalization.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / {@link SpiderGL.Math.Vec3.length}(v)
 */
SpiderGL.Math.Vec3.normalize$ = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec3.length(v);
	return SpiderGL.Math.Vec3.muls$(v, f);
}

/*---------------------------------------------------------*/



// 4-dimensional vector
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Vec4 namespace.
 * The provided functions operate on 4-dimensional vectors, represented as standard JavaScript arrays of length 4.
 * In general, vectors are considered as column vectors.
 *
 * @namespace The SpiderGL.Math.Vec4 namespace defines operations on 4-dimensional vectors.
 */
SpiderGL.Math.Vec4 = { };

/**
 * Duplicates the input 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = v[i] (same as v.slice(0, 4)).
 */
SpiderGL.Math.Vec4.dup = function (v) {
	return v.slice(0, 4);
}

/**
 * Creates a 4-dimensional vector initialized with a scalar.
 *
 * @param {number} s The input scalar.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = s.
 */
SpiderGL.Math.Vec4.scalar = function (s) {
	return [s, s, s, s];
}

/**
 * Creates a 4-dimensional vector initialized with zero.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = 0.
 */
SpiderGL.Math.Vec4.zero = function () {
	return [0, 0, 0, 0];
}

/**
 * Creates a 4-dimensional vector initialized with one.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = 1.0.
 */
SpiderGL.Math.Vec4.one = function () {
	return [1, 1, 1, 1];
}

/**
 * Creates a 4-dimensional vector initialized with SpiderGL.Math.MAX_NUMBER.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = SpiderGL.Math.MAX_NUMBER.
 */
SpiderGL.Math.Vec4.maxNumber = function () {
	return [SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER, SpiderGL.Math.MAX_NUMBER];
}

/**
 * Creates a 4-dimensional vector initialized with SpiderGL.Math.MIN_NUMBER.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = SpiderGL.Math.MIN_NUMBER.
 */
SpiderGL.Math.Vec4.minNumber = function () {
	return [SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER, SpiderGL.Math.MIN_NUMBER];
}

/**
 * Creates a 2-dimensional vector from a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 2-dimensional vector r equal to v with the 3rd and 4th components dropped.
 */
SpiderGL.Math.Vec4.to2 = function (v) {
	return [v[0], v[1]];
}

/**
 * Creates a 3-dimensional vector from a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 3-dimensional vector r equal to v with the 4th component dropped.
 */
SpiderGL.Math.Vec4.to3 = function (v) {
	return [v[0], v[1], v[2]];
}

/**
 * Component-wise negation of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = -v[i].
 */
SpiderGL.Math.Vec4.neg = function (v) {
	return [-v[0], -v[1], -v[2], -v[3]];
}

/**
 * Component-wise addition of two 4-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec4.add = function (u, v) {
	return [u[0]+v[0], u[1]+v[1], u[2]+v[2], u[3]+v[3]];
}

/**
 * Component-wise addition of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = v[i] + s.
 */
SpiderGL.Math.Vec4.adds = function (v, s) {
	return [v[0]+s, v[1]+s, v[2]+s, v[3]+s];
}

/**
 * Component-wise subtraction of two 4-dimensional vectors.
 *
 * @param {array} u The first subtraction operand.
 * @param {array} v The second subtraction operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec4.sub = function (u, v) {
	return [u[0]-v[0], u[1]-v[1], u[2]-v[2], u[3]-v[3]];
}

/**
 * Component-wise subtraction of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = v[i] - s.
 */
SpiderGL.Math.Vec4.subs = function (v, s) {
	return [v[0]-s, v[1]-s, v[2]-s, v[3]-s];
}

/**
 * Component-wise subtraction of a scalar and a 4-dimensional.
 *
 * @param {number} s The scalar subtraction operand.
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = s - v[i].
 */
SpiderGL.Math.Vec4.ssub = function (s, v) {
	return [s-v[0], s-v[1], s-v[2], s-v[3]];
}

/**
 * Component-wise multiplication of two 4-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec4.mul = function (u, v) {
	return [u[0]*v[0], u[1]*v[1], u[2]*v[2], u[3]*v[3]];
}

/**
 * Component-wise multiplication of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The vector multiplication operand.
 * @param {number} s The scalar multiplication operand.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = v[i] * s.
 */
SpiderGL.Math.Vec4.muls = function (v, s) {
	return [v[0]*s, v[1]*s, v[2]*s, v[3]*s];
}

/**
 * Component-wise division of two 4-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec4.div = function (u, v) {
	return [u[0]/v[0], u[1]/v[1], u[2]/v[2], u[3]/v[3]];
}

/**
 * Component-wise division of a 4-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = v[i] / s.
 */
SpiderGL.Math.Vec4.divs = function (v, s) {
	return [v[0]/s, v[1]/s, v[2]/s, v[3]/s];
}

/**
 * Component-wise division of a scalar by a 4-dimensional vector.
 *
 * @param {number} s The denominator scalar.
 * @param {array} v The numerator vector.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = s / v[i].
 */
SpiderGL.Math.Vec4.sdiv = function (s, v) {
	return [s/v[0], s/v[1], s/v[2], s/v[3]];
}

/**
 * Component-wise reciprocal of a 4-dimensional vector.
 *
 * @param {array} v The input (denominator) vector.
 *
 * @returns {array} A 4-dimensionals vector r, where r[i] = 1.0 / v[i].
 */
SpiderGL.Math.Vec4.rcp = function (v) {
	return [1.0/v[0], 1.0/v[1], 1.0/v[2], 1.0/v[3]];
}

/**
 * Dot product of two 4-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 *
 * @returns {number} The dot product of u and v.
 */
SpiderGL.Math.Vec4.dot = function (u, v) {
	return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2] + u[3]*v[3]);
}

/**
 * Cross product of three 4-dimensional vectors.
 *
 * @param {array} u The first vector operand.
 * @param {array} v The second vector operand.
 * @param {array} w The third vector operand.
 *
 * @returns {array} A 4-dimensional vector equal to the cross product of u, v and w.
 */
SpiderGL.Math.Vec4.cross = function (u, v, w) {
	var a = v[0]*w[1] - v[1]*w[0];
	var b = v[0]*w[2] - v[2]*w[0];
	var c = v[0]*w[3] - v[3]*w[0];
	var d = v[1]*w[2] - v[2]*w[1];
	var e = v[1]*w[3] - v[3]*w[1];
	var f = v[2]*w[3] - v[3]*w[2];

	return [
		u[1]*f - u[2]*e + u[3]*d,
		u[0]*f + u[2]*c - u[3]*b,
		u[0]*e - u[1]*c + u[3]*a,
		u[0]*d + u[1]*b - u[2]*a
	];
}

/**
 * Squared length of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The squared length of v, same as the dot product of v with itself.
 */
SpiderGL.Math.Vec4.sqLength = function (v) {
	return SpiderGL.Math.Vec4.dot(v, v);
}

/**
 * Length of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {number} The length of v.
 */
SpiderGL.Math.Vec4.length = function (v) {
	return SpiderGL.Math.sqrt(SpiderGL.Math.Vec4.sqLength(v));
}

/**
 * Creates a normalized 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r representing the normalized v, where r[i] = v[i] / {@link SpiderGL.Math.Vec4.length}(v).
 */
SpiderGL.Math.Vec4.normalize = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec4.length(v);
	return SpiderGL.Math.Vec4.muls(v, f);
}

/**
 * Projects a homogeneous 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = v[i] / v[3].
 */
SpiderGL.Math.Vec4.project = function (v) {
	var f = 1.0 / v[3];
	return [v[0]*f, v[1]*f, v[2]*f, 1.0];
}

/**
 * Component-wise absolute value of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.abs}(v[i]).
 */
SpiderGL.Math.Vec4.abs = function (v) {
	return [SpiderGL.Math.abs(v[0]), SpiderGL.Math.abs(v[1]), SpiderGL.Math.abs(v[2]), SpiderGL.Math.abs(v[3])];
}

/**
 * Component-wise arccosine of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.acos}(v[i]), in radians.
 */
SpiderGL.Math.Vec4.acos = function (v) {
	return [SpiderGL.Math.acos(v[0]), SpiderGL.Math.acos(v[1]), SpiderGL.Math.acos(v[2]), SpiderGL.Math.acos(v[3])];
}

/**
 * Component-wise arcsine of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.asin}(v[i]), in radians.
 */
SpiderGL.Math.Vec4.asin = function (v) {
	return [SpiderGL.Math.asin(v[0]), SpiderGL.Math.asin(v[1]), SpiderGL.Math.asin(v[2]), SpiderGL.Math.asin(v[3])];
}

/**
 * Component-wise arctangent of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan}(v[i]), between -PI/2 and PI/2 radians.
 */
SpiderGL.Math.Vec4.atan = function (v) {
	return [SpiderGL.Math.atan(v[0]), SpiderGL.Math.atan(v[1]), SpiderGL.Math.atan(v[2]), SpiderGL.Math.atan(v[3])];
}

/**
 * Component-wise arctangent of the quotient of two 4-dimensional vectors.
 *
 * @param {array} y The numerator vector.
 * @param {array} x The denominator vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.atan2}(y[i], x[i]), between PI and -PI radians.
 */
SpiderGL.Math.Vec4.atan2 = function (y, x) {
	return [SpiderGL.Math.atan2(y[0], x[0]), SpiderGL.Math.atan2(y[1], x[1]), SpiderGL.Math.atan2(y[2], x[2]), SpiderGL.Math.atan2(y[3], x[3])];
}

/**
 * Component-wise ceil of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.ceil}(v[i]).
 */
SpiderGL.Math.Vec4.ceil = function (v) {
	return [SpiderGL.Math.ceil(v[0]), SpiderGL.Math.ceil(v[1]), SpiderGL.Math.ceil(v[2]), SpiderGL.Math.ceil(v[3])];
}

/**
 * Component-wise clamp of a 4-dimensional vector with vector bounds.
 *
 * @param {array} v The input vector.
 * @param {array} min The lower 4-dimensional bound.
 * @param {array} max The upper 4-dimensional bound.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.clamp}(v[i], min[i], max[i]).
 */
SpiderGL.Math.Vec4.clamp = function (v, min, max) {
	return [SpiderGL.Math.clamp(v[0], min[0], max[0]), SpiderGL.Math.clamp(v[1], min[1], max[1]), SpiderGL.Math.clamp(v[2], min[2], max[2]), SpiderGL.Math.clamp(v[3], min[3], max[3])];
}

/**
 * Component-wise cosine of a 4-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.cos}(v[i]).
 */
SpiderGL.Math.Vec4.cos = function (v) {
	return [SpiderGL.Math.cos(v[0]), SpiderGL.Math.cos(v[1]), SpiderGL.Math.cos(v[2]), SpiderGL.Math.cos(v[3])];
}

/**
 * Component-wise conversion of a 4-dimensional vector from degrees to radians.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.degToRad}(v[i]).
 */
SpiderGL.Math.Vec4.degToRad = function (v) {
	return [SpiderGL.Math.degToRad(v[0]), SpiderGL.Math.degToRad(v[1]), SpiderGL.Math.degToRad(v[2]), SpiderGL.Math.degToRad(v[3])];
}

/**
 * Component-wise exponential of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.exp}(v[i]).
 */
SpiderGL.Math.Vec4.exp = function (v) {
	return [SpiderGL.Math.exp(v[0]), SpiderGL.Math.exp(v[1]), SpiderGL.Math.exp(v[2]), SpiderGL.Math.exp(v[3])];
}

/**
 * Component-wise floor of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.floor}(v[i]).
 */
SpiderGL.Math.Vec4.floor = function (v) {
	return [SpiderGL.Math.floor(v[0]), SpiderGL.Math.floor(v[1]), SpiderGL.Math.floor(v[2]), SpiderGL.Math.floor(v[3])];
}

/**
 * Linear interpolation between two 4-dimensional vectors.
 *
 * @param {array} u The start interpolation bound.
 * @param {array} v The stop interpolation bound.
 * @param {number} t The interpolation factor, between 0 and 1.
 *
 * @returns {array} The interpolated vector r, where r[i] = (1-t)*u[i] + t*v[i].
 */
SpiderGL.Math.Vec4.lerp = function (u, v, t) {
	return [
		SpiderGL.Math.lerp(u[0], v[0], t),
		SpiderGL.Math.lerp(u[1], v[1], t),
		SpiderGL.Math.lerp(u[2], v[2], t),
		SpiderGL.Math.lerp(u[3], v[3], t)
	];
}

/**
 * Component-wise natural (base E) logarithm of a 4-dimensional vector.
 * Same as {@link SpiderGL.Math.Vec4.log}.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.ln}(v[i]).
 */
SpiderGL.Math.Vec4.ln = function (v) {
	return [SpiderGL.Math.ln(v[0]), SpiderGL.Math.ln(v[1]), SpiderGL.Math.ln(v[2]), SpiderGL.Math.ln(v[3])];
}

/**
 * Component-wise natural (base E) logarithm of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.log}(v[i]).
 */
SpiderGL.Math.Vec4.log = function (v) {
	return [SpiderGL.Math.log(v[0]), SpiderGL.Math.log(v[1]), SpiderGL.Math.log(v[2]), SpiderGL.Math.log(v[3])];
}

/**
 * Component-wise base 2 logarithm of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.log2}(v[i]).
 */
SpiderGL.Math.Vec4.log2 = function (v) {
	return [SpiderGL.Math.log2(v[0]), SpiderGL.Math.log2(v[1]), SpiderGL.Math.log2(v[2]), SpiderGL.Math.log2(v[3])];
}

/**
 * Component-wise base 10 logarithm of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.log10}(v[i]).
 */
SpiderGL.Math.Vec4.log10 = function (v) {
	return [SpiderGL.Math.log10(v[0]), SpiderGL.Math.log10(v[1]), SpiderGL.Math.log10(v[2]), SpiderGL.Math.log10(v[3])];
}

/**
 * Component-wise maximum of two 4-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.max}(u[i], v[i]).
 */
SpiderGL.Math.Vec4.max = function (u, v) {
	return [SpiderGL.Math.max(u[0], v[0]), SpiderGL.Math.max(u[1], v[1]), SpiderGL.Math.max(u[2], v[2]), SpiderGL.Math.max(u[3], v[3])];
}

/**
 * Component-wise minimum of two 4-dimensional vectors.
 *
 * @param {array} u The first vector.
 * @param {array} v The second vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.min}(u[i], v[i]).
 */
SpiderGL.Math.Vec4.min = function (u, v) {
	return [SpiderGL.Math.min(u[0], v[0]), SpiderGL.Math.min(u[1], v[1]), SpiderGL.Math.min(u[2], v[2]), SpiderGL.Math.min(u[3], v[3])];
}

/**
 * Component-wise power of two 4-dimensional vectors.
 *
 * @param {array} u The base vector.
 * @param {array} v The exponent vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(u[i], v[i]).
 */
SpiderGL.Math.Vec4.pow = function (u, v) {
	return [SpiderGL.Math.pow(u[0], v[0]), SpiderGL.Math.pow(u[1], v[1]), SpiderGL.Math.pow(u[2], v[2]), SpiderGL.Math.pow(u[3], v[3])];
}

/**
 * Component-wise conversion of a 4-dimensional vector from radians to degrees.
 *
 * @param {array} v The input vector, in degrees.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.pow}(v[i]).
 */
SpiderGL.Math.Vec4.radToDeg = function (v) {
	return [SpiderGL.Math.radToDeg(v[0]), SpiderGL.Math.radToDeg(v[1]), SpiderGL.Math.radToDeg(v[2]), SpiderGL.Math.radToDeg(v[3])];
}

/**
 * Creates a random 4-dimensional vector between 0 and 1.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.random}().
 */
SpiderGL.Math.Vec4.random = function () {
	return [SpiderGL.Math.random(), SpiderGL.Math.random(), SpiderGL.Math.random(), SpiderGL.Math.random()];
}

/**
 * Creates a random 4-dimensional vector between 0 and 1.
 * Same as {@link SpiderGL.Math.Vec4.random}.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.random01}().
 */
SpiderGL.Math.Vec4.random01 = function () {
	return [SpiderGL.Math.random01(), SpiderGL.Math.random01(), SpiderGL.Math.random01(), SpiderGL.Math.random01()];
}

/**
 * Creates a random 4-dimensional vector between -1 and 1.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.random11}().
 */
SpiderGL.Math.Vec4.random11 = function () {
	return [SpiderGL.Math.random11(), SpiderGL.Math.random11(), SpiderGL.Math.random11(), SpiderGL.Math.random11()];
}

/**
 * Creates a random 4-dimensional vector inside a range.
 *
 * @param {array} min The range vector lower bound.
 * @param {array} max The range vector upper bound.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.randomRange}(min[i], max[i]).
 */
SpiderGL.Math.Vec4.randomRange = function (min, max) {
	return [SpiderGL.Math.randomRange(min[0], max[0]), SpiderGL.Math.randomRange(min[1], max[1]), SpiderGL.Math.randomRange(min[2], max[2]), SpiderGL.Math.randomRange(min[3], max[3])];
}

/**
 * Component-wise round of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.round}(v[i]).
 */
SpiderGL.Math.Vec4.round = function (v) {
	return [SpiderGL.Math.round(v[0]), SpiderGL.Math.round(v[1]), SpiderGL.Math.round(v[2]), SpiderGL.Math.round(v[3])];
}

/**
 * Component-wise sine of a 4-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.sin}(v[i]).
 */
SpiderGL.Math.Vec4.sin = function (v) {
	return [SpiderGL.Math.sin(v[0]), SpiderGL.Math.sin(v[1]), SpiderGL.Math.sin(v[2]), SpiderGL.Math.sin(v[3])];
}

/**
 * Component-wise square root of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.sqrt}(v[i]).
 */
SpiderGL.Math.Vec4.sqrt = function (v) {
	return [SpiderGL.Math.sqrt(v[0]), SpiderGL.Math.sqrt(v[1]), SpiderGL.Math.sqrt(v[2]), SpiderGL.Math.sqrt(v[3])];
}

/**
 * Component-wise tangent root of a 4-dimensional vector.
 *
 * @param {array} v The input vector, in radians.
 *
 * @returns {array} A 4-dimensional vector r, where r[i] = {@link SpiderGL.Math.tan}(v[i]).
 */
SpiderGL.Math.Vec4.tan = function (v) {
	return [SpiderGL.Math.tan(v[0]), SpiderGL.Math.tan(v[1]), SpiderGL.Math.tan(v[2]), SpiderGL.Math.tan(v[3])];
}

/**
 * In-place component-wise copy of two 4-dimensional vectors.
 *
 * @param {array} u The destination vector.
 * @param {array} v The source vector.
 *
 * @returns {array} The destination vector u, where u[i] = v[i].
 */
SpiderGL.Math.Vec4.copy$ = function (u, v) {
	u[0] = v[0];
	u[1] = v[1];
	u[2] = v[2];
	u[3] = v[3];
	return u;
}

/**
 * In-place component-wise negation of a 4-dimensional vector.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = -v[i].
 */
SpiderGL.Math.Vec4.neg$ = function (v) {
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
	v[3] = -v[3];
	return v;
}

/**
 * In-place component-wise addition of two 4-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] + v[i].
 */
SpiderGL.Math.Vec4.add$ = function (u, v) {
	u[0] += v[0];
	u[1] += v[1];
	u[2] += v[2];
	u[3] += v[3];
	return u;
}

/**
 * In-place component-wise addition of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The vector addition operand.
 * @param {number} s The scalar addition operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] + s.
 */
SpiderGL.Math.Vec4.adds$ = function (v, s) {
	v[0] += s;
	v[1] += s;
	v[2] += s;
	v[3] += s;
	return v;
}

/**
 * In-place component-wise subtraction of two 4-dimensional vectors.
 *
 * @param {array} u The first addition operand.
 * @param {array} v The second addition operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] - v[i].
 */
SpiderGL.Math.Vec4.sub$ = function (u, v) {
	u[0] -= v[0];
	u[1] -= v[1];
	u[2] -= v[2];
	u[3] -= v[3];
	return u;
}

/**
 * In-place component-wise subtraction of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The vector subtraction operand.
 * @param {number} s The scalar subtraction operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] - s.
 */
SpiderGL.Math.Vec4.subs$ = function (v, s) {
	v[0] -= s;
	v[1] -= s;
	v[2] -= s;
	v[3] -= s;
	return v;
}

/**
 * In-place component-wise subtraction of a scalar and a 4-dimensional vector.
 *
 * @param {number} s The scalar subtraction operand
 * @param {array} v The vector subtraction operand.
 *
 * @returns {array} The input vector v, where v[i] = s - v[i].
 */
SpiderGL.Math.Vec4.ssub$ = function (s, v) {
	v[0] = s - v[0];
	v[1] = s - v[1];
	v[2] = s - v[2];
	v[3] = s - v[3];
	return v;
}

/**
 * In-place component-wise multiplication of two 4-dimensional vectors.
 *
 * @param {array} u The first multiplication operand.
 * @param {array} v The second multiplication operand
 *
 * @returns {array} The input vector u, where u[i] = u[i] * v[i].
 */
SpiderGL.Math.Vec4.mul$ = function (u, v) {
	u[0] *= v[0];
	u[1] *= v[1];
	u[2] *= v[2];
	u[3] *= v[3];
	return u;
}

/**
 * In-place component-wise multiplication of a 4-dimensional vector and a scalar.
 *
 * @param {array} v The first multiplication operand.
 * @param {number} s The second multiplication operand
 *
 * @returns {array} The input vector v, where v[i] = v[i] * s.
 */
SpiderGL.Math.Vec4.muls$ = function (v, s) {
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
	v[3] *= s;
	return v;
}

/**
 * In-place component-wise division of two 4-dimensional vectors.
 *
 * @param {array} u The numerator vector.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector u, where u[i] = u[i] / v[i].
 */
SpiderGL.Math.Vec4.div$ = function (u, v) {
	u[0] /= v[0];
	u[1] /= v[1];
	u[2] /= v[2];
	u[3] /= v[3];
	return u;
}

/**
 * In-place component-wise division of a 4-dimensional vector by a scalar.
 *
 * @param {array} v The numerator vector.
 * @param {number} s The scalar denominator.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / s.
 */
SpiderGL.Math.Vec4.divs$ = function (v, s) {
	v[0] /= s;
	v[1] /= s;
	v[2] /= s;
	v[3] /= s;
	return v;
}

/**
 * In-place component-wise division of a scalar by a 4-dimensional.
 *
 * @param {number} s The scalar numerator.
 * @param {array} v The denominator vector.
 *
 * @returns {array} The input vector v, where v[i] = s / v[i].
 */
SpiderGL.Math.Vec4.sdiv$ = function (v, s) {
	v[0] = s / v[0];
	v[1] = s / v[1];
	v[2] = s / v[2];
	v[3] = s / v[3];
	return v;
}

/**
 * In-place 4-dimensional vector normalization.
 *
 * @param {array} v The input vector.
 *
 * @returns {array} The input vector v, where v[i] = v[i] / {@link SpiderGL.Math.Vec4.length}(v)
 */
SpiderGL.Math.Vec4.normalize$ = function (v) {
	var f = 1.0 / SpiderGL.Math.Vec4.length(v);
	return SpiderGL.Math.Vec4.muls$(v, f);
}

/*---------------------------------------------------------*/



// 3x3 matrix
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Mat3 namespace.
 * The provided functions operate on 3x3 matrices, represented as standard JavaScript arrays of length 9.
 * In general, matrices are considered in column-major format.
 *
 * @namespace The SpiderGL.Math.Mat3 namespace defines operations on 3x3 matrices.
 */
SpiderGL.Math.Mat3 = { };

/**
 * Duplicates the input 3x3 matrix.
 *
 * @param {array} n The input matrix.
 *
 * @returns {array} A 9-component array r, where r[i] = m[i] (same as m.slice(0, 9)).
 */
SpiderGL.Math.Mat3.dup = function (m) {
	return m.slice(0, 9);
}

/**
 * Creates a 3x3 matrix initialized with a scalar.
 *
 * @param {number} s The input scalar.
 *
 * @returns {array} A 9-component array r, where r[i] = s.
 */
SpiderGL.Math.Mat3.scalar = function (s) {
	return [
		s, s, s,
		s, s, s,
		s, s, s
	];
}

/**
 * Creates a 3x3 matrix initialized with zero.
 *
 * @returns {array} A 9-component array r, where r[i] = 0.
 */
SpiderGL.Math.Mat3.zero = function () {
	return [
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	];
}

/**
 * Creates a 3x3 matrix initialized with one.
 *
 * @returns {array} A 9-component array r, where r[i] = 1.
 */
SpiderGL.Math.Mat3.one = function () {
	return [
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	];
}

/**
 * Creates a diagonal 3x3 matrix.
 *
 * @param {array} d A 3-dimensional vector
 *
 * @returns {array} A 9-component array representing a 3x3 matrix with diagonal elements set to d.
 */
SpiderGL.Math.Mat3.diag = function (d) {
	return [
		d[0],    0,    0,
		0,    d[0],    0,
		0,       0, d[0]
	];
}

/**
 * Creates an identity 3x3 matrix.
 *
 * @returns {array} A 9-component array representing an identity 3x3 matrix.
 */
SpiderGL.Math.Mat3.identity = function () {
	return [
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	];
}

/**
 * Extends a 3x3 matrix to a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 16-component array representing a 4x4 matrix r, with the input 3x3 matrix as tue upper-left 3x3 region with [0, 0, 0, 1] as last row and column.
 */
SpiderGL.Math.Mat3.to44 = function (m) {
	return [
		m[0], m[1], m[2], 0,
		m[3], m[4], m[5], 0,
		m[6], m[7], m[8], 0,
		   0,    0,    0, 1
	];
}

/**
 * Pre-multiplies a 2-dimensional vector by a column-major 3x3 matrix.
 *
 * @param {array} m The input column-major 3x3 matrix.
 * @param {array} v The input 2-dimensional vector.
 * @param {number} [z=0] The 3rd component of the input 2-dimensional vector.
 *
 * @return {array} A 2-dimensional vector r, where r = m * v.
 */
SpiderGL.Math.Mat3.mul2 = function (m, v, z) {
	z = (z == undefined) ? (0) : (z);
	return [
		m[0]*v[0] + m[3]*v[1] + m[6]*z,
		m[1]*v[0] + m[4]*v[1] + m[7]*z /* ,
		m[2]*v[0] + m[5]*v[1] + m[8]*z */
	];
}

/**
 * Pre-multiplies a 3-dimensional vector by a column-major 3x3 matrix.
 *
 * @param {array} m The input column-major 3x3 matrix.
 * @param {array} v The input 3-dimensional vector.
 *
 * @return {array} A 3-dimensional vector r, where r = m * v.
 */
SpiderGL.Math.Mat3.mul3 = function (m, v) {
	return [
		m[0]*v[0] + m[3]*v[1] + m[6]*v[2],
		m[1]*v[0] + m[4]*v[1] + m[7]*v[2],
		m[2]*v[0] + m[5]*v[1] + m[8]*v[2]
	];
}
/*---------------------------------------------------------*/



// 4x4 matrix
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Mat4 namespace.
 * The provided functions operate on 4x4 matrices, represented as standard JavaScript arrays of length 16.
 * In general, matrices are considered in column-major format.
 *
 * @namespace The SpiderGL.Math.Mat4 namespace defines operations on 4x4 matrices.
 */
SpiderGL.Math.Mat4 = { };

/**
 * Duplicates the input 4x4 matrix.
 *
 * @param {array} n The input matrix.
 *
 * @returns {array} A 16-component array r, where r[i] = m[i] (same as m.slice(0, 16)).
 */
SpiderGL.Math.Mat4.dup = function (m) {
	return m.slice(0, 16);
}

/**
 * Creates a 4x4 matrix initialized with a scalar.
 *
 * @param {number} s The input scalar.
 *
 * @returns {array} A 16-component array r, where r[i] = s.
 */
SpiderGL.Math.Mat4.scalar = function (s) {
	return [
		s, s, s, s,
		s, s, s, s,
		s, s, s, s,
		s, s, s, s
	];
}

/**
 * Creates a 4x4 matrix initialized with zero.
 *
 * @returns {array} A 16-component array r, where r[i] = 0.
 */
SpiderGL.Math.Mat4.zero = function () {
	return [
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	];
}

/**
 * Creates a 4x4 matrix initialized with one.
 *
 * @returns {array} A 16-component array r, where r[i] = 1.
 */
SpiderGL.Math.Mat4.one = function () {
	return [
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1
	];
}

/**
 * Creates a diagonal 4x4 matrix.
 *
 * @param {array} d A 4-dimensional vector
 *
 * @returns {array} A 16-component array representing a 4x4 matrix with diagonal elements set to d.
 */
SpiderGL.Math.Mat4.diag = function (d) {
	return [
		d[0],    0,    0,    0,
		0,    d[0],    0,    0,
		0,       0, d[0],    0,
		0,       0,    0, d[0]
	];
}

/**
 * Creates an identity 4x4 matrix.
 *
 * @returns {array} A 16-component array representing an identity 4x4 matrix.
 */
SpiderGL.Math.Mat4.identity = function () {
	return [
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	];
}

/**
 * Extracts the upper-left 3x3 matrix from a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 9-component array representing the upper-left 3x3 matrix.
 */
SpiderGL.Math.Mat4.to33 = function (m) {
	return [
		m[ 0], m[ 1], m[ 2],
		m[ 4], m[ 5], m[ 6],
		m[ 8], m[ 9], m[10]
	];
}

/**
 * Gets an element of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} row The element row index.
 * @param {number} col The element column index.
 *
 * @returns {number} The value of the (i-th, j-th) element of m.
 */
SpiderGL.Math.Mat4.elem = function (m, row, col) {
	return m[row+col*4];
}

/**
 * Sets an element of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} row The element row index.
 * @param {number} col The element column index.
 * @param {number} value The element value to set.
 */
SpiderGL.Math.Mat4.elem$ = function (m, row, col, value) {
	m[row+col*4] = value;
}

/**
 * Gets a row of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} row The row index.
 *
 * @returns {array} A 4-component array representing the row-th row of m.
 */
SpiderGL.Math.Mat4.row = function (m, row) {
	return [m[row+0], m[row+4], m[row+8], m[row+12]];
}

/**
 * Sets a row of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} row The row index.
 * @param {array} v A 4-component array that will be copied to the row-th row of m.
 */
SpiderGL.Math.Mat4.row$ = function (m, row, v) {
	m[row+ 0] = v[0];
	m[row+ 4] = v[1];
	m[row+ 8] = v[2];
	m[row+12] = v[3];
}

/**
 * Gets a column of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} col The column index.
 *
 * @returns {array} A 4-component array representing the col-th column of m.
 */
SpiderGL.Math.Mat4.col = function (m, col) {
	var i = col * 4;
	return [m[i+0], m[i+1], m[i+2], m[i+3]];
}

/**
 * Sets a column of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 * @param {number} col The column index.
 * @param {array} v A 4-component array that will be copied to the col-th column of m.
 */
SpiderGL.Math.Mat4.col$ = function (m, col, v) {
	var i = col * 4;
	m[i+0] = v[0];
	m[i+1] = v[1];
	m[i+2] = v[2];
	m[i+3] = v[3];
}

/**
 * Tests whether a 4x4 matrix is the identity matrix.
 *
 * @param {array} m The input matrix.
 *
 * @return {bool} True if the input matrix is the identity matrix, false otherwise.
 */
SpiderGL.Math.Mat4.isIdentity = function (m) {
	return ((m[ 0] === 1) && (m[ 1] === 0) && (m[ 2] === 0) && (m[ 3] === 0) &&
	        (m[ 4] === 0) && (m[ 5] === 1) && (m[ 6] === 0) && (m[ 7] === 0) &&
	        (m[ 8] === 0) && (m[ 9] === 0) && (m[10] === 1) && (m[11] === 0) &&
	        (m[12] === 0) && (m[13] === 0) && (m[14] === 0) && (m[15] === 1));
}

/**
 * Component-wise negation of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @return {array} A 4x4 matrix r, where r[i] = -m[i].
 */
SpiderGL.Math.Mat4.neg = function (m) {
	return [
		-m[ 0], -m[ 1], -m[ 2], -m[ 3],
		-m[ 4], -m[ 5], -m[ 6], -m[ 7],
		-m[ 8], -m[ 9], -m[10], -m[11],
		-m[12], -m[13], -m[14], -m[15]
	];
}

/**
 * Component-wise addition of two 4x4 matrices.
 *
 * @param {array} a The first input matrix.
 * @param {array} b The first input matrix.
 *
 * @return {array} A 4x4 matrix r, where r[i] = a[i] + b[i].
 */
SpiderGL.Math.Mat4.add = function (a, b) {
	return [
		a[ 0]+b[ 0], a[ 1]+b[ 1], a[ 2]+b[ 2], a[ 3]+b[ 3],
		a[ 4]+b[ 4], a[ 5]+b[ 5], a[ 6]+b[ 6], a[ 7]+b[ 7],
		a[ 8]+b[ 8], a[ 9]+b[ 9], a[10]+b[10], a[11]+b[11],
		a[12]+b[12], a[13]+b[13], a[14]+b[14], a[15]+b[15]
	];
}

/**
 * Component-wise addition of two 4x4 matrices.
 *
 * @param {array} a The first input matrix.
 * @param {array} b The first input matrix.
 *
 * @return {array} A 4x4 matrix r, where r[i] = a[i] - b[i].
 */
SpiderGL.Math.Mat4.sub = function (a, b) {
	return [
		a[ 0]-b[ 0], a[ 1]-b[ 1], a[ 2]-b[ 2], a[ 3]-b[ 3],
		a[ 4]-b[ 4], a[ 5]-b[ 5], a[ 6]-b[ 6], a[ 7]-b[ 7],
		a[ 8]-b[ 8], a[ 9]-b[ 9], a[10]-b[10], a[11]-b[11],
		a[12]-b[12], a[13]-b[13], a[14]-b[14], a[15]-b[15]
	];
}

/**
 * Multiplies of two column-major 4x4 matrices.
 *
 * @param {array} a The first input matrix.
 * @param {array} b The first input matrix.
 *
 * @return {array} A 4x4 matrix r, result of matrix multiplication r = a * b.
 */
SpiderGL.Math.Mat4.mul = function (a, b) {
	var a0  = a[ 0], a1  = a[ 1],  a2 = a[ 2], a3  = a[ 3],
	    a4  = a[ 4], a5  = a[ 5],  a6 = a[ 6], a7  = a[ 7],
	    a8  = a[ 8], a9  = a[ 9], a10 = a[10], a11 = a[11],
	    a12 = a[12], a13 = a[13], a14 = a[14], a15 = a[15],

	    b0  = b[ 0], b1  = b[ 1], b2  = b[ 2], b3  = b[ 3],
	    b4  = b[ 4], b5  = b[ 5], b6  = b[ 6], b7  = b[ 7],
	    b8  = b[ 8], b9  = b[ 9], b10 = b[10], b11 = b[11],
	    b12 = b[12], b13 = b[13], b14 = b[14], b15 = b[15];

	return [
		a0*b0  + a4*b1  + a8*b2   + a12*b3,
		a1*b0  + a5*b1  + a9*b2   + a13*b3,
		a2*b0  + a6*b1  + a10*b2  + a14*b3,
		a3*b0  + a7*b1  + a11*b2  + a15*b3,

		a0*b4  + a4*b5  + a8*b6   + a12*b7,
		a1*b4  + a5*b5  + a9*b6   + a13*b7,
		a2*b4  + a6*b5  + a10*b6  + a14*b7,
		a3*b4  + a7*b5  + a11*b6  + a15*b7,

		a0*b8  + a4*b9  + a8*b10  + a12*b11,
		a1*b8  + a5*b9  + a9*b10  + a13*b11,
		a2*b8  + a6*b9  + a10*b10 + a14*b11,
		a3*b8  + a7*b9  + a11*b10 + a15*b11,

		a0*b12 + a4*b13 + a8*b14  + a12*b15,
		a1*b12 + a5*b13 + a9*b14  + a13*b15,
		a2*b12 + a6*b13 + a10*b14 + a14*b15,
		a3*b12 + a7*b13 + a11*b14 + a15*b15
	];
}

/**
 * Component-wise multiplication of a 4x4 matrix and a scalar.
 *
 * @param {array} m The matrix operand.
 * @param {number} s The scalar operand.
 *
 * @returns {array} A 4x4 matrix r, where r[i] = m[i] * s.
 */
SpiderGL.Math.Mat4.muls = function (m, s) {
	return [
		m[ 0]*s, m[ 1]*s, m[ 2]*s, m[ 3]*s,
		m[ 4]*s, m[ 5]*s, m[ 6]*s, m[ 7]*s,
		m[ 8]*s, m[ 9]*s, m[10]*s, m[11]*s,
		m[12]*s, m[13]*s, m[14]*s, m[15]*s
	];
}

/**
 * Pre-multiplies a 3-dimensional vector by a column-major 4x4 matrix.
 *
 * @param {array} m The input column-major 4x4 matrix.
 * @param {array} v The input 3-dimensional vector.
 * @param {number} [w=1] The 4th component of the input 3-dimensional vector.
 *
 * @return {array} A 3-dimensional vector r, where r = m * v.
 */
SpiderGL.Math.Mat4.mul3 = function (m, v, w) {
	w = (w == undefined) ? (1) : (w);
	return [
		m[ 0]*v[0] + m[ 4]*v[1] + m[ 8]*v[2] + m[12]*w,
		m[ 1]*v[0] + m[ 5]*v[1] + m[ 9]*v[2] + m[13]*w,
		m[ 2]*v[0] + m[ 6]*v[1] + m[10]*v[2] + m[14]*w /* ,
		m[ 3]*v[0] + m[ 7]*v[1] + m[11]*v[2] + m[15]*w */
	];
}

/**
 * Pre-multiplies a 4-dimensional vector by a column-major 4x4 matrix.
 *
 * @param {array} m The input column-major 4x4 matrix.
 * @param {array} v The input 4-dimensional vector.
 *
 * @return {array} A 4-dimensional vector r, where r = m * v.
 */
SpiderGL.Math.Mat4.mul4 = function (m, v) {
	return [
		m[ 0]*v[0] + m[ 4]*v[1] + m[ 8]*v[2] + m[12]*v[3],
		m[ 1]*v[0] + m[ 5]*v[1] + m[ 9]*v[2] + m[13]*v[3],
		m[ 2]*v[0] + m[ 6]*v[1] + m[10]*v[2] + m[14]*v[3],
		m[ 3]*v[0] + m[ 7]*v[1] + m[11]*v[2] + m[15]*v[3]
	];
}

/**
 * Component-wise reciprocal of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 4x4 matrix r, where r[i] = 1 / m[i].
 */
SpiderGL.Math.Mat4.rcp = function (m) {
	return [
		1/m[ 0], 1/m[ 1], 1/m[ 2], 1/m[ 3],
		1/m[ 4], 1/m[ 5], 1/m[ 6], 1/m[ 7],
		1/m[ 8], 1/m[ 9], 1/m[10], 1/m[11],
		1/m[12], 1/m[13], 1/m[14], 1/m[15]
	];
}

/**
 * Component-wise multiplication of two 4x4 matrices.
 *
 * @param {array} a The first matrix.
 * @param {array} b The second matrix.
 *
 * @returns {array} A 4x4 matrix r, where r[i] = a[i] * b[i].
 */
SpiderGL.Math.Mat4.compMul = function (a, b) {
	return [
		a[ 0]*b[ 0], a[ 1]*b[ 1], a[ 2]*b[ 2], a[ 3]*b[ 3],
		a[ 4]*b[ 4], a[ 5]*b[ 5], a[ 6]*b[ 6], a[ 7]*b[ 7],
		a[ 8]*b[ 8], a[ 9]*b[ 9], a[10]*b[10], a[11]*b[11],
		a[12]*b[12], a[13]*b[13], a[14]*b[14], a[15]*b[15]
	];
}

/**
 * Component-wise division of two 4x4 matrices.
 *
 * @param {array} a The first matrix.
 * @param {array} b The second matrix.
 *
 * @returns {array} A 4x4 matrix r, where r[i] = a[i] / b[i].
 */
SpiderGL.Math.Mat4.compDiv = function (a, b) {
	return [
		a[ 0]/b[ 0], a[ 1]/b[ 1], a[ 2]/b[ 2], a[ 3]/b[ 3],
		a[ 4]/b[ 4], a[ 5]/b[ 5], a[ 6]/b[ 6], a[ 7]/b[ 7],
		a[ 8]/b[ 8], a[ 9]/b[ 9], a[10]/b[10], a[11]/b[11],
		a[12]/b[12], a[13]/b[13], a[14]/b[14], a[15]/b[15]
	];
}

/**
 * Creates the transpose of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 4x4 matrix representing the transpose of m.
 */
SpiderGL.Math.Mat4.transpose = function (m) {
	return [
		m[ 0], m[ 4], m[ 8], m[12],
		m[ 1], m[ 5], m[ 9], m[13],
		m[ 2], m[ 6], m[10], m[14],
		m[ 3], m[ 7], m[11], m[15]
	];
}

/**
 * Calculates the determinant of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {number} The determinant of m.
 */
SpiderGL.Math.Mat4.determinant = function (m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15];

	return (m12 * m9 * m6 * m3 - m8 * m13 * m6 * m3 - m12 * m5 * m10 * m3 + m4 * m13 * m10 * m3 +
	        m8 * m5 * m14 * m3 - m4 * m9 * m14 * m3 - m12 * m9 * m2 * m7 + m8 * m13 * m2 * m7 +
	        m12 * m1 * m10 * m7 - m0 * m13 * m10 * m7 - m8 * m1 * m14 * m7 + m0 * m9 * m14 * m7 +
	        m12 * m5 * m2 * m11 - m4 * m13 * m2 * m11 - m12 * m1 * m6 * m11 + m0 * m13 * m6 * m11 +
	        m4 * m1 * m14 * m11 - m0 * m5 * m14 * m11 - m8 * m5 * m2 * m15 + m4 * m9 * m2 * m15 +
	        m8 * m1 * m6 * m15 - m0 * m9 * m6 * m15 - m4 * m1 * m10 * m15 + m0 * m5 * m10 * m15);
}

/**
 * Calculates the inverse of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 4x4 matrix representing the inverse of m.
 */
SpiderGL.Math.Mat4.inverse = function (m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15];

	var d = 1 / (
		m12 * m9 * m6 * m3 - m8 * m13 * m6 * m3 - m12 * m5 * m10 * m3 + m4 * m13 * m10 * m3 +
		m8 * m5 * m14 * m3 - m4 * m9 * m14 * m3 - m12 * m9 * m2 * m7 + m8 * m13 * m2 * m7 +
		m12 * m1 * m10 * m7 - m0 * m13 * m10 * m7 - m8 * m1 * m14 * m7 + m0 * m9 * m14 * m7 +
		m12 * m5 * m2 * m11 - m4 * m13 * m2 * m11 - m12 * m1 * m6 * m11 + m0 * m13 * m6 * m11 +
		m4 * m1 * m14 * m11 - m0 * m5 * m14 * m11 - m8 * m5 * m2 * m15 + m4 * m9 * m2 * m15 +
		m8 * m1 * m6 * m15 - m0 * m9 * m6 * m15 - m4 * m1 * m10 * m15 + m0 * m5 * m10 * m15);

	return [
		d * (m9*m14*m7-m13*m10*m7+m13*m6*m11-m5*m14*m11-m9*m6*m15+m5*m10*m15),
		d * (m13*m10*m3-m9*m14*m3-m13*m2*m11+m1*m14*m11+m9*m2*m15-m1*m10*m15),
		d * (m5*m14*m3-m13*m6*m3+m13*m2*m7-m1*m14*m7-m5*m2*m15+m1*m6*m15),
		d * (m9*m6*m3-m5*m10*m3-m9*m2*m7+m1*m10*m7+m5*m2*m11-m1*m6*m11),

		d * (m12*m10*m7-m8*m14*m7-m12*m6*m11+m4*m14*m11+m8*m6*m15-m4*m10*m15),
		d * (m8*m14*m3-m12*m10*m3+m12*m2*m11-m0*m14*m11-m8*m2*m15+m0*m10*m15),
		d * (m12*m6*m3-m4*m14*m3-m12*m2*m7+m0*m14*m7+m4*m2*m15-m0*m6*m15),
		d * (m4*m10*m3-m8*m6*m3+m8*m2*m7-m0*m10*m7-m4*m2*m11+m0*m6*m11),

		d * (m8*m13*m7-m12*m9*m7+m12*m5*m11-m4*m13*m11-m8*m5*m15+m4*m9*m15),
		d * (m12*m9*m3-m8*m13*m3-m12*m1*m11+m0*m13*m11+m8*m1*m15-m0*m9*m15),
		d * (m4*m13*m3-m12*m5*m3+m12*m1*m7-m0*m13*m7-m4*m1*m15+m0*m5*m15),
		d * (m8*m5*m3-m4*m9*m3-m8*m1*m7+m0*m9*m7+m4*m1*m11-m0*m5*m11),

		d * (m12*m9*m6-m8*m13*m6-m12*m5*m10+m4*m13*m10+m8*m5*m14-m4*m9*m14),
		d * (m8*m13*m2-m12*m9*m2+m12*m1*m10-m0*m13*m10-m8*m1*m14+m0*m9*m14),
		d * (m12*m5*m2-m4*m13*m2-m12*m1*m6+m0*m13*m6+m4*m1*m14-m0*m5*m14),
		d * (m4*m9*m2-m8*m5*m2+m8*m1*m6-m0*m9*m6-m4*m1*m10+m0*m5*m10)
	];
}

/**
 * Calculates the inverse transpose of the upper-left 3x3 matrix of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {array} A 3x3 matrix representing the inverse transpose of the upper-left 3x3 matrix of m.
 */
SpiderGL.Math.Mat4.inverseTranspose33 = function (m) {
	var m11 = m[ 0], m21 = m[ 1], m31 = m[ 2],
	    m12 = m[ 4], m22 = m[ 5], m32 = m[ 6],
	    m13 = m[ 8], m23 = m[ 9], m33 = m[10];

	var d =  1 / (m11*(m33*m22 - m32*m23) - m21*(m33*m12 - m32*m13) + m31*(m23*m12 - m22*m13));

	return [
		d * (m33*m22 - m32*m23), d * (m32*m13 - m33*m12), d * (m23*m12 - m22*m13),
		d * (m31*m23 - m33*m21), d * (m33*m11 - m31*m13), d * (m21*m13 - m23*m11),
		d * (m32*m21 - m31*m22), d * (m31*m12 - m32*m11), d * (m22*m11 - m21*m12)
	];
}

/**
 * Calculates the trace (i.e. the sum of the diagonal elements) of a 4x4 matrix.
 *
 * @param {array} m The input matrix.
 *
 * @returns {number} The trace of m.
 */
SpiderGL.Math.Mat4.trace = function (m) {
	return (m[0] + m[5] + m[10] + m[15]);
}

/**
 * Creates a column-major 4x4 translation matrix.
 * The input translation vector will be placed on the 4th column of an identity matrix.
 *
 * @param {array} v A 3-dimensional vector with translation offsets.
 *
 * @returns {array} A column-major 4x4 translation matrix.
 */
SpiderGL.Math.Mat4.translation = function (v) {
	return [
		   1,    0,    0, 0,
		   0,    1,    0, 0,
		   0,    0,    1, 0,
		v[0], v[1], v[2], 1
	];
}

/**
 * Creates a column-major 4x4 rotation matrix.
 * The returned matrix will represent a counter-clockwise rotation about the input axis by the input angle in radians.
 * The input axis need not to be normalized.
 *
 * @param {number} angle The counter-clockwise rotation angle, in radians.
 * @param {array} axis A 3-dimensional vector representing the rotation axis.
 *
 * @returns {array} A column-major 4x4 rotation matrix.
 */
SpiderGL.Math.Mat4.rotationAngleAxis = function (angle, axis) {
	var ax = SpiderGL.Math.Vec3.normalize(axis);
	var s  = SpiderGL.Math.sin(angle);
	var c  = SpiderGL.Math.cos(angle);
	var q   = 1 - c;

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

	return [
		(q * xx) + c,  (q * xy) + zs, (q * zx) - ys, 0,
		(q * xy) - zs, (q * yy) + c,  (q * yz) + xs, 0,
		(q * zx) + ys, (q * yz) - xs, (q * zz) + c,  0,
		            0,             0,            0,  1
	];
}

/**
 * Creates a column-major 4x4 scaling matrix.
 *
 * @param {array} v The scaling amount as a 3-dimensional array.
 *
 * @returns {array} A 4x4 scaling matrix.
 */
SpiderGL.Math.Mat4.scaling = function (v) {
	return [
		v[0],    0,    0, 0,
		   0, v[0],    0, 0,
		   0,    0, v[0], 0,
		   0,    0,    0, 1
	];
}

/**
 * Creates a column-major 4x4 look-at matrix.
 *
 * @param {array} position The viewer's position as a 3-dimensional vector.
 * @param {array} target The viewer's look-at point as a 3-dimensional vector.
 * @param {array} position The viewer's up vector as a 3-dimensional vector.
 *
 * @returns {array} A column-major 4x4 look-at matrix.
 */
SpiderGL.Math.Mat4.lookAt = function (position, target, up) {
	var v = SpiderGL.Math.Vec3.normalize(SpiderGL.Math.Vec3.sub(target, position));
	var u = SpiderGL.Math.Vec3.normalize(up);
	var s = SpiderGL.Math.Vec3.normalize(SpiderGL.Math.Vec3.cross(v, u));

	u = SpiderGL.Math.Vec3.cross(s, v);

	var m = [
		 s[0], u[0], -v[0], 0,
		 s[1], u[1], -v[1], 0,
		 s[2], u[2], -v[2], 0,
		    0,    0,    0,  1
	];

	return SpiderGL.Math.Mat4.translate$(m, SpiderGL.Math.Vec3.neg(position));
}

/**
 * Creates a column-major 4x4 orthographic projection matrix.
 *
 * @param {array} min A 3-component array with the minimum coordinates of the parallel viewing volume.
 * @param {array} max A 3-component array with the maximum coordinates of the parallel viewing volume.
 *
 * @returns {array} A column-major 4x4 orthographic projection matrix.
 */
SpiderGL.Math.Mat4.ortho = function (min, max) {
	var sum = SpiderGL.Math.Vec3.add(max, min);
	var dif = SpiderGL.Math.Vec3.sub(max, min);

	return [
		     2 / dif[0],                 0,           0,      0,
		               0,       2 / dif[1],           0,      0,
		               0,                0, -2 / dif[2],      0,
		-sum[0] / dif[0], -sum[1] / dif[1], -sum[2] / dif[2], 1
	];
}

/**
 * Creates a column-major 4x4 frustum matrix.
 *
 * @param {array} min A 3-component array with the minimum coordinates of the frustum volume.
 * @param {array} max A 3-component array with the maximum coordinates of the frustum volume.
 *
 * @returns {array} A column-major 4x4 frustum matrix.
 */
SpiderGL.Math.Mat4.frustum = function (min, max) {
	var sum = SpiderGL.Math.Vec3.add(max, min);
	var dif = SpiderGL.Math.Vec3.sub(max, min);
	var t   = 2.0 * min[2];

	return [
		     t / dif[0],               0,                     0,  0,
		              0,      t / dif[1],                     0,  0,
		sum[0] / dif[0], sum[1] / dif[1],      -sum[2] / dif[2], -1,
		              0,               0, -t *  max[2] / dif[2],  0
	];
}

/**
 * Creates a column-major 4x4 perspective projection matrix.
 *
 * @param {number} fovY The vertical field-of-view angle, in radians.
 * @param {number} aspectRatio The projection plane aspect ratio.
 * @param {number} zNear The distance of the near clipping plane.
 * @param {number} zFar The distance of the far clipping plane.
 *
 * @returns {array} A column-major 4x4 perspective projection matrix.
 */
SpiderGL.Math.Mat4.perspective = function (fovY, aspectRatio, zNear, zFar) {
	var a = zNear * SpiderGL.Math.tan(fovY / 2);
	var b = a * aspectRatio;

	return SpiderGL.Math.Mat4.frustum([-b, -a, zNear], [b, a, zFar]);
}

/**
 * Copies a 4x4 matrix.
 *
 * @param {array} dst The destination 4x4 matrix.
 * @param {array} src The source 4x4 matrix.
 *
 * @returns {array} The input matrix dst, where dst[i] = src[i].
 */
SpiderGL.Math.Mat4.copy$ = function (dst, src) {
	for (var i=0; i<16; ++i) {
		dst[i] = src[i];
	}
	return dst;
}

/**
 * Sets a 4x4 matrix as the identity matrix.
 *
 * @param {array} m The input 4x4 matrix to be set as identity.
 *
 * @returns {array} The input matrix m.
 */
SpiderGL.Math.Mat4.identity$ = function (m) {
	m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
	m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
	m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
	return m;
}

/**
 * In-place negation of a 4x4 matrix.
 *
 * @param {array} m The input 4x4 matrix.
 *
 * @returns {array} The input matrix m, where m[i] = -m[i].
 */
SpiderGL.Math.Mat4.neg$ = function (m) {
	for (var i=0; i<16; ++i) {
		m[i] = -m[i];
	}
	return m;
}

/**
 * In-place addition of two 4x4 matrices.
 *
 * @param {array} a The first 4x4 input matrix.
 * @param {array} b The second 4x4 input matrix.
 *
 * @returns {array} The input matrix a, where a[i] = a[i] + b[i].
 */
SpiderGL.Math.Mat4.add$ = function (a, b) {
	for (var i=0; i<16; ++i) {
		a[i] += b[i];
	}
	return a;
}

/**
 * In-place subtraction of two 4x4 matrices.
 *
 * @param {array} a The first 4x4 input matrix.
 * @param {array} b The second 4x4 input matrix.
 *
 * @returns {array} The input matrix a, where a[i] = a[i] - b[i].
 */
SpiderGL.Math.Mat4.sub$ = function (a, b) {
	for (var i=0; i<16; ++i) {
		a[i] -= b[i];
	}
	return a;
}

/**
 * In-place multiplication of two 4x4 matrices.
 *
 * @param {array} a The first 4x4 input matrix.
 * @param {array} b The second 4x4 input matrix.
 *
 * @returns {array} The input matrix a, where a = a * b.
 */
SpiderGL.Math.Mat4.mul$ = function (a, b) {
	var a0  = a[ 0], a1  = a[ 1],  a2 = a[ 2], a3  = a[ 3],
	    a4  = a[ 4], a5  = a[ 5],  a6 = a[ 6], a7  = a[ 7],
	    a8  = a[ 8], a9  = a[ 9], a10 = a[10], a11 = a[11],
	    a12 = a[12], a13 = a[13], a14 = a[14], a15 = a[15],

	    b0  = b[ 0], b1  = b[ 1], b2  = b[ 2], b3  = b[ 3],
	    b4  = b[ 4], b5  = b[ 5], b6  = b[ 6], b7  = b[ 7],
	    b8  = b[ 8], b9  = b[ 9], b10 = b[10], b11 = b[11],
	    b12 = b[12], b13 = b[13], b14 = b[14], b15 = b[15];

	a[ 0] = a0*b0 + a4*b1 + a8*b2  + a12*b3;
	a[ 1] = a1*b0 + a5*b1 + a9*b2  + a13*b3;
	a[ 2] = a2*b0 + a6*b1 + a10*b2 + a14*b3;
	a[ 3] = a3*b0 + a7*b1 + a11*b2 + a15*b3;

	a[ 4] = a0*b4 + a4*b5 + a8*b6  + a12*b7;
	a[ 5] = a1*b4 + a5*b5 + a9*b6  + a13*b7;
	a[ 6] = a2*b4 + a6*b5 + a10*b6 + a14*b7;
	a[ 7] = a3*b4 + a7*b5 + a11*b6 + a15*b7;

	a[ 8] = a0*b8 + a4*b9 + a8*b10  + a12*b11;
	a[ 9] = a1*b8 + a5*b9 + a9*b10  + a13*b11;
	a[10] = a2*b8 + a6*b9 + a10*b10 + a14*b11;
	a[11] = a3*b8 + a7*b9 + a11*b10 + a15*b11;

	a[12] = a0*b12 + a4*b13 + a8*b14  + a12*b15;
	a[13] = a1*b12 + a5*b13 + a9*b14  + a13*b15;
	a[14] = a2*b12 + a6*b13 + a10*b14 + a14*b15;
	a[15] = a3*b12 + a7*b13 + a11*b14 + a15*b15;

	return a;
}

/**
 * In-place subtraction of a 4x4 matrix and a scalar.
 *
 * @param {array} m The 4x4 input matrix.
 * @param {number} s The input scalar.
 *
 * @returns {array} The input matrix m, where m[i] = m[i] * s.
 */
SpiderGL.Math.Mat4.muls$ = function (m, s) {
	for (var i=0; i<16; ++i) {
		m[i] *= s;
	}
	return m;
}

/**
 * In-place component-wise multiplication of two 4x4 matrices.
 *
 * @param {array} a The first 4x4 input matrix.
 * @param {array} b The second 4x4 input matrix.
 *
 * @returns {array} The input matrix a, where a[i] = a[i] * b[i].
 */
SpiderGL.Math.Mat4.compMul$ = function (a, b) {
	for (var i=0; i<16; ++i) {
		a[i] *= b[i];
	}
	return a;
}

/**
 * In-place component-wise division of two 4x4 matrices.
 *
 * @param {array} a The first 4x4 input matrix.
 * @param {array} b The second 4x4 input matrix.
 *
 * @returns {array} The input matrix a, where a[i] = a[i] / b[i].
 */
SpiderGL.Math.Mat4.compDiv$ = function (a, b) {
	for (var i=0; i<16; ++i) {
		a[i] /= b[i];
	}
	return a;
}

/**
 * In-place transpose of a 4x4 matrix.
 *
 * @param {array} m The 4x4 input matrix.
 *
 * @returns {array} The transposed input matrix m.
 */
SpiderGL.Math.Mat4.transpose$ = function (m) {
	var t;
	t = m[ 1]; m[ 1] = m[ 4]; m[ 4] = t;
	t = m[ 2]; m[ 2] = m[ 8]; m[ 8] = t;
	t = m[ 3]; m[ 3] = m[12]; m[12] = t;
	t = m[ 6]; m[ 6] = m[ 9]; m[ 9] = t;
	t = m[ 7]; m[ 7] = m[13]; m[13] = t;
	t = m[11]; m[11] = m[14]; m[14] = t;
	return m;
}

/**
 * In-place inversion of a 4x4 matrix.
 *
 * @param {array} m The 4x4 input matrix.
 *
 * @returns {array} The inverted input matrix m.
 */
SpiderGL.Math.Mat4.invert$ = function (m) {
	var m0  = m[ 0], m1  = m[ 1], m2  = m[ 2], m3  = m[ 3],
	    m4  = m[ 4], m5  = m[ 5], m6  = m[ 6], m7  = m[ 7],
	    m8  = m[ 8], m9  = m[ 9], m10 = m[10], m11 = m[11],
	    m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15];

	var d = 1.0 / (
		m12 * m9 * m6 * m3 - m8 * m13 * m6 * m3 - m12 * m5 * m10 * m3 + m4 * m13 * m10 * m3 +
		m8 * m5 * m14 * m3 - m4 * m9 * m14 * m3 - m12 * m9 * m2 * m7 + m8 * m13 * m2 * m7 +
		m12 * m1 * m10 * m7 - m0 * m13 * m10 * m7 - m8 * m1 * m14 * m7 + m0 * m9 * m14 * m7 +
		m12 * m5 * m2 * m11 - m4 * m13 * m2 * m11 - m12 * m1 * m6 * m11 + m0 * m13 * m6 * m11 +
		m4 * m1 * m14 * m11 - m0 * m5 * m14 * m11 - m8 * m5 * m2 * m15 + m4 * m9 * m2 * m15 +
		m8 * m1 * m6 * m15 - m0 * m9 * m6 * m15 - m4 * m1 * m10 * m15 + m0 * m5 * m10 * m15);

	m[ 0] = d * (m9*m14*m7-m13*m10*m7+m13*m6*m11-m5*m14*m11-m9*m6*m15+m5*m10*m15);
	m[ 1] = d * (m13*m10*m3-m9*m14*m3-m13*m2*m11+m1*m14*m11+m9*m2*m15-m1*m10*m15);
	m[ 2] = d * (m5*m14*m3-m13*m6*m3+m13*m2*m7-m1*m14*m7-m5*m2*m15+m1*m6*m15);
	m[ 3] = d * (m9*m6*m3-m5*m10*m3-m9*m2*m7+m1*m10*m7+m5*m2*m11-m1*m6*m11);

	m[ 4] = d * (m12*m10*m7-m8*m14*m7-m12*m6*m11+m4*m14*m11+m8*m6*m15-m4*m10*m15);
	m[ 5] = d * (m8*m14*m3-m12*m10*m3+m12*m2*m11-m0*m14*m11-m8*m2*m15+m0*m10*m15);
	m[ 6] = d * (m12*m6*m3-m4*m14*m3-m12*m2*m7+m0*m14*m7+m4*m2*m15-m0*m6*m15);
	m[ 7] = d * (m4*m10*m3-m8*m6*m3+m8*m2*m7-m0*m10*m7-m4*m2*m11+m0*m6*m11);

	m[ 8] = d * (m8*m13*m7-m12*m9*m7+m12*m5*m11-m4*m13*m11-m8*m5*m15+m4*m9*m15);
	m[ 9] = d * (m12*m9*m3-m8*m13*m3-m12*m1*m11+m0*m13*m11+m8*m1*m15-m0*m9*m15);
	m[10] = d * (m4*m13*m3-m12*m5*m3+m12*m1*m7-m0*m13*m7-m4*m1*m15+m0*m5*m15);
	m[11] = d * (m8*m5*m3-m4*m9*m3-m8*m1*m7+m0*m9*m7+m4*m1*m11-m0*m5*m11);

	m[12] = d * (m12*m9*m6-m8*m13*m6-m12*m5*m10+m4*m13*m10+m8*m5*m14-m4*m9*m14);
	m[13] = d * (m8*m13*m2-m12*m9*m2+m12*m1*m10-m0*m13*m10-m8*m1*m14+m0*m9*m14);
	m[14] = d * (m12*m5*m2-m4*m13*m2-m12*m1*m6+m0*m13*m6+m4*m1*m14-m0*m5*m14);
	m[15] = d * (m4*m9*m2-m8*m5*m2+m8*m1*m6-m0*m9*m6-m4*m1*m10+m0*m5*m10);

	return m;
}

/**
 * In-place column-major translation of a 4x4 matrix.
 *
 * @param {array} m The 4x4 input matrix.
 * @param {array} v The 3-dimensional translation vector.
 *
 * @returns {array} The translated input matrix m with the same result as m = m * t, where t is a translation matrix.
 */
SpiderGL.Math.Mat4.translate$ = function (m, v) {
	var x = v[0],
	    y = v[1],
	    z = v[2];

	m[12] = m[ 0]*x + m[ 4]*y + m[ 8]*z + m[12];
	m[13] = m[ 1]*x + m[ 5]*y + m[ 9]*z + m[13];
	m[14] = m[ 2]*x + m[ 6]*y + m[10]*z + m[14];
	m[15] = m[ 3]*x + m[ 7]*y + m[11]*z + m[15];

	return m;
}

/**
 * In-place column-major rotation of a 4x4 matrix.
 * The input matrix m will be post-multiplied by a matrix r representing a counter-clockwise rotation about the input axis by the input angle in radians.
 * The input axis need not to be normalized.
 *
 * @param {array} m The input 4x4 matrix.
 * @param {number} angle The counter-clockwise rotation angle, in radians.
 * @param {array} axis A 3-dimensional vector representing the rotation axis.
 *
 * @returns {array} The rotated input matrix m with the same result as m = m * r, where r is a rotation matrix.
 */
SpiderGL.Math.Mat4.rotateAngleAxis$ = function (m, angle, axis) {
	var r = SpiderGL.Math.Mat4.rotationAngleAxis(angle, axis);
	return SpiderGL.Math.Mat4.mul$(m, r);
}

/**
 * In-place column-major scaling of a 4x4 matrix.
 *
 * @param {array} m The 4x4 input matrix.
 * @param {array} v The scaling amount as a 3-dimensional array.
 *
 * @returns {array} The scaled input matrix m with the same result as m = m * s, where s is a scaling matrix.
 */
SpiderGL.Math.Mat4.scale$ = function (m, v) {
	var x = v[0],
	    y = v[1],
	    z = v[2];

	m[ 0] *= x; m[ 1] *= x; m[ 2] *= x; m[ 3] *= x;
	m[ 4] *= y; m[ 5] *= y; m[ 6] *= y; m[ 7] *= y;
	m[ 8] *= z; m[ 9] *= z; m[10] *= z; m[11] *= z;

	return m;
}

/*---------------------------------------------------------*/



// quaternion
/*---------------------------------------------------------*/

/**
 * The SpiderGL.Math.Quat namespace.
 * The provided functions operate on quaternions, represented as standard JavaScript arrays of length 4.
 *
 * @namespace The SpiderGL.Math.Quat namespace defines operations on quaternions.
 */
SpiderGL.Math.Quat = { };

/**
 * Duplicates the input quaternion.
 *
 * @param {array} n The input quaternion.
 *
 * @returns {array} A 4-component array r, where r[i] = q[i] (same as q.slice(0, 4)).
 */
SpiderGL.Math.Quat.dup = function (q) {
	return q.slice(0, 4);
}

SpiderGL.Math.Quat.identity = function (q) {
	return [0, 0, 0, 1];
}

SpiderGL.Math.Quat.inverse = function (q) {
	return [-q[0], -q[1], -q[2], q[3]];
}

SpiderGL.Math.Quat.mul = function (p, q) {
	var px = p[0],
	    py = p[1],
	    pz = p[2],
	    pw = p[3];

	var qx = q[0],
	    qy = q[1],
	    qz = q[2],
	    qw = q[3];

	return [
		px*qw + pw*qx + pz*qy - py*qz,
		py*qw + pw*qy + px*qz - pz*qx,
		pz*qw + pw*qz + py*qx - px*qy,
		pw*qw - px*qx - py*qy - pz*qz
	];
}

SpiderGL.Math.Quat.muls = function (q, s) {
	return [q[0]*s, q[1]*s, q[2]*s, q[3]*s];
}

SpiderGL.Math.Quat.normalize = function (q) {
	var s = 1 / SpiderGL.Math.sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
	return SpiderGL.Math.Quat.muls(q, s);
}

SpiderGL.Math.Quat.from33 = function (m) {
	var m00 = m[0],
	    m10 = m[1],
	    m20 = m[2];

	var m01 = m[3],
	    m11 = m[4],
	    m21 = m[5];

	var m02 = m[6],
	    m12 = m[7],
	    m22 = m[8];

	var t = m00 + m11 + m22;
	var s;

	if (t > 0) {
		t = t + 1;
		s = 0.5 / SpiderGL.Math.sqrt(t);
		return [
			(m21 - m12) * s,
			(m02 - m20) * s,
			(m10 - m01) * s,
			t * s
		];
	} else if ((m00 > m11) && (m00 > m22)) {
		t = m00 - m11 - m22 + 1;
		s = 0.5 / SpiderGL.Math.sqrt(t);
		return [
			t * s,
			(m10 + m01) * s,
			(m02 + m20) * s,
			(m21 - m12) * s,
		];
	}
	else if (m11 > m22) {
		t = -m00 + m11 - m22 + 1;
		s = 0.5 / SpiderGL.Math.sqrt(t);
		return [
			(m10 + m01) * s,
			t * s,
			(m21 + m12) * s,
			(m02 - m20) * s
		];
	}
	else {
		t = -m00 - m11 + m22 + 1;
		s = 0.5 / SpiderGL.Math.sqrt(t);
		return [
			(m02 + m20) * s,
			(m21 + m12) * s,
			t * s,
			(m10 - m01) * s
		];
	}

	return null;
}

SpiderGL.Math.Quat.to33 = function (q) {
	var x  = q[0],
	    y  = q[1],
	    z  = q[2],
	    w  = q[3];

	var xx = x*x,
	    xy = x*y,
	    xz = x*z,
	    xw = x*w;

	var yy = y*y,
	    yz = y*z,
	    yw = y*w;

	var zz = z*z,
	    zw = z*w;

	return [
		1 - 2 * ( yy + zz ),
		    2 * ( xy + zw ),
		    2 * ( xz - yw ),

		    2 * ( xy - zw ),
		1 - 2 * ( xx + zz ),
		    2 * ( yz + xw ),

		    2 * ( xz + yw ),
		    2 * ( yz - xw ),
		1 - 2 * ( xx + yy )
	];
}

SpiderGL.Math.Quat.from44 = function (m) {
	return SpiderGL.Math.Quat.from33(SpiderGL.Math.Mat4.to33(m));
}

SpiderGL.Math.Quat.to44 = function (q) {
	return SpiderGL.Math.Mat3.to44(SpiderGL.Math.Quat.to33(q));
}

SpiderGL.Math.Quat.fromAngleAxis = function (angle, axis) {
	return [0, 0, 0, 1];
}

SpiderGL.Math.Quat.toAngleAxis = function (q) {
	return [0, 0, 0, 1];
}

SpiderGL.Math.Quat.fromEulerAngles = function (x, y, z) {
	return [0, 0, 0, 1];
}

SpiderGL.Math.Quat.toEulerAngles = function (q) {
	return [0, 0, 0, 1];
}

SpiderGL.Math.Quat.copy$ = function (p, q) {
	p[0] = q[0];
	p[1] = q[1];
	p[2] = q[2];
	p[3] = q[3];
	return p;
}

SpiderGL.Math.Quat.identity$ = function (q) {
	q[0] = 0;
	q[1] = 0;
	q[2] = 0;
	q[3] = 1;
	return q;
}

SpiderGL.Math.Quat.invert$ = function (q) {
	q[0] = -q[0];
	q[1] = -q[1];
	q[2] = -q[2];
	return q;
}

SpiderGL.Math.Quat.mul$ = function (q) {
	var px = p[0],
	    py = p[1],
	    pz = p[2],
	    pw = p[3];

	var qx = q[0],
	    qy = q[1],
	    qz = q[2],
	    qw = q[3];

	q[0] = px*qw + pw*qx + pz*qy - py*qz;
	q[1] = py*qw + pw*qy + px*qz - pz*qx;
	q[2] = pz*qw + pw*qz + py*qx - px*qy;
	q[3] = pw*qw - px*qx - py*qy - pz*qz;

	return q;
}

SpiderGL.Math.Quat.muls$ = function (q, s) {
	q[0] *= s;
	q[1] *= s;
	q[2] *= s;
	q[3] *= s;
	return q;
}

SpiderGL.Math.Quat.normalize$ = function (q) {
	var s = 1 / SpiderGL.Math.sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
	return SpiderGL.Math.Quat.muls$(q, s);
}

SpiderGL.Math.project = function (xyzw, modelViewProjectionMatrix, viewport, depthRange) {
	var v3 = SpiderGL.Math.Vec3;
	var m4 = SpiderGL.Math.Mat4;

	var r    = m4.mul4(modelViewProjectionMatrix, xyzw);
	var invW = 1 / r[3];
	r[3]     = invW;

	v3.muls$(r, invW / 2);
	v3.adds$(r, 0.5);

	v3.mul$(r, [viewport[2], viewport[3], depthRange[1] - depthRange[0]]);
	v3.add$(r, [viewport[0], viewport[1], depthRange[0]]);

	return r;
};

SpiderGL.Math.unproject = function (xyz, modelViewProjectionMatrixInverse, viewport, depthRange) {
	var v3 = SpiderGL.Math.Vec3;
	var m4 = SpiderGL.Math.Mat4;

	var r = v3.to4(xyz, 1.0);

	v3.sub$(r, [viewport[0], viewport[1], depthRange[0]]);
	v3.div$(r, [viewport[2], viewport[3], depthRange[1] - depthRange[0]]);

	v3.muls$(r, 2);
	v3.subs$(r, 1);

	r        = m4.mul4(modelViewProjectionMatrixInverse, r);
	var invW = 1 / r[3];
	r[3]     = invW;

	v3.muls$(r, invW);

	return r;
};

