#include "Engine\Math\Matrix44.hpp"
#include "Engine\Math\Vector4.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <cmath>

TODO("Refactor matrix class so make methods aren't creating pointers");
const Matrix44 Matrix44::IDENTITY = Matrix44();

Matrix44::Matrix44( const float* sixteenValuesBasisMajor ) // float[16] array in order Ix, Iy...
{
	Ix = *sixteenValuesBasisMajor;
	Iy = *(sixteenValuesBasisMajor + 1);
	Iz = *(sixteenValuesBasisMajor + 2);
	Iw = *(sixteenValuesBasisMajor + 3);
	Jx = *(sixteenValuesBasisMajor + 4);
	Jy = *(sixteenValuesBasisMajor + 5);
	Jz = *(sixteenValuesBasisMajor + 6);
	Jw = *(sixteenValuesBasisMajor + 7);
	Kx = *(sixteenValuesBasisMajor + 8);
	Ky = *(sixteenValuesBasisMajor + 9);
	Kz = *(sixteenValuesBasisMajor + 10);
	Kw = *(sixteenValuesBasisMajor + 11);
	Tx = *(sixteenValuesBasisMajor + 12);
	Ty = *(sixteenValuesBasisMajor + 13);
	Tz = *(sixteenValuesBasisMajor + 14);
	Tw = *(sixteenValuesBasisMajor + 15);
}

Matrix44::Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation=Vector2(0.f,0.f) )
{
	SetIdentity();
	Ix = iBasis.x;
	Iy = iBasis.y;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Tx = translation.x;
	Ty = translation.y;
}


Matrix44::Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation=Vector3(0.f, 0.f, 0.f))
{
	SetIdentity();
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;
	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
}

Matrix44::Matrix44(const Vector4& iBasis, const Vector4& jBasis, const Vector4& kBasis, const Vector4& translation)
{
	SetIdentity();
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Iw = iBasis.w;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;
	Jw = jBasis.w;
	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Kw = kBasis.w;
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = translation.w;
}


//operators
const Vector4 Matrix44::operator*(const Vector4& vect )
{
	return Vector4(DotProduct(GetXRow(), vect),
		DotProduct(GetYRow(), vect),
		DotProduct(GetZRow(), vect),
		DotProduct(GetWRow(), vect)
		);
}

// Accessors
Vector2 Matrix44::TransformPosition2D( const Vector2& position2D ) // Written assuming z=0, w=1
{
	Vector2 newPosition;
	newPosition.x = (Ix * position2D.x) + (Jx * position2D.y) + (Tx);
	newPosition.y = (Iy * position2D.x) + (Jy * position2D.y) + (Ty);

	return newPosition;
}
Vector2 Matrix44::TransformDisplacement2D( const Vector2& displacement2D ) // Written assuming z=0, w=0
{
	Vector2 newPosition;
	newPosition.x = (Ix * displacement2D.x) + (Jx * displacement2D.y);
	newPosition.y = (Iy * displacement2D.x) + (Jy * displacement2D.y);

	return newPosition;
}

Vector3 Matrix44::TransformPosition3D(const Vector3& position3D) //Written assuming w = 0
{
	Vector3 newPosition;
	newPosition.x = (Ix * position3D.x) + (Jx * position3D.y) + (Kx * position3D.z) + (Tx);
	newPosition.y = (Iy * position3D.x) + (Jy * position3D.y) + (Ky * position3D.z) + (Ty);
	newPosition.z = (Iz * position3D.x) + (Jz * position3D.y) + (Kz * position3D.z) + (Tz);
	
	return newPosition;
}

Vector3 Matrix44::TransformDisplacement3D(const Vector3& displacement3D) //Written assuming w=0
{
	Vector3 newPosition;
	newPosition.x = (Ix * displacement3D.x) + (Jx * displacement3D.y) + (Kx * displacement3D.x);
	newPosition.y = (Iy * displacement3D.x) + (Jy * displacement3D.y) + (Ky * displacement3D.y);
	newPosition.z = (Iz * displacement3D.z) + (Jz * displacement3D.z) + (Kz * displacement3D.z);

	return newPosition;
}

																  // Mutators
void Matrix44::SetIdentity()
{
	Ix = 1.0f;
	Iy = 0.0f;
	Iz = 0.0f;
	Iw = 0.0f;
	Jx = 0.0f;
	Jy = 1.0f;
	Jz = 0.0f;
	Jw = 0.0f;
	Kx = 0.0f;
	Ky = 0.0f;
	Kz = 1.0f;
	Kw = 0.0f;
	Tx = 0.0f;
	Ty = 0.0f;
	Tz = 0.0f;
	Tw = 1.0f;
}

void Matrix44::SetInvalid()
{
	Ix = 9999.0f;
	Iy = 9999.0f;
	Iz = 9999.0f;
	Iw = 9999.0f;
	Jx = 9999.0f;
	Jy = 9999.0f;
	Jz = 9999.0f;
	Jw = 9999.0f;
	Kx = 9999.0f;
	Ky = 9999.0f;
	Kz = 9999.0f;
	Kw = 9999.0f;
	Tx = 9999.0f;
	Ty = 9999.0f;
	Tz = 9999.0f;
	Tw = 9999.0f;
}

void Matrix44::SetEmpty()
{
	Ix = 0.0f;
	Iy = 0.0f;
	Iz = 0.0f;
	Iw = 0.0f;
	Jx = 0.0f;
	Jy = 0.0f;
	Jz = 0.0f;
	Jw = 0.0f;
	Kx = 0.0f;
	Ky = 0.0f;
	Kz = 0.0f;
	Kw = 0.0f;
	Tx = 0.0f;
	Ty = 0.0f;
	Tz = 0.0f;
	Tw = 0.0f;
}

