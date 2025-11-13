#ifndef RAYMATH_EXT_H
#define RAYMATH_EXT_H

#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_MATRIX_TYPE
#define RL_QUATERNION_TYPE

typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Vector4 Vector4;
typedef struct Matrix  Matrix;
typedef struct float3  float3;
typedef struct float16 float16;
typedef Vector4 Quaternion;

float Lerp(float start, float end, float amount);
float Normalize(float value, float start, float end);
float Remap(float value, float inputStart, float inputEnd, float outputStart, float outputEnd);
float Wrap(float value, float min, float max);
int FloatEquals(float x, float y);
Vector2 Vector2Zero(void);
Vector2 Vector2One(void);
Vector2 Vector2Add(Vector2 v1, Vector2 v2);
Vector2 Vector2AddValue(Vector2 v, float add);
Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);
Vector2 Vector2SubtractValue(Vector2 v, float sub);
float Vector2Length(Vector2 v);
float Vector2LengthSqr(Vector2 v);
float Vector2DotProduct(Vector2 v1, Vector2 v2);
float Vector2Distance(Vector2 v1, Vector2 v2);
float Vector2DistanceSqr(Vector2 v1, Vector2 v2);
float Vector2Angle(Vector2 v1, Vector2 v2);
float Vector2LineAngle(Vector2 start, Vector2 end);
Vector2 Vector2Scale(Vector2 v, float scale);
Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);
Vector2 Vector2Negate(Vector2 v);
Vector2 Vector2Divide(Vector2 v1, Vector2 v2);
Vector2 Vector2Normalize(Vector2 v);
Vector2 Vector2Transform(Vector2 v, Matrix mat);
Vector2 Vector2Lerp(Vector2 v1, Vector2 v2, float amount);
Vector2 Vector2Reflect(Vector2 v, Vector2 normal);
Vector2 Vector2Rotate(Vector2 v, float angle);
Vector2 Vector2MoveTowards(Vector2 v, Vector2 target, float maxDistance);
Vector2 Vector2Invert(Vector2 v);
Vector2 Vector2Clamp(Vector2 v, Vector2 min, Vector2 max);
Vector2 Vector2ClampValue(Vector2 v, float min, float max);
int Vector2Equals(Vector2 p, Vector2 q);
Vector3 Vector3Zero(void);
Vector3 Vector3One(void);
Vector3 Vector3Add(Vector3 v1, Vector3 v2);
Vector3 Vector3AddValue(Vector3 v, float add);
Vector3 Vector3Subtract(Vector3 v1, Vector3 v2);
Vector3 Vector3SubtractValue(Vector3 v, float sub);
Vector3 Vector3Scale(Vector3 v, float scalar);
Vector3 Vector3Multiply(Vector3 v1, Vector3 v2);
Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2);
Vector3 Vector3Perpendicular(Vector3 v);
float Vector3Length(const Vector3 v);
float Vector3LengthSqr(const Vector3 v);
float Vector3DotProduct(Vector3 v1, Vector3 v2);
float Vector3Distance(Vector3 v1, Vector3 v2);
float Vector3DistanceSqr(Vector3 v1, Vector3 v2);
float Vector3Angle(Vector3 v1, Vector3 v2);
Vector3 Vector3Negate(Vector3 v);
Vector3 Vector3Divide(Vector3 v1, Vector3 v2);
Vector3 Vector3Normalize(Vector3 v);
void Vector3OrthoNormalize(Vector3 * v1, Vector3 * v2);
Vector3 Vector3Transform(Vector3 v, Matrix mat);
Vector3 Vector3RotateByQuaternion(Vector3 v, Quaternion q);
Vector3 Vector3RotateByAxisAngle(Vector3 v, Vector3 axis, float angle);
Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, float amount);
Vector3 Vector3Reflect(Vector3 v, Vector3 normal);
Vector3 Vector3Min(Vector3 v1, Vector3 v2);
Vector3 Vector3Max(Vector3 v1, Vector3 v2);
Vector3 Vector3Barycenter(Vector3 p, Vector3 a, Vector3 b, Vector3 c);
Vector3 Vector3Unproject(Vector3 source, Matrix projection, Matrix view);
float3 Vector3ToFloatV(Vector3 v);
Vector3 Vector3Invert(Vector3 v);
Vector3 Vector3Clamp(Vector3 v, Vector3 min, Vector3 max);
Vector3 Vector3ClampValue(Vector3 v, float min, float max);
int Vector3Equals(Vector3 p, Vector3 q);
Vector3 Vector3Refract(Vector3 v, Vector3 n, float r);
float MatrixDeterminant(Matrix mat);
float MatrixTrace(Matrix mat);
Matrix MatrixTranspose(Matrix mat);
Matrix MatrixInvert(Matrix mat);
Matrix MatrixIdentity(void);
Matrix MatrixAdd(Matrix left, Matrix right);
Matrix MatrixSubtract(Matrix left, Matrix right);
Matrix MatrixMultiply(Matrix left, Matrix right);
Matrix MatrixTranslate(float x, float y, float z);
Matrix MatrixRotate(Vector3 axis, float angle);
Matrix MatrixRotateX(float angle);
Matrix MatrixRotateY(float angle);
Matrix MatrixRotateZ(float angle);
Matrix MatrixRotateXYZ(Vector3 angle);
Matrix MatrixRotateZYX(Vector3 angle);
Matrix MatrixScale(float x, float y, float z);
Matrix MatrixFrustum(double left, double right, double bottom, double top, double near, double far);
Matrix MatrixPerspective(double fovy, double aspect, double near, double far);
Matrix MatrixOrtho(double left, double right, double bottom, double top, double near, double far);
Matrix MatrixLookAt(Vector3 eye, Vector3 target, Vector3 up);
float16 MatrixToFloatV(Matrix mat);
Quaternion QuaternionAdd(Quaternion q1, Quaternion q2);
Quaternion QuaternionAddValue(Quaternion q, float add);
Quaternion QuaternionSubtract(Quaternion q1, Quaternion q2);
Quaternion QuaternionSubtractValue(Quaternion q, float sub);
Quaternion QuaternionIdentity(void);
float QuaternionLength(Quaternion q);
Quaternion QuaternionNormalize(Quaternion q);
Quaternion QuaternionInvert(Quaternion q);
Quaternion QuaternionMultiply(Quaternion q1, Quaternion q2);
Quaternion QuaternionScale(Quaternion q, float mul);
Quaternion QuaternionDivide(Quaternion q1, Quaternion q2);
Quaternion QuaternionLerp(Quaternion q1, Quaternion q2, float amount);
Quaternion QuaternionNlerp(Quaternion q1, Quaternion q2, float amount);
Quaternion QuaternionSlerp(Quaternion q1, Quaternion q2, float amount);
Quaternion QuaternionFromVector3ToVector3(Vector3 from, Vector3 to);
Quaternion QuaternionFromMatrix(Matrix mat);
Matrix QuaternionToMatrix(Quaternion q);
Quaternion QuaternionFromAxisAngle(Vector3 axis, float angle);
void QuaternionToAxisAngle(Quaternion q, Vector3 * outAxis, float * outAngle);
Quaternion QuaternionFromEuler(float pitch, float yaw, float roll);
Vector3 QuaternionToEuler(Quaternion q);
Quaternion QuaternionTransform(Quaternion q, Matrix mat);
int QuaternionEquals(Quaternion p, Quaternion q);

