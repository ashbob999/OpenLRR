// Maths.cpp : 
//

#include "Maths.h"


// When defined, D3DRMVector functions implemented here will use the
//  EXACT math needed to produce identical floating point results.
// Otherwise, the functions will use cleaner, easier-to-read implementations
//  that produce nearly-the-same results without excessive float10 casting.
#define D3DRM_USE_IDENTICAL_FLOATMATH


#ifndef DEFINED_FLOAT10
// We don't want to define this typedef outside of source files.
// Ghidra treats all floating function returns as `float10` (since they *are* returned in the ST* registers).
// So keeping it undefined in headers will prevent function signatures from mistakenly using this return type
// instead of the return type `real32`.

// NOTE: `long double` is identical to `double` for MSVC compilers.
//       Yet casting with it we can still can produce identical results to stored register values...(?)
typedef long double float10;
#endif


/**********************************************************************************
 ******** Globals
 **********************************************************************************/

#pragma region Globals

// State of LCG engine used by LRR's C runtime `sint32 std::rand()` and `void std::srand(uint32)`.
// <LegoRR.exe @004b0cc8>
uint32 & Gods98::gLCGState = *(uint32*)0x004b0cc8; // = 1;

// Random number generator replacement for `sint32 std::rand()` and `void std::srand(uint32)`.
Random::WrapperLCGEngine Gods98::gRandom = Random::WrapperLCGEngine(gLCGState);

// Random number generator replacement for `D3DRMVectorRandom`.
// NOTE: Only for when `D3DRM_USE_PRIMARY_RAND` is not being used. Otherwise `gRandom` is used.
Random::LCGEngine gD3DRMRandom = Random::LCGEngine();

#pragma endregion

/**********************************************************************************
 ******** D3DRMVector Functions
 **********************************************************************************/

#pragma region D3DRMVector Functions

// Replacements for D3DRM function calls used in Maths module.

#ifdef D3DRM_USE_PRIMARY_RAND
#define mathsD3DRM_Rand()					Gods98::gRandom.Next()
#else
//static Random::LCGEngine _d3drmRandom = Random::LCGEngine();

#define mathsD3DRM_Rand()					Gods98::gD3DRMRandom.Next() //_d3drmRandom.Next()
#endif