void Matrix44::SetValues( const float* sixteenValuesBasisMajor ) // float[16] array in order Ix, Iy...
{
	Ix = *sixteenValuesBasisMajor;
	Iy = *sixteenValuesBasisMajor + 1;
	Iz = *sixteenValuesBasisMajor + 2;
	Iw = *sixteenValuesBasisMajor + 3;
	Jx = *sixteenValuesBasisMajor + 4;
	Jy = *sixteenValuesBasisMajor + 5;
	Jz = *sixteenValuesBasisMajor + 6;
	Jw = *sixteenValuesBasisMajor + 7;
	Kx = *sixteenValuesBasisMajor + 8;
	Ky = *sixteenValuesBasisMajor + 9;
	Kz = *sixteenValuesBasisMajor + 10;
	Kw = *sixteenValuesBasisMajor + 11;
	Tx = *sixteenValuesBasisMajor + 12;
	Ty = *sixteenValuesBasisMajor + 13;
	Tz = *sixteenValuesBasisMajor + 14;
	Tw = *sixteenValuesBasisMajor + 15;
}
void Matrix44::Append( const Matrix44& matrixToAppend ) // a.k.a. Concatenate (right-multiply)
{
	Matrix44 tempMatrix = CloneTemporaryMatrix44(*this);

	//I
	Ix = (tempMatrix.Ix * matrixToAppend.Ix) + (tempMatrix.Jx * matrixToAppend.Iy) + (tempMatrix.Kx * matrixToAppend.Iz) + (tempMatrix.Tx * matrixToAppend.Iw);
	Iy = (tempMatrix.Iy * matrixToAppend.Ix) + (tempMatrix.Jy * matrixToAppend.Iy) + (tempMatrix.Ky * matrixToAppend.Iz) + (tempMatrix.Ty * matrixToAppend.Iw);
	Iz = (tempMatrix.Iz * matrixToAppend.Ix) + (tempMatrix.Jz * matrixToAppend.Iy) + (tempMatrix.Kz * matrixToAppend.Iz) + (tempMatrix.Tz * matrixToAppend.Iw);
	Iw = (tempMatrix.Iw * matrixToAppend.Ix) + (tempMatrix.Jw * matrixToAppend.Iy) + (tempMatrix.Kw * matrixToAppend.Iz) + (tempMatrix.Tw * matrixToAppend.Iw);
	//J
	Jx = (tempMatrix.Ix * matrixToAppend.Jx) + (tempMatrix.Jx * matrixToAppend.Jy) + (tempMatrix.Kx * matrixToAppend.Jz) + (tempMatrix.Tx * matrixToAppend.Jw);
	Jy = (tempMatrix.Iy * matrixToAppend.Jx) + (tempMatrix.Jy * matrixToAppend.Jy) + (tempMatrix.Ky * matrixToAppend.Jz) + (tempMatrix.Ty * matrixToAppend.Jw);
	Jz = (tempMatrix.Iz * matrixToAppend.Jx) + (tempMatrix.Jz * matrixToAppend.Jy) + (tempMatrix.Kz * matrixToAppend.Jz) + (tempMatrix.Tz * matrixToAppend.Jw);
	Jw = (tempMatrix.Iw * matrixToAppend.Jx) + (tempMatrix.Jw * matrixToAppend.Jy) + (tempMatrix.Kw * matrixToAppend.Jz) + (tempMatrix.Tw * matrixToAppend.Jw);
	//K
	Kx = (tempMatrix.Ix * matrixToAppend.Kx) + (tempMatrix.Jx * matrixToAppend.Ky) + (tempMatrix.Kx * matrixToAppend.Kz) + (tempMatrix.Tx * matrixToAppend.Kw);
	Ky = (tempMatrix.Iy * matrixToAppend.Kx) + (tempMatrix.Jy * matrixToAppend.Ky) + (tempMatrix.Ky * matrixToAppend.Kz) + (tempMatrix.Ty * matrixToAppend.Kw);
	Kz = (tempMatrix.Iz * matrixToAppend.Kx) + (tempMatrix.Jz * matrixToAppend.Ky) + (tempMatrix.Kz * matrixToAppend.Kz) + (tempMatrix.Tz * matrixToAppend.Kw);
	Kw = (tempMatrix.Iw * matrixToAppend.Kx) + (tempMatrix.Jw * matrixToAppend.Ky) + (tempMatrix.Kw * matrixToAppend.Kz) + (tempMatrix.Tw * matrixToAppend.Kw);
	//T
	Tx = (tempMatrix.Ix * matrixToAppend.Tx) + (tempMatrix.Jx * matrixToAppend.Ty) + (tempMatrix.Kx * matrixToAppend.Tz) + (tempMatrix.Tx * matrixToAppend.Tw);
	Ty = (tempMatrix.Iy * matrixToAppend.Tx) + (tempMatrix.Jy * matrixToAppend.Ty) + (tempMatrix.Ky * matrixToAppend.Tz) + (tempMatrix.Ty * matrixToAppend.Tw);
	Tz = (tempMatrix.Iz * matrixToAppend.Tx) + (tempMatrix.Jz * matrixToAppend.Ty) + (tempMatrix.Kz * matrixToAppend.Tz) + (tempMatrix.Tz * matrixToAppend.Tw);
	Tw = (tempMatrix.Iw * matrixToAppend.Tx) + (tempMatrix.Jw * matrixToAppend.Ty) + (tempMatrix.Kw * matrixToAppend.Tz) + (tempMatrix.Tw * matrixToAppend.Tw);
}


void Matrix44::RotateDegrees2D( float rotationDegreesAboutZ ) //
{
	Matrix44 rotationMatrix = Matrix44();
	rotationMatrix.SetIdentity();

	rotationMatrix.Ix = CosDegrees(rotationDegreesAboutZ);
	rotationMatrix.Iy = SinDegrees(rotationDegreesAboutZ);
	rotationMatrix.Jx = -1 * SinDegrees(rotationDegreesAboutZ);
	rotationMatrix.Jy = CosDegrees(rotationDegreesAboutZ);

	this->Append(rotationMatrix);
}
void Matrix44::Translate2D( const Vector2& translation )
{
	Matrix44 translationMatrix = Matrix44();
	translationMatrix.SetIdentity();

	translationMatrix.Tx = translation.x;
	translationMatrix.Ty = translation.y;

	this->Append(translationMatrix);
}
void Matrix44::ScaleUniform2D( float scaleXY )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleXY;
	scaleMatrix.Jy *= scaleXY;

	this->Append(scaleMatrix);
}
void Matrix44::Scale2D( float scaleX, float scaleY )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleX;
	scaleMatrix.Jy *= scaleY;

	this->Append(scaleMatrix);
}