struct Vector2 
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		struct
		{
			float r;
			float g;
		};
		float _E[2];
	};
	
	Vector2 operator+(Vector2 array);
    Vector2 operator-(Vector2 array);
    Vector2 operator*(float scale);
    Vector2 operator/(float scale);
    Vector2 operator*(Vector2 array);	
    void operator+=(Vector2 array);
    void operator-=(Vector2 array);
	void operator*=(float scale);
	void operator/=(float scale);
	void operator*=(Vector2 array);
	
	float operator[](unsigned int index);
};

inline Vector2
V2(float x, float y)
{
	Vector2 res;
	res.x = x;
	res.y = y;
	
	return res;
}

inline float
Vector2::operator[](unsigned int index)
{
	return _E[index];
}

inline Vector2
Vector2::operator+(Vector2 v)
{
	return Vector2Add({ x, y }, v);
}

inline Vector2
Vector2::operator-(Vector2 v)
{
	return Vector2Subtract({ x, y }, v);
}

inline Vector2
Vector2::operator*(float scale)
{
	return Vector2Scale({ x, y }, scale);
}

inline Vector2
Vector2::operator/(float scale)
{
	return Vector2Scale({ x, y }, 1.f / scale);
}

inline Vector2
Vector2::operator*(Vector2 v)
{
	return Vector2Multiply({ x, y }, v);
}

