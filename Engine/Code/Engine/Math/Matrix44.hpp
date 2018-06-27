#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\core\EngineCommon.hpp"
#include "Engine\Math\Vector4.hpp"

class Matrix44
{
public:
	Matrix44() {SetIdentity();} // default-construct to Identity matrix (via variable initialization)
	explicit Matrix44( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation);
	explicit Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation);
	explicit Matrix44(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation);

	// Accessors
	Vector2 TransformPosition2D( const Vector2& position2D ); // Written assuming z=0, w=1
	Vector2 TransformDisplacement2D( const Vector2& displacement2D ); // Written assuming z=0, w=0
	Vector3 TransformPosition3D(const Vector3& position3D); //Written assuming w = 0
	Vector3 TransformDisplacement3D(const Vector3& displacement3D); //Written assuming w=0

	//operator overloads

	const Vector4 operator*(const Vector4& vect );			// matrix44 * vector4

	// Mutators
	void SetEmpty();
	void SetIdentity();
	void SetInvalid();
	void SetValues( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	void SetValueAtIndex(int index, float value);
	void Append( const Matrix44& matrixToAppend ); // a.k.a. Concatenate (right-multiply)

	//2D transformations
	void RotateDegrees2D( float rotationDegreesAboutZ ); // 
	void Translate2D( const Vector2& translation );
	void ScaleUniform2D( float scaleXY );
	void Scale2D( float scaleX, float scaleY );

	//3d transformations
	void Translate3D( const Vector3& translation );
	void RotateDegreesAroundX3D(float rotationDegreesAroundX);
	void RotateDegreesAroundY3D(float rotationDegreesAroundY);
	void RotateDegreesAroundZ3D(float rotationDegreesAroundZ);
	void ScaleUniform3D( float scaleXYZ );
	void Scale3D( float scaleX, float scaleY, float scaleZ );
	Matrix44 TurnToward(Matrix44& target, float maxTurnAngle);

	//inversions and transposes
	void InvertFast();
	Matrix44 InvertFastToNew();
	bool Invert();
	void TransposeRotation();
	void Transpose();

	// Producers
	static Matrix44 MakeRotationDegrees2D( float rotationDegreesAboutZ );
	static Matrix44 MakeTranslation2D( const Vector2& translation );
	static Matrix44 MakeScaleUniform2D( float scaleXY );
	static Matrix44 MakeScale2D( float scaleX, float scaleY );
	static Matrix44 MakeOrtho2D( const Vector2& bottomLeft, const Vector2& topRight );

	Matrix44 MakeOrtho3D(const Vector3& center, const Vector2& dimensions, float near, float);

	static Matrix44 MakeTranslation3D( const Vector3& translation );
	static Matrix44 MakeRotationAroundX3D(float rotationDegreesAroundX);
	static Matrix44 MakeRotationAroundY3D(float rotationDegreesAroundY);
	static Matrix44 MakeRotationAroundZ3D(float rotationDegreesAroundZ);
	static Matrix44 MakeScaleUniform3D( float scaleXYZ );
	static Matrix44 MakeScale3D( float scaleX, float scaleY, float scaleZ ); 
	static Matrix44 LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::UP);

	Matrix44 CloneTemporaryMatrix44(const Matrix44& matrixToClone);
	Matrix44 SphericalInterpolateMatrix(Matrix44& start, Matrix44& end, float fractionTowardEnd);

	//getters
	float GetValueAtIndex(int index);
	float GetValueAtIndex(int indexI, int indexJ);
	float GetTrace(); //get axis angle

	Vector4 GetXRow() const{ return Vector4(Ix, Jx, Kx, Tx);}; 
	Vector4 GetYRow() const{ return Vector4(Iy, Jy, Ky, Ty);};
	Vector4 GetZRow() const{ return Vector4(Iz, Jz, Kz, Tz);};
	Vector4 GetWRow() const{ return Vector4(Iw, Jw, Kw, Tw);};

	Vector4 GetIBasis() const { return Vector4(Ix, Iy, Iz, Iw);};
	Vector4 GetJBasis() const { return Vector4(Jx, Jy, Jz, Jw);};
	Vector4 GetKBasis() const { return Vector4(Kx, Ky, Kz, Kw);};
	Vector4 GetTBasis() const { return Vector4(Tx, Ty, Tz, Tw);};

	Vector3 GetPosition() const {return Vector3(Tx, Ty, Tz);};
	Vector3 GetTranslation() const {return GetPosition();};
	Vector3 GetRotation() const;
	Vector3 GetScale() const;

	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Vector3 GetForward() const;

	Vector2 GetPosition2D() const{return Vector2(Tx, Ty); };
	Vector2 GetTranslation2D() const{return GetPosition2D(); };
	Vector2 GetRight2D() const;
	Vector2 GetUp2D() const;

	Matrix44 GetInverse();

	//setters
	void SetFromBasisVectors(Vector4 iBasis, Vector4 jBasis, Vector4 kBasis, Vector4 tBasis);	
	void SetXRow(const Vector4& row); 
	void SetYRow(const Vector4& row);
	void SetZRow(const Vector4& row);
	void SetWRow(const Vector4& row);

	void SetIBasis(const Vector4& basis);
	void SetJBasis(const Vector4& basis);
	void SetKBasis(const Vector4& basis);
	void SetTBasis(const Vector4& basis);

	void SetTranslation(const Vector3& translation);

	//static methods
	static Matrix44 MakeRotationMatrix(Vector3 rotation);
	static Vector3 GetRotationFromMatrix(const Matrix44& matrix);

	//projection
	void PerspectiveProjection(float fovDegrees, float aspect, float nearZ, float farZ);


public:
	float Ix = 1.0f;
	float Iy = 0.0f;
	float Iz = 0.0f;
	float Iw = 0.0f;
	float Jx = 0.0f;
	float Jy = 1.0f;
	float Jz = 0.0f;
	float Jw = 0.0f;
	float Kx = 0.0f;
	float Ky = 0.0f;
	float Kz = 1.0f;
	float Kw = 0.0f;
	float Tx = 0.0f;
	float Ty = 0.0f;
	float Tz = 0.0f;
	float Tw = 1.0f;

public:
	//static methods
	static const Matrix44 IDENTITY;
	
};