void Matrix44::Translate3D( const Vector3& translation )
{
	Matrix44 translationMatrix = Matrix44();
	translationMatrix.SetIdentity();

	translationMatrix.Tx = translation.x;
	translationMatrix.Ty = translation.y;
	translationMatrix.Tz = translation.z;

	this->Append(translationMatrix);
}

void Matrix44::RotateDegreesAroundX3D(float rotationDegreesAroundX)
{
	Matrix44 rotationMatrix = Matrix44();
	rotationMatrix.SetIdentity();

	rotationMatrix.Jy = CosDegrees(rotationDegreesAroundX);
	rotationMatrix.Jz = -1.f * SinDegrees(rotationDegreesAroundX);
	rotationMatrix.Ky = SinDegrees(rotationDegreesAroundX);
	rotationMatrix.Kz = CosDegrees(rotationDegreesAroundX);

	this->Append(rotationMatrix);
}

void Matrix44::RotateDegreesAroundY3D(float rotationDegreesAroundY)
{
	Matrix44 rotationMatrix = Matrix44();
	rotationMatrix.SetIdentity();
				  
	rotationMatrix.Ix = CosDegrees(rotationDegreesAroundY);
	rotationMatrix.Iz = -1.f * SinDegrees(rotationDegreesAroundY);
	rotationMatrix.Kx = SinDegrees(rotationDegreesAroundY);
	rotationMatrix.Kz = CosDegrees(rotationDegreesAroundY);

	this->Append(rotationMatrix);
}	

void Matrix44::RotateDegreesAroundZ3D(float rotationDegreesAroundZ)
{	
	Matrix44 rotationMatrix = Matrix44();
	rotationMatrix.SetIdentity();

	rotationMatrix.Ix = CosDegrees(rotationDegreesAroundZ);
	rotationMatrix.Iy = SinDegrees(rotationDegreesAroundZ);
	rotationMatrix.Jx = -1.f * SinDegrees(rotationDegreesAroundZ);
	rotationMatrix.Jy = CosDegrees(rotationDegreesAroundZ);

	this->Append(rotationMatrix);
}

void Matrix44::ScaleUniform3D( float scaleXYZ )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleXYZ;
	scaleMatrix.Jy *= scaleXYZ;
	scaleMatrix.Kz *= scaleXYZ;

	this->Append(scaleMatrix);
}

void Matrix44::Scale3D( float scaleX, float scaleY, float scaleZ )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleX;
	scaleMatrix.Jy *= scaleY;
	scaleMatrix.Kz *= scaleZ;

	this->Append(scaleMatrix);
}

Matrix44 Matrix44::TurnToward(Matrix44& target, float maxTurnAngle)
{
	//store off position to stick back in matrix after we've rotated
	Matrix44 inverse = this->GetInverse();
	inverse.Append(target);
	Matrix44 rotation = inverse;

	// trace is 1 + 2 cos(theta) == sum of diagonal
	float trace = rotation.GetTrace();

	// trace = 1 + 2.f * cos(theta)
	// theta = acos( (trace - 1) *.5f ); 
	float inner = (trace - 1.0f) * .5f; 
	inner = ClampFloat( inner, -1.0f, 1.0f ); 
	float theta = AcosfAsDegrees( trace - 1.0f, 1.0f); 

	float t = GetMinFloat( maxTurnAngle/theta, 1.0f );

	//handles infinity condition
	if (isnan(t))
	{
		t = 0.1f;
	}

	Matrix44 finalMatrix = SphericalInterpolateMatrix( *this, target, t ); 
	finalMatrix.SetTranslation(this->GetPosition());

	return finalMatrix;	
}

// Producers
Matrix44 Matrix44::MakeRotationDegrees2D( float rotationDegreesAboutZ )
{
	Matrix44 rotationMatrix = Matrix44();
	rotationMatrix.SetIdentity();

	rotationMatrix.Ix = CosDegrees(rotationDegreesAboutZ);
	rotationMatrix.Iy = SinDegrees(rotationDegreesAboutZ);
	rotationMatrix.Jx = -1 * SinDegrees(rotationDegreesAboutZ);
	rotationMatrix.Jy = CosDegrees(rotationDegreesAboutZ);

	return rotationMatrix;
}
Matrix44 Matrix44::MakeTranslation2D( const Vector2& translation )
{
	Matrix44 translationMatrix = Matrix44();
	translationMatrix.SetIdentity();
					 
	translationMatrix.Tx += translation.x;
	translationMatrix.Ty += translation.y;

	return translationMatrix;
}
Matrix44 Matrix44::MakeScaleUniform2D( float scaleXY )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();
			   
	scaleMatrix.Ix *= scaleXY;
	scaleMatrix.Jy *= scaleXY;

	return scaleMatrix;
}
Matrix44 Matrix44::MakeScale2D( float scaleX, float scaleY )
{
	Matrix44 scaleMatrix = Matrix44();
	scaleMatrix.SetIdentity();
			   
	scaleMatrix.Ix *= scaleX;
	scaleMatrix.Jy *= scaleY;

	return scaleMatrix;
}
Matrix44 Matrix44::MakeOrtho2D( const Vector2& bottomLeft, const Vector2& topRight )
{
	Matrix44 orthoMatrix;
	orthoMatrix.SetIdentity();

	orthoMatrix.Tx = (-1 *((topRight.x + bottomLeft.x)/ (topRight.x - bottomLeft.x)));
	orthoMatrix.Ty = (-1 *((topRight.y + bottomLeft.y)/ (topRight.y - bottomLeft.y)));
	orthoMatrix.Tz = -1*(1 + 0)/(1-0);
			   
	orthoMatrix.Ix = 2/(topRight.x - bottomLeft.x);
	orthoMatrix.Jy = 2/(topRight.y - bottomLeft.y);
	orthoMatrix.Kz = 2;

	return orthoMatrix;
}