inline void
Vector2::operator+=(Vector2 v)
{
	x += v.x;
	y += v.y;
}

inline void
Vector2::operator-=(Vector2 v)
{
	x -= v.x;
	y -= v.y;
}

inline void
Vector2::operator*=(float scale)
{
	x *= x * scale;
	y *= y * scale;
}

// NOTE(Ecy): vec product
inline void
Vector2::operator*=(Vector2 v)
{
	Vector2 res = Vector2Multiply({ x, y }, v);
	x = res.x;
	y = res.y;
}

inline void
Vector2::operator/=(float scale)
{
	x /= scale;
	y /= scale;
}

inline Vector2
operator*(float scale, Vector2 v)
{
	Vector2 res;
	res.x = v.x * scale;
	res.y = v.y * scale;
	
	return res;
}

struct Vector3 
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
		float _E[3];
	};
	
	Vector3 operator+(Vector3 array);
    Vector3 operator-(Vector3 array);
    Vector3 operator*(float scale);
    Vector3 operator/(float scale);
    Vector3 operator*(Vector3 array);	
    void operator+=(Vector3 array);
    void operator-=(Vector3 array);
	void operator*=(float scale);
	void operator/=(float scale);
	void operator*=(Vector3 array);
	
	float operator[](unsigned int index);
};

inline Vector3
V3(float x, float y, float z)
{
	Vector3 res;
	res.x = x;
	res.y = y;
	res.z = z;
	
	return res;
}

inline float
Vector3::operator[](unsigned int index)
{
	return _E[index];
}

inline Vector3
Vector3::operator+(Vector3 v)
{
	return Vector3Add({ x, y, z }, v);
}

inline Vector3
Vector3::operator-(Vector3 v)
{
	return Vector3Subtract({ x, y, z }, v);
}

inline Vector3
Vector3::operator*(float scale)
{
	return Vector3Scale({ x, y, z }, scale);
}

inline Vector3
Vector3::operator/(float scale)
{
	return Vector3Scale({ x, y, z }, 1.f / scale);
}

inline Vector3
Vector3::operator*(Vector3 v)
{
	return Vector3Multiply({ x, y, z }, v);
}

inline void
Vector3::operator+=(Vector3 v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

inline void
Vector3::operator-=(Vector3 v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

inline void
Vector3::operator*=(float scale)
{
	x *= x * scale;
	y *= y * scale;
	z *= z * scale;
}

// NOTE(Ecy): vec product
inline void
Vector3::operator*=(Vector3 v)
{
	Vector3 res = Vector3Multiply({ x, y, z }, v);
	x = res.x;
	y = res.y;
	z = res.z;
}

inline void
Vector3::operator/=(float scale)
{
	x /= scale;
	y /= scale;
	z /= scale;
}

inline Vector3
operator*(float scale, Vector3 v)
{
	Vector3 res;
	res.x = v.x * scale;
	res.y = v.y * scale;
	res.z = v.z * scale;
	
	return res;
}

struct Vector4
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		float _E[4];
	};
	
	Vector4 operator+(Vector4 array);
    Vector4 operator-(Vector4 array);
    Vector4 operator*(float scale);
    Vector4 operator/(float scale);
    Vector4 operator*(Vector4 array);	
    void operator+=(Vector4 array);
    void operator-=(Vector4 array);
	void operator*=(float scale);
	void operator/=(float scale);
	void operator*=(Vector4 array);
	
	float operator[](unsigned int index);
};

inline Vector4
V4(float x, float y, float z, float w)
{
	Vector4 res;
	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	
	return res;
}

inline float
Vector4::operator[](unsigned int index)
{
	return _E[index];
}

inline Vector4
Vector4::operator+(Vector4 v)
{
	return QuaternionAdd({ x, y, z, w }, v);
}

inline Vector4
Vector4::operator-(Vector4 v)
{
	return QuaternionSubtract({ x, y, z, w }, v);
}