// Unlike Maths_Vector3DNormalize, this performs sanity zero checks. LRR probably enjoys Dividing by zero...
// <D3DRM.DLL @6dda8b04>
Vector3F* __stdcall Gods98::mathsD3DRMVectorNormalize(IN OUT Vector3F* v)
{
	if ((v->x != 0.0f) || (v->y != 0.0f) || (v->z != 0.0f)) {

#ifdef D3DRM_USE_IDENTICAL_FLOATMATH
		const real32 m = std::sqrt((((float10)v->x * v->x) + ((float10)v->y * v->y) + ((float10)v->z * v->z))); // (stored as float)
		if (m != 0.0f) {
			const float10 n = 1.0L / m;
			v->x *= n;
			v->y *= n;
			v->z *= n;
			return v;
		}
#else
		const real32 m = std::sqrt(((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));
		if (m != 0.0f) {
			const real32 n = 1.0f / m;
			v->x *= n;
			v->y *= n;
			v->z *= n;
			return v;
		}
#endif
	}
	v->x = 1.0f;
	v->y = 0.0f;
	v->z = 0.0f;
	return v;
}

// <D3DRM.DLL @6dda8d54>
Vector3F* __stdcall Gods98::mathsD3DRMVectorRandom(OUT Vector3F* d)
{
#ifdef D3DRM_USE_IDENTICAL_FLOATMATH
	// (1.0 / 0x7fff) float hex: 0x38000100
	static_assert((1.0f / MATHS_RAND_MAX) == 3.051851e-05f, "Divisor is not identical for exact D3DRM math");

	// D3DRM uses MSVCRT.DLL::rand, which has an identical implementation to what's used in LRR's runtime.
	// So we're using a second copy of the same function.
	//  (though really, this could be joined, so that setting the level seed has more meaning than just seeding randomness fields).
	do {
		d->x = ((float10)mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;
		d->y = ((float10)mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;
		d->z = ((float10)mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;

	//} while (D3DRMVectorModulus(d) == 0.0f);
	} while (std::sqrt(((float10)d->x*d->x)+((float10)d->y*d->y)+((float10)d->z*d->z)) == 0.0f);
#else
	do {
		d->x = (mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;
		d->y = (mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;
		d->z = (mathsD3DRM_Rand() * (1.0f / MATHS_RAND_MAX)) - 0.5f;

	} while (std::sqrt((d->x*d->x)+(d->y*d->y)+(d->z*d->z)) == 0.0f);
#endif

	mathsD3DRMVectorNormalize(d);
	return d;
}

// <D3DRM.DLL @6dda8c47>
Vector3F* __stdcall Gods98::mathsD3DRMVectorRotate(OUT Vector3F* r, const Vector3F* pV, IN OUT Vector3F* pAxis, real32 theta)
{
	// Original function has a null check for the return value of the first `D3DRMVectorNormalize`.
	// But it should not be possible to to reach this, because `D3DRMVectorNormalize` has no null-checks.
	// Making these pointer checks... *pointless*
	/*Vector3F* result =*/ mathsD3DRMVectorNormalize(pAxis);

	const Vector3F v = *pV;
	const Vector3F axis = *pAxis;

#ifdef D3DRM_USE_IDENTICAL_FLOATMATH
	const real32 sinmt = (real32)(float10)std::sin((float10)(real32)-(float10)theta); // asm FSIN (stored as float), (theta passed as float10)
	const float10 cosmt = (float10)std::cos(-(float10)theta); // asm FCOS (stored as float10), (theta passed as float10)
	const float10 cos1_mt = 1.0L - cosmt; // (stored as float10)

	// Minimal casts to reproduce the output of `D3DRMVectorRotate` floating point math, down to the bit.
	r->y = ((real32)(axis.y * cos1_mt * axis.x) - (float10)(axis.z * sinmt)) * v.x +
		   ((real32)(axis.y * cos1_mt * axis.z) + (float10)(axis.x * sinmt)) * v.z +
		   ((real32)(axis.y * cos1_mt) *          (float10) axis.y + cosmt)  * v.y;

	r->x = ((real32)(axis.x * cos1_mt * axis.z) - (float10)(axis.y * sinmt)) * v.z +
		   ((real32)(axis.x * cos1_mt * axis.y) + (float10)(axis.z * sinmt)) * v.y +
		   ((real32)(axis.x * cos1_mt) *          (float10) axis.x + cosmt)  * v.x;

	r->z = ((real32)(axis.z * cos1_mt * axis.x) + (float10)(axis.y * sinmt)) * v.x +
		   ((real32)(axis.z * cos1_mt * axis.y) - (float10)(axis.x * sinmt)) * v.y +
		   ((float10)axis.z * cos1_mt *                     axis.z + cosmt)  * v.z;
#else
	const real32 cosmt = std::cos(-theta);
	const real32 sinmt = std::sin(-theta);
	const real32 cos1_mt = 1.0f - cosmt;

	r->y = ((axis.y * cos1_mt * axis.x) - (axis.z * sinmt)) * v.x +
		   ((axis.y * cos1_mt * axis.z) + (axis.x * sinmt)) * v.z +
		   ((axis.y * cos1_mt * axis.y) +          (cosmt)) * v.y;

	r->x = ((axis.x * cos1_mt * axis.z) - (axis.y * sinmt)) * v.z +
		   ((axis.x * cos1_mt * axis.y) + (axis.z * sinmt)) * v.y +
		   ((axis.x * cos1_mt * axis.x) +          (cosmt)) * v.x;

	r->z = ((axis.z * cos1_mt * axis.y) - (axis.x * sinmt)) * v.y +
		   ((axis.z * cos1_mt * axis.x) + (axis.y * sinmt)) * v.x +
		   ((axis.z * cos1_mt * axis.z) +          (cosmt)) * v.z;
#endif

	mathsD3DRMVectorNormalize(r);
	return r;
}

// <D3DRM.DLL @6dda82ab>
Vector3F* __stdcall Gods98::mathsD3DRMVectorReflect(OUT Vector3F* d, const Vector3F* pRay, const Vector3F* pNorm)
{
	const Vector3F ray = *pRay;
	const Vector3F norm = *pNorm;

#ifdef D3DRM_USE_IDENTICAL_FLOATMATH
	const float10 dotProduct = ((float10)norm.x * ray.x) + ((float10)norm.y * ray.y) + ((float10)norm.z * ray.z); // (stored as float)
	const float10 dotProduct2 = dotProduct + dotProduct; // (stored as float)
#else
	const real32 dotProduct = (norm.x * ray.x) + (norm.y * ray.y) + (norm.z * ray.z);
	const real32 dotProduct2 = dotProduct + dotProduct;
#endif

	d->x = (dotProduct2 * norm.x) - ray.x;
	d->y = (dotProduct2 * norm.y) - ray.y;
	d->z = (dotProduct2 * norm.z) - ray.z;
	return d;
}

#pragma endregion

/**********************************************************************************
 ******** Vector and Misc Functions
 **********************************************************************************/

#pragma region Functions

/*
// <inlined>
__inline real32 Maths_InterpolationDelta(real32 start, real32 end, real32 current) { return (current - start) / (end - start); }

// <inlined>
__inline real32 Maths_Interpolate(real32 start, real32 end, real32 delta) { return start + ((end - start) * delta); }

// <inlined>
__inline Vector3F* Maths_Vector2DMake3D(OUT Vector3F* r, const Point2F* v) { r->x = v->x; r->y = v->y; r->z = 0.0f; return r; }

// <inlined>
__inline Point2F* Maths_Vector3DMake2D(OUT Point2F* r, const Vector3F* v) { r->x = v->x; r->y = v->y; return r; }

// <inlined>
__inline real32 Maths_Vector2DModulus(const Point2F* v) { return (real32)std::sqrt((v->x*v->x)+(v->y*v->y)); }
*/

// <LegoRR.exe @00401240>
real32 __cdecl Gods98::noinline(Maths_Vector2DDistance)(const Point2F* a, const Point2F* b)
{
	return (real32)std::sqrt(((a->x-b->x)*(a->x-b->x))+((a->y-b->y)*(a->y-b->y)));
}
//__inline real32 Maths_Vector2DDistance(const Point2F* a, const Point2F* b) { return (real32)std::sqrt(((a->x-b->x)*(a->x-b->x))+((a->y-b->y)*(a->y-b->y))); }

/*
// <inlined>
__inline real32 Maths_Vector2DDotProduct(const Point2F* a, const Point2F* b) { return ((a->x*b->x) + (a->y*b->y)); }

// <inlined>
__inline Point2F* Maths_Vector2DSubtract(OUT Point2F* r, const Point2F* a, const Point2F* b) { r->x = a->x-b->x; r->y = a->y-b->y; return r; }

// <inlined>
__inline Point2F* Maths_Vector2DAdd(OUT Point2F* r, const Point2F* a, const Point2F* b) { r->x = a->x+b->x; r->y = a->y+b->y; return r; }

// <inlined>
__inline Point2F* Maths_Vector2DScale(OUT Point2F* r, const Point2F* a, real32 f) { r->x = a->x*f; r->y = a->y*f; return r; }

// <inlined>
__inline Point2F* Maths_Vector2DNormalize(OUT Point2F* r) { Maths_Vector2DScale(r, r, 1.0f/Maths_Vector2DLength(r)); return r; }

// <inlined>
__inline Point2F* Maths_Vector2DSetLength(OUT Point2F* r, const Point2F* a, real32 l) { real32 m = Maths_Vector2DModulus(a); return Maths_Vector2DScale(r, a, (1.0f/m) * l); }

// <inlined>
__inline Point2F* Maths_Vector2DRandom(OUT Point2F* r) { r->x = Maths_RandRange(0.0f, 1.0f); r->y = Maths_RandRange(0.0f, 1.0f); return Maths_Vector2DNormalise(r); }

// <inlined>
__inline real32 Maths_Vector2DCrossProductZ(const Point2F* a, const Point2F* b) { return (a->x*b->y) - (a->y*b->x); }

// <inlined>
__inline real32 Maths_Vector3DModulus(const Vector3F* v) { return (real32)std::sqrt((v->x*v->x)+(v->y*v->y)+(v->z*v->z)); }

// <inlined>
__inline real32 Maths_Vector3DDistance(const Vector3F* a, const Vector3F* b) { return (real32)std::sqrt(((a->x-b->x)*(a->x-b->x))+((a->y-b->y)*(a->y-b->y))+((a->z-b->z)*(a->z-b->z))); }
*/

// <LegoRR.exe @004013e0>
Vector3F* __cdecl Gods98::noinline(Maths_Vector3DCrossProduct)(OUT Vector3F* r, const Vector3F* a, const Vector3F* b)
{
	log_firstcall();

	r->x = (a->y*b->z)-(a->z*b->y); r->y = (a->z*b->x)-(a->x*b->z); r->z = (a->x*b->y)-(a->y*b->x); return r;
}
//__inline Vector3F* Maths_Vector3DCrossProduct(OUT Vector3F* r, const Vector3F* a, const Vector3F* b) { r->x = (a->y*b->z)-(a->z*b->y); r->y = (a->z*b->x)-(a->x*b->z); r->z = (a->x*b->y)-(a->y*b->x); return r; }

// <LegoRR.exe @00401470>
Vector3F* __cdecl Gods98::noinline(Maths_Vector3DAdd)(OUT Vector3F* r, const Vector3F* a, const Vector3F* b)
{
	log_firstcall();

	r->x = (a->x+b->x); r->y = (a->y+b->y); r->z = (a->z+b->z); return r;
}
//__inline Vector3F* Maths_Vector3DAdd(OUT Vector3F* r, const Vector3F* a, const Vector3F* b) { r->x = (a->x+b->x); r->y = (a->y+b->y); r->z = (a->z+b->z); return r; }

// <LegoRR.exe @00401630>
Vector3F* __cdecl Gods98::noinline(Maths_Vector3DSubtract)(OUT Vector3F* r, const Vector3F* a, const Vector3F* b)
{
	log_firstcall();

	r->x = (a->x-b->x); r->y = (a->y-b->y); r->z = (a->z-b->z); return r;
}
//__inline Vector3F* Maths_Vector3DSubtract(OUT Vector3F* r, const Vector3F* a, const Vector3F* b) { r->x = (a->x-b->x); r->y = (a->y-b->y); r->z = (a->z-b->z); return r; }


// <inlined>
//__inline real32 Maths_Vector3DDotProduct(const Vector3F* a, const Vector3F* b) { return (a->x*b->x) + (a->y*b->y) + (a->z*b->z); }


// <LegoRR.exe @00401660>
Vector3F* __cdecl Gods98::noinline(Maths_Vector3DScale)(OUT Vector3F* r, const Vector3F* a, real32 f)
{
	log_firstcall();

	r->x = a->x*f; r->y = a->y*f; r->z = a->z*f; return r;
}
//__inline Vector3F* Maths_Vector3DScale(OUT Vector3F* r, const Vector3F* a, real32 f) { r->x = a->x*f; r->y = a->y*f; r->z = a->z*f; return r; }

// <LegoRR.exe @00401690>
Vector3F* __cdecl Gods98::noinline(Maths_Vector3DNormalize)(IN OUT Vector3F* r)
{
	log_firstcall();

	return Maths_Vector3DScale(r, r, 1.0f/Maths_Vector3DLength(r)); return r;
}
//__inline Vector3F* Maths_Vector3DNormalize(OUT Vector3F* r) { Maths_Vector3DScale(r, r, 1.0f/Maths_Vector3DLength(r)); return r; }


// <inlined>
//__inline Vector3F* Maths_Vector3DInterpolate(OUT Vector3F* result, const Vector3F* start, const Vector3F* end, real32 delta) { Maths_Vector3DSubtract(result, end, start); Maths_Vector3DScale(result, result, delta); Maths_Vector3DAdd(result, start, result); return result; }


//// END OF INLINES ////

// <unused>
Area2F* __cdecl Gods98::Maths_Area2DInterpolate(OUT Area2F* result, const Area2F* start, const Area2F* end, real32 delta)
{
	result->x = end->x - start->x;
	result->y = end->y - start->y;
	result->width = end->width - start->width;
	result->height = end->height - start->height;

	result->x *= delta;
	result->y *= delta;
	result->width *= delta;
	result->height *= delta;
	
	result->x = start->x + result->x;
	result->y = start->y + result->y;
	result->width = start->width + result->width;
	result->height = start->height + result->height;
	
	return result;
}


// <LegoRR.exe @004797c0>
Vector3F* __cdecl Gods98::Maths_Vector3DRandom(OUT Vector3F* r)
{
	log_firstcall();

	return mathsD3DRMVectorRandom(r);

	//return reinterpret_cast<Vector3F*>(::D3DRMVectorRandom(reinterpret_cast<D3DVECTOR*>(r)));
}


// <unused>
Vector3F* __cdecl Gods98::Maths_Vector3DReflect(OUT Vector3F* d, const Vector3F* ray, const Vector3F* norm)
{
	log_firstcall();

	return mathsD3DRMVectorReflect(d, ray, norm);

	//return reinterpret_cast<Vector3F*>(::D3DRMVectorReflect(reinterpret_cast<D3DVECTOR*>(d),
	//														reinterpret_cast<D3DVECTOR*>(const_cast<Vector3F*>(ray)),
	//														reinterpret_cast<D3DVECTOR*>(const_cast<Vector3F*>(norm))));
}


// SIDE EFFECTS: This function normalizes the input axis vector.
// <LegoRR.exe @004797d0>
Vector3F* __cdecl Gods98::Maths_Vector3DRotate(OUT Vector3F* r, const Vector3F* v, IN OUT Vector3F* axis, real32 theta)
{
	log_firstcall();

	/// TODO: Consider isolating axis so that's not normalized as a side effect of `D3DRMVectorRandom`.
	///       First needs research into whether any existing functions rely on these side effects.
	return mathsD3DRMVectorRotate(r, v, axis, theta);

	//return reinterpret_cast<Vector3F*>(::D3DRMVectorRotate(reinterpret_cast<D3DVECTOR*>(r),
	//													   reinterpret_cast<D3DVECTOR*>(const_cast<Vector3F*>(v)),
	//													   reinterpret_cast<D3DVECTOR*>(axis), //const_cast<Vector3F*>(axis)),
	//													   theta));
}

// <LegoRR.exe @004797f0>
Vector3F* __cdecl Gods98::Maths_PlaneNormal(OUT Vector3F* n, const Vector3F* p1, const Vector3F* p2, const Vector3F* p3)
{
	log_firstcall();

	Vector3F v1, v2;

	Maths_Vector3DSubtract(&v1, p2, p1);
	Maths_Vector3DSubtract(&v2, p3, p2);
	Maths_Vector3DCrossProduct(n, &v1, &v2);
	return Maths_Vector3DNormalize(n);
}

// <LegoRR.exe @004798f0>
real32 __cdecl Gods98::Maths_TriangleAreaZ(const Vector3F* p1, const Vector3F* p2, const Vector3F* p3, bool32 bfc)
{
	log_firstcall();

	real32 area = 0.0f, maxLen = 0.0f, len[3], angle, height;
	Vector3F edgeVec[3], p[3];
	Vector3F* edgeDef[3][2] = { { &p[0], &p[1] }, { &p[1], &p[2] }, { &p[2], &p[0] } };
	Vector3F* point; // assigned but unused???

	// Ignore the z value, but don't clear it in the original...

	p[0] = *p1; p[0].z = 0.0f;
	p[1] = *p2; p[1].z = 0.0f;
	p[2] = *p3; p[2].z = 0.0f;

	Vector3F norm;
	if (bfc) Maths_PlaneNormal(&norm, &p[0], &p[1], &p[2]);
	else norm.z = -1.0f;

	// Backface cull if required...
	if (norm.z < 0.0f) {
		uint32 base = 0; // dummy init (I don't know if initialization is guaranteed with this loop)

		for (uint32 loop = 0; loop < 3; loop++) {

			// Store a set of lengths and vectors...
			Maths_Vector3DSubtract(&edgeVec[loop], edgeDef[loop][1], edgeDef[loop][0]);
			len[loop] = Maths_Vector3DModulus(&edgeVec[loop]);

			// Normalise the vector (for the angle calculation)...
			Maths_Vector3DScale(&edgeVec[loop], &edgeVec[loop], 1.0f/len[loop]);

			// Find which is the longest edge to use as the base...
			if (len[loop] > maxLen) {
				maxLen = len[loop];
				base = loop;
			}
		}

		// Use the end point of the base line as the test point
		// then reverse the vector of the base line to point away from this point...
		point = edgeDef[base][1]; // unused????
		Maths_Vector3DScale(&edgeVec[base], &edgeVec[base], -1.0f);

		// Calculate the angle between the base and the 'next' side...
		// Obtain the height from the angle and the length of the adjacent edge.
		uint32 next = (base + 1) % 3;
		angle = Maths_ACos(Maths_Vector3DDotProduct(&edgeVec[base], &edgeVec[next]));
		if (::_finite(angle)) {

			height = Maths_Sin(angle) * len[next];

			// 1/2base * height...
			area = (len[base] / 2.0f) * height;
		}
	}

	return area;
}


/// CUSTOM: Legacy `void std::srand(uint32 seed);`
void __cdecl Gods98::Maths_SeedRand(uint32 seed)
{
	log_firstcall();

	gRandom.Seed(seed);
	//legacy::srand(seed);
}

// We have this version as an alternative to `Maths_Rand`, since a 32-bit integer is expected to be
// returned. Which is important for the logic of functions using `std::rand` vs. `Maths_Rand`.
/// CUSTOM: Legacy `sint32 std::rand();`
sint32 __cdecl Gods98::Maths_RandInt32(void)
{
	log_firstcall();

	return gRandom.Next();
	//return legacy::rand();
}


// Uses legacy `sint32 std::rand();`
// <LegoRR.exe @00479b60>
sint16 __cdecl Gods98::Maths_Rand(void)
{
	log_firstcall();

	return (sint16)Maths_RandInt32();
	//return (sint16)legacy::rand();
}

// <LegoRR.exe @00479b70>
real32 __cdecl Gods98::Maths_RandRange(real32 low, real32 high)
{
	log_firstcall();

	real32 val = (real32)Maths_Rand();
	val /= MATHS_RAND_MAX; // 0x7fff

	val *= (high-low);
	return low + val;
}

// <LegoRR.exe @00479ba0>
bool32 __cdecl Gods98::Maths_RayPlaneIntersection(Vector3F* endPoint, const Vector3F* rayOrigin, const Vector3F* ray, const Vector3F* planePoint, const Vector3F* planeNormal)
{
	log_firstcall();

	Vector3F r = *ray, n = *planeNormal;

	Maths_Vector3DNormalise(&r);
	Maths_Vector3DNormalise(&n);

	real32 dist;
	if (Maths_RayPlaneDistance(&dist, rayOrigin, &r, planePoint, &n)) {
		Maths_RayEndPoint(endPoint, rayOrigin, &r, dist);
		return true;
	}
	return false;
}

// <LegoRR.exe @00479cf0>
bool32 __cdecl Gods98::Maths_RayPlaneDistance(OUT real32* dist, const Vector3F* rayOrigin, const Vector3F* ray, const Vector3F* planePoint, const Vector3F* planeNormal)
{
	real32 t;
	if ((t = Maths_Vector3DDotProduct(planeNormal, ray)) != 0.0f) {

		// Translate the point in the plane so that the ray would originate from (0,0,0).
		Vector3F point;
		Maths_Vector3DSubtract(&point, planePoint, rayOrigin);

		*dist = Maths_Vector3DDotProduct(planeNormal, &point) / t;

		return true;
	}
	return false;
}

// <LegoRR.exe @00479d70>
void __cdecl Gods98::Maths_RayEndPoint(OUT Vector3F* endPoint, const Vector3F* rayOrigin, const Vector3F* ray, real32 dist)
{
	Vector3F temp;

	Maths_Vector3DScale(&temp, ray, dist);
	Maths_Vector3DAdd(endPoint, rayOrigin, &temp);
}


// <unused>
Point2F* __cdecl Gods98::Maths_Vector2DLineIntersection(OUT Point2F* intersection, const Point2F* start1, const Point2F* end1, const Point2F* start2, const Point2F* end2)
{
	Point2F vector1, vector2;
	//real32 length1, length2;

	Maths_Vector2DSubtract(&vector1, end1, start1);
	real32 length1 = Maths_Vector2DLength(&vector1);
	Maths_Vector2DScale(&vector1, &vector1, 1.0f / length1);
	Maths_Vector2DSubtract(&vector2, end2, start2);
	real32 length2 = Maths_Vector2DLength(&vector2);
	Maths_Vector2DScale(&vector2, &vector2, 1.0f / length2);

	if (Maths_Vector2DIntersection(intersection, start1, &vector1, start2, &vector2)) {

		Point2F siVec;
		Maths_Vector2DSubtract(&siVec, intersection, start1);
		if (Maths_Vector2DDotProduct(&siVec, &vector1) >= 0.0f) {
			Maths_Vector2DSubtract(&siVec, intersection, start2);
			if (Maths_Vector2DDotProduct(&siVec, &vector2) >= 0.0f) {
				if (length1 >= Maths_Vector2DDistance(intersection, start1) &&
					length2 >= Maths_Vector2DDistance(intersection, start2)) {

					return intersection;
				}
			}
		}
	}

	return nullptr;
}


// <LegoRR.exe @00479db0>
Point2F* __cdecl Gods98::Maths_Vector2DIntersection(OUT Point2F* intersection, const Point2F* start1, const Point2F* vector1, const Point2F* start2, const Point2F* vector2)
{
	log_firstcall();

	real32 a =  vector2->x;
	real32 b = -vector1->x;
	real32 c =  vector2->y;
	real32 d = -vector1->y;

	real32 determinant = ( (a * d) - (b * c) );

	if (determinant != 0.0f) {

		real32 t, x = start1->x - start2->x, y = start1->y - start2->y;

		real32 invc = -c / determinant;
		real32 invd =  a / determinant;

		t = invc * x + invd * y;

		Maths_Vector2DScale(intersection, vector1, t);
		Maths_Vector2DAdd(intersection, start1, intersection);

		return intersection;
	}

	return nullptr;
}

// <LegoRR.exe @00479e40>
bool32 __cdecl Gods98::Maths_PointInsidePoly(const Point2F* point, const Point2F* fromList, const Point2F* toList, uint32 count)
{
	log_firstcall();

	// Count the edges to the right of the point and return TRUE if it is odd...

	uint32 rightCount = 0;

	for (uint32 loop = 0; loop < count; loop++) {
		if ((fromList[loop].y <= point->y && toList[loop].y >= point->y) ||
			(fromList[loop].y >= point->y && toList[loop].y <= point->y)) {

			real32 alt = toList[loop].y - fromList[loop].y;
			real32 slope = toList[loop].x - fromList[loop].x;
			real32 line = point->y - fromList[loop].y;

			real32 inter = fromList[loop].x + ((line / alt) * slope);

			if (inter > point->x) rightCount++;
		}
	}

	return (rightCount & 0x00000001); // is odd
}


// <unused>
Vector3F* __cdecl Gods98::Maths_Vector3DApplyMatrixXYZ(OUT Vector3F* result, const Matrix4F* matrix, real32 x, real32 y, real32 z)
{
//	real32 tx, ty, tz, tw;

//	real32 tx = (matrix->m[0][0] * x) + (matrix->m[1][0] * y) + (matrix->m[2][0] * z) + matrix->m[3][0];
//	real32 ty = (matrix->m[0][1] * x) + (matrix->m[1][1] * y) + (matrix->m[2][1] * z) + matrix->m[3][1];
//	real32 tz = (matrix->m[0][2] * x) + (matrix->m[1][2] * y) + (matrix->m[2][2] * z) + matrix->m[3][2];
//	real32 tw = (matrix->m[0][3] * x) + (matrix->m[1][3] * y) + (matrix->m[2][3] * z) + matrix->m[3][3];

//	if (std::fabs(w) < 1.0e-5f) // Tolerance for FLOATs
//		Error_Warn(true, "Invalid arguments.");

//	result->x = tx / tw;
//	result->y = ty / tw;
//	result->z = tz / tw;

	result->x = (matrix->m[0][0] * x) + (matrix->m[1][0] * y) + (matrix->m[2][0] * z) + matrix->m[3][0];
	result->y = (matrix->m[0][1] * x) + (matrix->m[1][1] * y) + (matrix->m[2][1] * z) + matrix->m[3][1];
	result->z = (matrix->m[0][2] * x) + (matrix->m[1][2] * y) + (matrix->m[2][2] * z) + matrix->m[3][2];

	return result;
}

/*
// not inlined (or even implemented) by Gods98
// <unused>
__inline Vector3F* Maths_Vector3DApplyMatrix(OUT Vector3F* result, const Matrix4F* matrix, const Vector3F* point)
{
	return Maths_Vector3DApplyMatrixXYZ(result, matrix, point->x, point->y, point->z);
}
*/


// <LegoRR.exe @00479ed0>
bool32 __cdecl Gods98::Maths_RaySphereIntersection(const Vector3F* center, real32 radius, const Vector3F* camera, const Vector3F* line)
{
	log_firstcall();

//	P = Center of sphere
//	L = line = L(t) = Q + tV
//	Q = Any point on line (camera pos)
//	V = normalise direction of line.
//	R = Radius of sphere
//
//	Intersection occurs if : 
//
//	( (2V .(Q - P)) ^ 2 ) - ( 4 * (V.V) * ((Q - P) . (Q - P) - R^2 ) ) > 0

	Vector3F csVec, ray = *line;
//	Maths_Vector3DNormalise(&ray);
	Maths_Vector3DScale(&ray, &ray, 2.0f);
	Maths_Vector3DSubtract(&csVec, camera, center);
//	( (2V . csVec) ^ 2 ) - ( 4 * (V.V) * (csVec . csVec - R^2 ) ) > 0
	
	real32 t = Maths_Vector3DDotProduct(&ray, &csVec);
	t *= t;
	t -= (4 * Maths_Vector3DDotProduct(&csVec, &csVec) - (radius * radius));

	return (t > 0);
}



#pragma endregion


/**********************************************************************************
 ******** Matrix4 Functions
 **********************************************************************************/

/////////////////////////////////////////////////////////////////////////
// Andy Ray.  3/2/99.
// Some simple matrix functions.
/////////////////////////////////////////////////////////////////////////

#pragma region Functions

// Compared to other Matrix4 multiplication functions, a = right, and b = left
// <LegoRR.exe @00479fa0>
void __cdecl Gods98::Matrix_Mult(OUT Matrix4F* ans, const Matrix4F* a, const Matrix4F* b)
{
	log_firstcall();

	Matrix_Zero(ans);
	for (uint32 i = 0; i < 4; i++) {
		for (uint32 j = 0; j < 4; j++) {
			for (uint32 k = 0; k < 4; k++) {
				ans->m[i][j] += a->m[k][j] * b->m[i][k];
			}
		}
	}
}

// Generates the matrix for a rotation of rot radians about the X-Axis
// <LegoRR.exe @0047a010>
void __cdecl Gods98::Matrix_RotX(OUT Matrix4F* ans, real32 rot)
{
	log_firstcall();

	real32 cosine = Maths_Cos(rot);
	real32 sine = Maths_Sin(rot);
	Matrix_Identity(ans);

	ans->m[1][1] = cosine;
	ans->m[2][2] = cosine;
	ans->m[1][2] = -sine;
	ans->m[2][1] =  sine;
}

// Generates the matrix for a rotation of rot radians about the Y-Axis
// <LegoRR.exe @0047a060>
void __cdecl Gods98::Matrix_RotY(OUT Matrix4F* ans, real32 rot)
{
	log_firstcall();

	real32 cosine = Maths_Cos(rot);
	real32 sine = Maths_Sin(rot);
	Matrix_Identity(ans);

	ans->m[0][0] = cosine;
	ans->m[2][2] = cosine;
	ans->m[0][2] =  sine;
	ans->m[2][0] = -sine;
}

// Generates the matrix for a rotation of rot radians about the Z-Axis
// <LegoRR.exe @0047a0b0>
void __cdecl Gods98::Matrix_RotZ(OUT Matrix4F* ans, real32 rot)
{
	log_firstcall();

	real32 cosine = Maths_Cos(rot);
	real32 sine = Maths_Sin(rot);
	Matrix_Identity(ans);

	ans->m[0][0] = cosine;
	ans->m[1][1] = cosine;
	ans->m[0][1] = -sine;
	ans->m[1][0] =  sine;
}

// Generates a translation matrix.
// <LegoRR.exe @0047a100>
void __cdecl Gods98::Matrix_Translate(OUT Matrix4F* ans, const Vector3F* trans)
{
	log_firstcall();

	Matrix_Identity(ans);

	ans->m[3][0] = trans->x;
	ans->m[3][1] = trans->y;
	ans->m[3][2] = trans->z;
}

// Makes the identity matrix
// <LegoRR.exe @0047a130>
void __cdecl Gods98::Matrix_Identity(OUT Matrix4F* ans)
{
	log_firstcall();

	for (uint32 i = 0; i < 4; i++) {
		for (uint32 j = 0; j < 4; j++) {
			ans->m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}
}

// Makes the zero matrix.
// <LegoRR.exe @0047a160>
void __cdecl Gods98::Matrix_Zero(OUT Matrix4F* ans)
{
	log_firstcall();

	//ALT: std::memset(ans, 0, sizeof(Matrix4F));

	for (uint32 i = 0; i < 4; i++) {
		for (uint32 j = 0; j < 4; j++) {
			ans->m[i][j] = 0.0f;
		}
	}
}

// Copies a matrix.
// <LegoRR.exe @0047a170>
void __cdecl Gods98::Matrix_Copy(OUT Matrix4F* to, const Matrix4F* from)
{
	log_firstcall();

	//ALT: std::memcpy(to, from, sizeof(Matrix4F));

	for (uint32 i = 0; i < 4; i++) {
		for (uint32 j = 0; j < 4; j++) {
			to->m[i][j] = from->m[i][j];
		}
	}
}


// <unused>
bool32 __cdecl Gods98::Matrix_Invert(OUT Matrix4F* q, const Matrix4F* a)
{
	if (std::fabs(a->m[3][3] - 1.0f) > 0.001f)
		return false;
	if (std::fabs(a->m[0][3]) > 0.001f || std::fabs(a->m[1][3]) > 0.001f || std::fabs(a->m[2][3]) > 0.001f)
		return false;
	
	real32 fDetInv = 1.0f / ( a->m[0][0] * ( a->m[1][1] * a->m[2][2] - a->m[1][2] * a->m[2][1] ) -
							  a->m[0][1] * ( a->m[1][0] * a->m[2][2] - a->m[1][2] * a->m[2][0] ) +
							  a->m[0][2] * ( a->m[1][0] * a->m[2][1] - a->m[1][1] * a->m[2][0] ) );
	
	q->m[0][0] =  fDetInv * ( a->m[1][1] * a->m[2][2] - a->m[1][2] * a->m[2][1] );
	q->m[0][1] = -fDetInv * ( a->m[0][1] * a->m[2][2] - a->m[0][2] * a->m[2][1] );
	q->m[0][2] =  fDetInv * ( a->m[0][1] * a->m[1][2] - a->m[0][2] * a->m[1][1] );
	q->m[0][3] = 0.0f;
	
	q->m[1][0] = -fDetInv * ( a->m[1][0] * a->m[2][2] - a->m[1][2] * a->m[2][0] );
	q->m[1][1] =  fDetInv * ( a->m[0][0] * a->m[2][2] - a->m[0][2] * a->m[2][0] );
	q->m[1][2] = -fDetInv * ( a->m[0][0] * a->m[1][2] - a->m[0][2] * a->m[1][0] );
	q->m[1][3] = 0.0f;
	
	q->m[2][0] =  fDetInv * ( a->m[1][0] * a->m[2][1] - a->m[1][1] * a->m[2][0] );
	q->m[2][1] = -fDetInv * ( a->m[0][0] * a->m[2][1] - a->m[0][1] * a->m[2][0] );
	q->m[2][2] =  fDetInv * ( a->m[0][0] * a->m[1][1] - a->m[0][1] * a->m[1][0] );
	q->m[2][3] = 0.0f;
	
	q->m[3][0] = -( a->m[3][0] * q->m[0][0] + a->m[3][1] * q->m[1][0] + a->m[3][2] * q->m[2][0] );
	q->m[3][1] = -( a->m[3][0] * q->m[0][1] + a->m[3][1] * q->m[1][1] + a->m[3][2] * q->m[2][1] );
	q->m[3][2] = -( a->m[3][0] * q->m[0][2] + a->m[3][1] * q->m[1][2] + a->m[3][2] * q->m[2][2] );
	q->m[3][3] = 1.0f;
	
	return true;
}


/// CUSTOM:
bool Gods98::Matrix_Equals(const Matrix4F* a, const Matrix4F* b)
{
	for (uint32 i = 0; i < 4; i++) {
		for (uint32 j = 0; j < 4; j++) {
			if (a->m[i][j] != b->m[i][j])
				return false;
		}
	}
	return true;
}

#pragma endregion