Matrix44 Matrix44::MakeOrtho3D(const Vector3& center, const Vector2& dimensions, float nearPlane, float farPlane)
{
	Matrix44 orthoMatrix;

	Vector2 topRight = Vector2(center.x + (dimensions.x * 0.5f), center.y + (dimensions.y * 0.5f));
	Vector2 bottomLeft = Vector2(center.x - (dimensions.x * 0.5f), center.y - (dimensions.y * 0.5f));

	orthoMatrix.Tx = (-1 *((topRight.x + bottomLeft.x)/ (topRight.x - bottomLeft.x)));
	orthoMatrix.Ty = (-1 *((topRight.y + bottomLeft.y)/ (topRight.y - bottomLeft.y)));
	orthoMatrix.Tz = -1*(farPlane + nearPlane)/(farPlane - nearPlane);
	
	orthoMatrix.Ix = 2/(topRight.x - bottomLeft.x);
	orthoMatrix.Jy = 2/(topRight.y - bottomLeft.y);
	orthoMatrix.Kz = 2/(farPlane - nearPlane);

	return orthoMatrix;
}



Matrix44 Matrix44::MakeTranslation3D( const Vector3& translation )
{
	Matrix44 translationMatrix;
	translationMatrix.SetIdentity();

	translationMatrix.Tx += translation.x;
	translationMatrix.Ty += translation.y;
	translationMatrix.Tz += translation.z;

	return translationMatrix;
}

Matrix44 Matrix44::MakeRotationAroundX3D(float rotationDegreesAroundX)
{
	Matrix44 rotationMatrix;
	rotationMatrix.SetIdentity();

	if(fmodf(rotationDegreesAroundX, 360.f) != 0.f)
	{
		rotationMatrix.Jy = CosDegrees(rotationDegreesAroundX);
		rotationMatrix.Jz = -1.f * SinDegrees(rotationDegreesAroundX);
		rotationMatrix.Ky = SinDegrees(rotationDegreesAroundX);
		rotationMatrix.Kz = CosDegrees(rotationDegreesAroundX);
	}	

	return rotationMatrix;
}

Matrix44 Matrix44::MakeRotationAroundY3D(float rotationDegreesAroundY)
{
	Matrix44 rotationMatrix;
	rotationMatrix.SetIdentity();

	if(fmodf(rotationDegreesAroundY, 360.f) != 0.f)
	{
		rotationMatrix.Ix = CosDegrees(rotationDegreesAroundY);
		rotationMatrix.Iz = -1.f * SinDegrees(rotationDegreesAroundY);
		rotationMatrix.Kx = SinDegrees(rotationDegreesAroundY);
		rotationMatrix.Kz = CosDegrees(rotationDegreesAroundY);
	}

	return rotationMatrix;
}

Matrix44 Matrix44::MakeRotationAroundZ3D(float rotationDegreesAroundZ)
{
	Matrix44 rotationMatrix;
	rotationMatrix.SetIdentity();

	if(fmodf(rotationDegreesAroundZ, 360.f) != 0.f)
	{
		rotationMatrix.Ix = CosDegrees(rotationDegreesAroundZ);
		rotationMatrix.Iy = SinDegrees(rotationDegreesAroundZ);
		rotationMatrix.Jx = -1.f * SinDegrees(rotationDegreesAroundZ);
		rotationMatrix.Jy = CosDegrees(rotationDegreesAroundZ);
	}

	return rotationMatrix;
}

Matrix44 Matrix44::MakeScaleUniform3D( float scaleXYZ )
{
	Matrix44 scaleMatrix;
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleXYZ;
	scaleMatrix.Jy *= scaleXYZ;
	scaleMatrix.Kz *= scaleXYZ;

	return scaleMatrix;
}

Matrix44 Matrix44::MakeScale3D( float scaleX, float scaleY, float scaleZ )
{
	Matrix44 scaleMatrix;
	scaleMatrix.SetIdentity();

	scaleMatrix.Ix *= scaleX;
	scaleMatrix.Jy *= scaleY;
	scaleMatrix.Kz *= scaleZ;

	return scaleMatrix;
}

Matrix44 Matrix44::CloneTemporaryMatrix44(const Matrix44& matrixToClone)
{
	Matrix44 clonedMatrix44 = Matrix44();
	clonedMatrix44.Ix = matrixToClone.Ix;
	clonedMatrix44.Iy = matrixToClone.Iy;
	clonedMatrix44.Iz = matrixToClone.Iz;
	clonedMatrix44.Iw = matrixToClone.Iw;
				  
	clonedMatrix44.Jx = matrixToClone.Jx;
	clonedMatrix44.Jy = matrixToClone.Jy;
	clonedMatrix44.Jz = matrixToClone.Jz;
	clonedMatrix44.Jw = matrixToClone.Jw;
				  
	clonedMatrix44.Kx = matrixToClone.Kx;
	clonedMatrix44.Ky = matrixToClone.Ky;
	clonedMatrix44.Kz = matrixToClone.Kz;
	clonedMatrix44.Kw = matrixToClone.Kw;
				  
	clonedMatrix44.Tx = matrixToClone.Tx;
	clonedMatrix44.Ty = matrixToClone.Ty;
	clonedMatrix44.Tz = matrixToClone.Tz;
	clonedMatrix44.Tw = matrixToClone.Tw;

	return clonedMatrix44;
}

Matrix44 Matrix44::SphericalInterpolateMatrix(Matrix44& start, Matrix44& end, float fractionTwoardEnd)
{
	Vector3 startRight = start.GetRight();
	Vector3 endRight = end.GetRight();
	
	Vector3 startUp = start.GetUp();
	Vector3 endUp = end.GetUp();

	Vector3 startForward = start.GetForward();
	Vector3 endForward = end.GetForward();
	
	Vector3 startTranslation = start.GetTranslation();
	Vector3 endTranslation = end.GetTranslation();

	//interpolate
	Vector3 right = SphericalInterpolate(startRight, endRight, fractionTwoardEnd);
	Vector3 up = SphericalInterpolate(startUp, endUp, fractionTwoardEnd);
	Vector3 forward = SphericalInterpolate(startForward, endForward, fractionTwoardEnd);
	Vector3 translation = Interpolate(startTranslation, endTranslation, fractionTwoardEnd);

	return Matrix44(right, up, forward, translation);
}