inline Vector4
Vector4::operator*(float scale)
{
	return QuaternionScale({ x, y, z, w }, scale);
}

inline Vector4
Vector4::operator/(float scale)
{
	return QuaternionScale({ x, y, z, w }, 1.f / scale);
}

inline Vector4
Vector4::operator*(Vector4 v)
{
	return QuaternionMultiply({ x, y, z, w }, v);
}

inline void
Vector4::operator+=(Vector4 v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

inline void
Vector4::operator-=(Vector4 v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

inline void
Vector4::operator*=(float scale)
{
	x *= x * scale;
	y *= y * scale;
	z *= z * scale;
	w *= w * scale;
}

// NOTE(Ecy): vec product
inline void
Vector4::operator*=(Vector4 v)
{
	Vector4 res = QuaternionMultiply({ x, y, z, w }, v);
	x = res.x;
	y = res.y;
	z = res.z;
	w = res.w;
}

inline void
Vector4::operator/=(float scale)
{
	x /= scale;
	y /= scale;
	z /= scale;
	w /= scale;
}

inline Vector4
operator*(float scale, Vector4 v)
{
	Vector4 res;
	res.x = v.x * scale;
	res.y = v.y * scale;
	res.z = v.z * scale;
	res.w = v.w * scale;
	
	return res;
}

struct Matrix
{
	union
	{
		struct
		{
			float m0, m4, m8, m12;      // Matrix first row (4 components)
			float m1, m5, m9, m13;      // Matrix second row (4 components)
			float m2, m6, m10, m14;     // Matrix third row (4 components)
			float m3, m7, m11, m15;     // Matrix fourth row (4 components)
		};
		
		Vector4 _R[4];
		float _E[16];
	};
	
	Matrix operator+(Matrix mat);
    Matrix operator-(Matrix mat);
    Matrix operator*(Matrix mat);	
    void operator+=(Matrix mat);
    void operator-=(Matrix mat);
	void operator*=(Matrix mat);
	
	Vector4 operator[](unsigned int index);
};

inline Vector4
Matrix::operator[](unsigned int index)
{
	return _R[index];
}

inline Matrix
Matrix::operator+(Matrix mat)
{
	return MatrixAdd({
						 m0, m4, m8, m12,
						 m1, m5, m9, m13,
						 m2, m6, m10, m14,
						 m3, m7, m11, m15,
					 }, mat);
}

inline Matrix
Matrix::operator-(Matrix mat)
{
	return MatrixSubtract({
							  m0, m4, m8, m12,
							  m1, m5, m9, m13,
							  m2, m6, m10, m14,
							  m3, m7, m11, m15,
						  }, mat);
}

inline Matrix
Matrix::operator*(Matrix mat)
{
	return MatrixMultiply({
							  m0, m4, m8, m12,
							  m1, m5, m9, m13,
							  m2, m6, m10, m14,
							  m3, m7, m11, m15,
						  }, mat);
}

inline void
Matrix::operator+=(Matrix mat)
{
	Matrix res = MatrixAdd(*this, mat);
	memcpy(this, &res, sizeof(Matrix));
}

inline void
Matrix::operator-=(Matrix mat)
{
	Matrix res = MatrixSubtract(*this, mat);
	memcpy(this, &res, sizeof(Matrix));
}

inline void
Matrix::operator*=(Matrix mat)
{
	Matrix res = MatrixMultiply(*this, mat);
	memcpy(this, &res, sizeof(Matrix));
}

#define RAYMATH_IMPLEMENTATION
#ifdef near
    #undef near
#endif
#ifdef far
    #undef far
#endif
#include "../vendor/raylib/raymath.h"

inline Matrix
OrthographicProjection(Vector2 pos, float width, float height)
{
	float L = pos.x - width / 2.f;
	float R = pos.x + width / 2.f;
	float T = pos.y + height / 2.f;
	float B = pos.y - height / 2.f;
	Matrix res = 
	{
		2.0f/(R-L),   0.0f,           0.0f,       0.0f,
		0.0f,         2.0f/(T-B),     0.0f,       0.0f,
		0.0f,         0.0f,           0.5f,       0.0f,
		(R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f,
	};
	
	return res;
}

#endif //RAYMATH_EXT_H