Matrix44 Matrix44::LookAt(const Vector3& position, const Vector3& target, const Vector3& up)
{
	Vector3 zAxis = target - position;
	zAxis.Normalize();

	Vector3 xAxis = CrossProduct(up, zAxis);
	xAxis.Normalize();

	Vector3 yAxis = CrossProduct(zAxis, xAxis);
	yAxis.Normalize(); //should already be normalized.

	/*float xAxisDot = -(DotProduct(xAxis, position));
	float yAxisDot = -(DotProduct(yAxis, position));
	float zAxisDot = -(DotProduct(zAxis, position));*/

	Matrix44 lookAtMatrix = Matrix44(Vector4(xAxis, 0.f), Vector4(yAxis, 0.f), Vector4(zAxis, 0.f), Vector4(position, 1.f));

	return lookAtMatrix;
}

void Matrix44::InvertFast()
{
	Vector3 translation = GetPosition();
	SetTranslation(Vector3::ZERO);
	TransposeRotation();

	/*Following code is equivalent to:
	translation = (vec4(-translation, 0.0f) * (*this)).xyz()
	SetTranslation( translation );*/

	SetTranslation( Vector3( 
		-translation.Dot( GetRight() ), 
		-translation.Dot( GetUp() ), 
		-translation.Dot( GetForward() )
	)); 
	
}

Matrix44 Matrix44::InvertFastToNew()
{
	Matrix44 invertedMatrix = *this;

	Vector3 translation = invertedMatrix.GetPosition();
	invertedMatrix.SetTranslation(Vector3::ZERO);
	invertedMatrix.TransposeRotation();

	/*Following code is equivalent to:*
	translation = invertedMatrix.TransformPosition3D( -translation ); 
	invertedMatrix.SetTranslation( translation );/**/

	/**/
	invertedMatrix.SetTranslation( Vector3( 
		-translation.Dot( Vector3(invertedMatrix.GetXRow()) ), 
		-translation.Dot( Vector3(invertedMatrix.GetYRow()) ), 
		-translation.Dot( Vector3(invertedMatrix.GetZRow()) )
	)); 
	/**/

	return invertedMatrix;
}

void Matrix44::TransposeRotation()
{
	Matrix44 tempMatrix = *this;

	this->Jx = tempMatrix.Iy;
	this->Iy = tempMatrix.Jx;

	this->Kx = tempMatrix.Iz;
	this->Iz = tempMatrix.Kx;

	this->Jz = tempMatrix.Ky;
	this->Ky = tempMatrix.Jz;
}

void  Matrix44::Transpose()
{
	Matrix44 tempMatrix = *this;

	this->Jx = tempMatrix.Iy;
	this->Iy = tempMatrix.Jx;

	this->Kx = tempMatrix.Iz;
	this->Iz = tempMatrix.Kx;

	this->Jz = tempMatrix.Ky;
	this->Ky = tempMatrix.Jz;

	this->Tz = tempMatrix.Kw;
	this->Kw = tempMatrix.Tz;
}

Matrix44 Matrix44::MakeRotationMatrix(Vector3 rotation)
{
	float cosX = CosDegrees(rotation.x);
	float sinX = SinDegrees(rotation.x);

	float cosY = CosDegrees(rotation.y);
	float sinY = SinDegrees(rotation.y);

	float cosZ = CosDegrees(rotation.z);
	float sinZ = SinDegrees(rotation.z);

	Matrix44 rotationMatrix;

	//i basis
	float iBasisx = (cosZ * cosY) + (sinZ * sinX * sinY);
	float iBasisy = sinZ * cosX;
	float iBasisz = (-1.f * cosZ * sinY) + (sinZ * sinX * cosY);
	float iBasisw = 0.f;

	//jbasis
	float jBasisx = (-1.f * sinZ * cosY) + (cosZ * sinX * sinY);
	float jBasisy = cosZ * cosX;
	float jBasisz = (sinZ * sinY) + (cosZ * sinX * cosY);
	float jBasisw = 0.f;

	//kbasis
	float kBasisX = cosX * sinY;
	float kBasisY = (-1.f * sinX);
	float kBasisZ = cosX * cosY;
	float kBasisW = 0.f;

	rotationMatrix.SetIBasis(Vector4(iBasisx, iBasisy, iBasisz, iBasisw));
	rotationMatrix.SetJBasis(Vector4(jBasisx, jBasisy, jBasisz, jBasisw));
	rotationMatrix.SetKBasis(Vector4(kBasisX, kBasisY, kBasisZ, kBasisW));
	rotationMatrix.SetTBasis(Vector4(0.f, 0.f, 0.f, 1.f));

	return rotationMatrix;
}

Vector3 Matrix44::GetRotationFromMatrix(const Matrix44& matrix)
{
	return matrix.GetRotation();
}

//xyz rows
void Matrix44::SetXRow(const Vector4& row)
{
	Ix = row.x;
	Jx = row.y;
	Kx = row.z;
	Tx = row.w;
}

void Matrix44::SetYRow(const Vector4& row)
{
	Iy = row.x;
	Jy = row.y;
	Ky = row.z;
	Ty = row.w;
}

void Matrix44::SetZRow(const Vector4& row)
{
	Iz = row.x;
	Jz = row.y;
	Kz = row.z;
	Tz = row.w;
}

void Matrix44::SetWRow(const Vector4& row)
{
	Iw = row.x;
	Jw = row.y;
	Kw = row.z;
	Tw = row.w;
}

//basis
void Matrix44::SetIBasis(const Vector4& basis)
{
	Ix = basis.x;
	Iy = basis.y;
	Iz = basis.z;
	Iw = basis.w;
}

void Matrix44::SetJBasis(const Vector4& basis)
{
	Jx = basis.x;
	Jy = basis.y;
	Jz = basis.z;
	Jw = basis.w;
}

void Matrix44::SetKBasis(const Vector4& basis)
{
	Kx = basis.x;
	Ky = basis.y;
	Kz = basis.z;
	Kw = basis.w;
}

void Matrix44::SetTBasis(const Vector4& basis)
{
	Tx = basis.x;
	Ty = basis.y;
	Tz = basis.z;
	Tw = basis.w;
}

void Matrix44::SetTranslation(const Vector3& translation)
{
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
	Tw = 1.f;
}


Vector3 Matrix44::GetRotation() const
{
	float xDegrees;
	float yDegrees;
	float zDegrees;

	float sx = -1.f * Ky;
	sx = AsinfAsDegrees(sx, 1.f);
	sx = ClampFloat(sx, -90.f, 90.f);
	xDegrees = sx;

	float cx = CosDegrees(sx);
	if(cx != 0.0f)
	{
		yDegrees = AtanfAsDegrees(Kx, Kz);
		zDegrees = AtanfAsDegrees(Iy, Jy);
	}
	else
	{
		yDegrees = AtanfAsDegrees((-1.f * Iz), Ix);
		zDegrees = 0.0f;
	}

	return Vector3(xDegrees, yDegrees, zDegrees);
}

Vector3 Matrix44::GetScale() const
{
	return Vector3( GetIBasis().GetLength(), GetJBasis().GetLength(), GetKBasis().GetLength());	
}

Vector3 Matrix44::GetRight() const
{
	Vector3 rightVector = Vector3(GetIBasis());
	rightVector.Normalize();
	return rightVector;
}

Vector3 Matrix44::GetUp() const
{
	Vector3 upVector = Vector3(GetJBasis());
	upVector.Normalize();
	return upVector;
}

Vector3 Matrix44::GetForward() const
{
	Vector3 forwardVector = Vector3(GetKBasis());
	forwardVector.Normalize();
	return forwardVector;
}

Vector2 Matrix44::GetRight2D() const
{
	Vector3 forwardVector = Vector3(GetIBasis());
	forwardVector.Normalize();
	return Vector2(forwardVector.x, forwardVector.y);
}

Vector2 Matrix44::GetUp2D() const
{
	Vector3 forwardVector = Vector3(GetKBasis());
	forwardVector.Normalize();
	return Vector2(forwardVector.x, forwardVector.y);
}



void Matrix44::SetFromBasisVectors(Vector4 iBasis, Vector4 jBasis, Vector4 kBasis, Vector4 tBasis)
{
	SetIBasis(iBasis);
	SetJBasis(jBasis);
	SetKBasis(kBasis);
	SetTBasis(tBasis);
}

void Matrix44::PerspectiveProjection(float fovDegrees, float aspect, float nearZ, float farZ)
{
	float d = 1.0f / TanDegrees(fovDegrees/2.f);
	float q = 1.0f / (farZ - nearZ);

	//if aspect > 1 (d/aspect) else (d * aspect)

	Vector4 iBasis = Vector4( d / aspect, 0.f,    0.f,                   0.f );  
	Vector4 jBasis = Vector4( 0.f,          d,    0.f,                   0.f ); 
	Vector4 kBasis = Vector4( 0.f,          0.f,    (nearZ + farZ) * q,       1.f );
	Vector4 tBasis = Vector4( 0.f,          0.f,    -2.0f * nearZ * farZ * q, 0.f ); 

	SetFromBasisVectors( iBasis, jBasis, kBasis, tBasis );
}

/*
===============
|ix	jx	kx	tx|
|iy	jy	ky	ty|
|iz	jz	kz	tz|
|iw	jw	kw	tw|
===============
	EQUALS
===============
|0	4	8	12|
|1	5	9	13|
|2	6	10	14|
|3	7	11	15|
===============

*/

float Matrix44::GetValueAtIndex(int index)
{
	index = ClampInt(index, 0, 15);

	float value = 0.0f;
	switch (index)
	{
		case 0:
			value = Ix;
			break;
		case 1:
			value = Iy;
			break;
		case 2:
			value = Iz;
			break;
		case 3:
			value = Iw;
			break;
		case 4:
			value = Jx;
			break;
		case 5:
			value = Jy;
			break;
		case 6:
			value = Jz;
			break;
		case 7:
			value = Jw;
			break;
		case 8:
			value = Kx;
			break;
		case 9:
			value = Ky;
			break;
		case 10:
			value = Kz;
			break;
		case 11:
			value = Kw;
			break;
		case 12:
			value = Tx;
			break;
		case 13:
			value = Ty;
			break;
		case 14:
			value = Tz;
			break;
		case 15:
			value = Tw;
			break;			
	}

	return value;
}

float Matrix44::GetValueAtIndex(int indexI, int indexJ)
{
	//insure indexes are inside matrix bounds
	indexI = ClampInt(indexI, 0, 3);
	indexJ = ClampInt(indexJ, 0, 3);

	return GetValueAtIndex((indexI * 4) + indexJ);
}

float Matrix44::GetTrace()
{
	float sum = 0.0f;

	for (int matrixIndex = 0; matrixIndex < 3; ++matrixIndex)
	{
		sum += GetValueAtIndex(matrixIndex, matrixIndex); //go along the diagonal and sum the values
	}

	return sum;
}

/*
===============
|ix	jx	kx	tx|
|iy	jy	ky	ty|
|iz	jz	kz	tz|
|iw	jw	kw	tw|
===============
EQUALS
===============
|0	4	8	12|
|1	5	9	13|
|2	6	10	14|
|3	7	11	15|
===============

*/

void Matrix44::SetValueAtIndex(int index, float value)
{
	switch (index)
	{
	case 0:
		Ix = value;
		break;
	case 1:
		Iy = value;
		break;
	case 2:
		Iz = value;
		break;
	case 3:
		Iw = value;
		break;
	case 4:
		Jx = value;
		break;
	case 5:
		Jy = value;
		break;
	case 6:
		Jz = value;
		break;
	case 7:
		Jw = value;
		break;
	case 8:
		Kx = value;
		break;
	case 9:
		Ky = value;
		break;
	case 10:
		Kz = value;
		break;
	case 11:
		Kw = value;
		break;
	case 12:
		Tx = value;
		break;
	case 13:
		Ty = value;
		break;
	case 14:
		Tz = value;
		break;
	case 15:
		Tw = value;
		break;
	}
}

bool Matrix44::Invert()
{
	Matrix44 inverse;
	float determinant = 0.0f;
	float value = 0.0f;

	value = GetValueAtIndex(5) * GetValueAtIndex(10) * GetValueAtIndex(15) -
		GetValueAtIndex(5) * GetValueAtIndex(11) * GetValueAtIndex(14) -
		GetValueAtIndex(9) * GetValueAtIndex(6) * GetValueAtIndex(15) +
		GetValueAtIndex(9) * GetValueAtIndex(7) * GetValueAtIndex(14) +
		GetValueAtIndex(13) * GetValueAtIndex(6) * GetValueAtIndex(11) -
		GetValueAtIndex(13) * GetValueAtIndex(7) * GetValueAtIndex(10);
	inverse.SetValueAtIndex(0, value);

	value = -GetValueAtIndex(4) * GetValueAtIndex(10) * GetValueAtIndex(15) +
		GetValueAtIndex(4) * GetValueAtIndex(11) * GetValueAtIndex(14) +
		GetValueAtIndex(8) * GetValueAtIndex(6) * GetValueAtIndex(15) -
		GetValueAtIndex(8) * GetValueAtIndex(7) * GetValueAtIndex(14) -
		GetValueAtIndex(12) * GetValueAtIndex(6) * GetValueAtIndex(11) +
		GetValueAtIndex(12) * GetValueAtIndex(7) * GetValueAtIndex(10);
	inverse.SetValueAtIndex(4, value);

	value = GetValueAtIndex(4) * GetValueAtIndex(9) * GetValueAtIndex(15) -
		GetValueAtIndex(4) * GetValueAtIndex(11) * GetValueAtIndex(13) -
		GetValueAtIndex(8) * GetValueAtIndex(5) * GetValueAtIndex(15) +
		GetValueAtIndex(8) * GetValueAtIndex(7) * GetValueAtIndex(13) +
		GetValueAtIndex(12) * GetValueAtIndex(5) * GetValueAtIndex(11) -
		GetValueAtIndex(12) * GetValueAtIndex(7) * GetValueAtIndex(9);
	inverse.SetValueAtIndex(8, value);

	value = -GetValueAtIndex(4) * GetValueAtIndex(9) * GetValueAtIndex(14) +
		GetValueAtIndex(4) * GetValueAtIndex(10) * GetValueAtIndex(13) +
		GetValueAtIndex(8) * GetValueAtIndex(5) * GetValueAtIndex(14) -
		GetValueAtIndex(8) * GetValueAtIndex(6) * GetValueAtIndex(13) -
		GetValueAtIndex(12) * GetValueAtIndex(5) * GetValueAtIndex(10) +
		GetValueAtIndex(12) * GetValueAtIndex(6) * GetValueAtIndex(9);
	inverse.SetValueAtIndex(12, value);

	value = -GetValueAtIndex(1) * GetValueAtIndex(10) * GetValueAtIndex(15) +
		GetValueAtIndex(1) * GetValueAtIndex(11) * GetValueAtIndex(14) +
		GetValueAtIndex(9) * GetValueAtIndex(2) * GetValueAtIndex(15) -
		GetValueAtIndex(9) * GetValueAtIndex(3) * GetValueAtIndex(14) -
		GetValueAtIndex(13) * GetValueAtIndex(2) * GetValueAtIndex(11) +
		GetValueAtIndex(13) * GetValueAtIndex(3) * GetValueAtIndex(10);
	inverse.SetValueAtIndex(1, value);

	value = GetValueAtIndex(0) * GetValueAtIndex(10) * GetValueAtIndex(15) -
		GetValueAtIndex(0) * GetValueAtIndex(11) * GetValueAtIndex(14) -
		GetValueAtIndex(8) * GetValueAtIndex(2) * GetValueAtIndex(15) +
		GetValueAtIndex(8) * GetValueAtIndex(3) * GetValueAtIndex(14) +
		GetValueAtIndex(12) * GetValueAtIndex(2) * GetValueAtIndex(11) -
		GetValueAtIndex(12) * GetValueAtIndex(3) * GetValueAtIndex(10);
	inverse.SetValueAtIndex(5, value);

	value = -GetValueAtIndex(0) * GetValueAtIndex(9) * GetValueAtIndex(15) +
		GetValueAtIndex(0) * GetValueAtIndex(11) * GetValueAtIndex(13) +
		GetValueAtIndex(8) * GetValueAtIndex(1) * GetValueAtIndex(15) -
		GetValueAtIndex(8) * GetValueAtIndex(3) * GetValueAtIndex(13) -
		GetValueAtIndex(12) * GetValueAtIndex(1) * GetValueAtIndex(11) +
		GetValueAtIndex(12) * GetValueAtIndex(3) * GetValueAtIndex(9);
	inverse.SetValueAtIndex(9, value);

	value = GetValueAtIndex(0) * GetValueAtIndex(9) * GetValueAtIndex(14) -
		GetValueAtIndex(0) * GetValueAtIndex(10) * GetValueAtIndex(13) -
		GetValueAtIndex(8) * GetValueAtIndex(1) * GetValueAtIndex(14) +
		GetValueAtIndex(8) * GetValueAtIndex(2) * GetValueAtIndex(13) +
		GetValueAtIndex(12) * GetValueAtIndex(1) * GetValueAtIndex(10) -
		GetValueAtIndex(12) * GetValueAtIndex(2) * GetValueAtIndex(9);
	inverse.SetValueAtIndex(13, value);

	value = GetValueAtIndex(1) * GetValueAtIndex(6) * GetValueAtIndex(15) -
		GetValueAtIndex(1) * GetValueAtIndex(7) * GetValueAtIndex(14) -
		GetValueAtIndex(5) * GetValueAtIndex(2) * GetValueAtIndex(15) +
		GetValueAtIndex(5) * GetValueAtIndex(3) * GetValueAtIndex(14) +
		GetValueAtIndex(13) * GetValueAtIndex(2) * GetValueAtIndex(7) -
		GetValueAtIndex(13) * GetValueAtIndex(3) * GetValueAtIndex(6);
	inverse.SetValueAtIndex(2, value);

	value = -GetValueAtIndex(0) * GetValueAtIndex(6) * GetValueAtIndex(15) +
		GetValueAtIndex(0) * GetValueAtIndex(7) * GetValueAtIndex(14) +
		GetValueAtIndex(4) * GetValueAtIndex(2) * GetValueAtIndex(15) -
		GetValueAtIndex(4) * GetValueAtIndex(3) * GetValueAtIndex(14) -
		GetValueAtIndex(12) * GetValueAtIndex(2) * GetValueAtIndex(7) +
		GetValueAtIndex(12) * GetValueAtIndex(3) * GetValueAtIndex(6);
	inverse.SetValueAtIndex(6, value);

	value = GetValueAtIndex(0) * GetValueAtIndex(5) * GetValueAtIndex(15) -
		GetValueAtIndex(0) * GetValueAtIndex(7) * GetValueAtIndex(13) -
		GetValueAtIndex(4) * GetValueAtIndex(1) * GetValueAtIndex(15) +
		GetValueAtIndex(4) * GetValueAtIndex(3) * GetValueAtIndex(13) +
		GetValueAtIndex(12) * GetValueAtIndex(1) * GetValueAtIndex(7) -
		GetValueAtIndex(12) * GetValueAtIndex(3) * GetValueAtIndex(5);
	inverse.SetValueAtIndex(10, value);

	value = -GetValueAtIndex(0) * GetValueAtIndex(5) * GetValueAtIndex(14) +
		GetValueAtIndex(0) * GetValueAtIndex(6) * GetValueAtIndex(13) +
		GetValueAtIndex(4) * GetValueAtIndex(1) * GetValueAtIndex(14) -
		GetValueAtIndex(4) * GetValueAtIndex(2) * GetValueAtIndex(13) -
		GetValueAtIndex(12) * GetValueAtIndex(1) * GetValueAtIndex(6) +
		GetValueAtIndex(12) * GetValueAtIndex(2) * GetValueAtIndex(5);
	inverse.SetValueAtIndex(14, value);

	value = -GetValueAtIndex(1) * GetValueAtIndex(6) * GetValueAtIndex(11) +
		GetValueAtIndex(1) * GetValueAtIndex(7) * GetValueAtIndex(10) +
		GetValueAtIndex(5) * GetValueAtIndex(2) * GetValueAtIndex(11) -
		GetValueAtIndex(5) * GetValueAtIndex(3) * GetValueAtIndex(10) -
		GetValueAtIndex(9) * GetValueAtIndex(2) * GetValueAtIndex(7) +
		GetValueAtIndex(9) * GetValueAtIndex(3) * GetValueAtIndex(6);
	inverse.SetValueAtIndex(3, value);

	value = GetValueAtIndex(0) * GetValueAtIndex(6) * GetValueAtIndex(11) -
		GetValueAtIndex(0) * GetValueAtIndex(7) * GetValueAtIndex(10) -
		GetValueAtIndex(4) * GetValueAtIndex(2) * GetValueAtIndex(11) +
		GetValueAtIndex(4) * GetValueAtIndex(3) * GetValueAtIndex(10) +
		GetValueAtIndex(8) * GetValueAtIndex(2) * GetValueAtIndex(7) -
		GetValueAtIndex(8) * GetValueAtIndex(3) * GetValueAtIndex(6);
	inverse.SetValueAtIndex(7, value);

	value = -GetValueAtIndex(0) * GetValueAtIndex(5) * GetValueAtIndex(11) +
		GetValueAtIndex(0) * GetValueAtIndex(7) * GetValueAtIndex(9) +
		GetValueAtIndex(4) * GetValueAtIndex(1) * GetValueAtIndex(11) -
		GetValueAtIndex(4) * GetValueAtIndex(3) * GetValueAtIndex(9) -
		GetValueAtIndex(8) * GetValueAtIndex(1) * GetValueAtIndex(7) +
		GetValueAtIndex(8) * GetValueAtIndex(3) * GetValueAtIndex(5);
	inverse.SetValueAtIndex(11, value);

	value = GetValueAtIndex(0) * GetValueAtIndex(5) * GetValueAtIndex(10) -
		GetValueAtIndex(0) * GetValueAtIndex(6) * GetValueAtIndex(9) -
		GetValueAtIndex(4) * GetValueAtIndex(1) * GetValueAtIndex(10) +
		GetValueAtIndex(4) * GetValueAtIndex(2) * GetValueAtIndex(9) +
		GetValueAtIndex(8) * GetValueAtIndex(1) * GetValueAtIndex(6) -
		GetValueAtIndex(8) * GetValueAtIndex(2) * GetValueAtIndex(5);
	inverse.SetValueAtIndex(15, value);

	determinant = GetValueAtIndex(0) * inverse.GetValueAtIndex(0) + GetValueAtIndex(1) * inverse.GetValueAtIndex(4) + GetValueAtIndex(2) * inverse.GetValueAtIndex(8) + GetValueAtIndex(3) * inverse.GetValueAtIndex(12);

	if (determinant == 0.0f)
	{
		return false;
	}

	determinant = 1.f / determinant;

	for (int matrixIndex = 0; matrixIndex < 16; ++matrixIndex)
	{
		SetValueAtIndex(matrixIndex, inverse.GetValueAtIndex(matrixIndex) * determinant);
	}

	return true;
}



Matrix44 Matrix44::GetInverse()
{
	Matrix44 inverseMatrix = *this;
	bool success = inverseMatrix.Invert();

	if (success == false)
	{
		inverseMatrix.SetInvalid();
	}

	return inverseMatrix;
}