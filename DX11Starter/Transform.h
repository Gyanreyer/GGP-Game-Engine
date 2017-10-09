#pragma once
#include <DirectXMath.h>
#include <vector>

using std::vector;
using namespace DirectX;

class Transform
{
public:
	Transform();//Default constructor defaults to 0,0,0 pos and rot
	Transform(XMFLOAT3 pos, XMFLOAT3 rot);//Constructor takes position, rotation, and scale vectors
	~Transform();

	//Sets position of object, overloaded so may pass in individual x,y,z coords or an XMVECTOR
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 pos);
	void SetPosition(XMVECTOR pos);

	//Sets rotation of object, overloaded so may pass roll,pitch,yaw or an XMVECTOR
	void SetRotation(float roll, float pitch, float yaw);
	void SetRotation(XMFLOAT3 rot);
	void SetRotation(XMVECTOR rot);

	//Methods to modify current position/rotation by a given amount
	void Move(float x, float y, float z);
	void Move(XMVECTOR moveVec);
	void Rotate(float roll, float pitch, float yaw);
	void MoveRelative(float fwdSpeed, float sideSpeed, float upSpeed);//Move relative to forward and left dir vectors
	void MoveRelativeAxes(float fwdSpeed, float sideSpeed, float upSpeed);//Move relative to direction where fwd is always on xz plane and up is always y axis

	//Getter methods return vectors/matrices of object
	XMVECTOR GetPosition();
	XMVECTOR GetRotation();

	//Getters for direction vectors
	XMVECTOR GetForward();
	XMVECTOR GetForwardXZ();//Get forward vector with only x/z components
	XMVECTOR GetRight();
	XMVECTOR GetUp();

	XMMATRIX GetPositionMatrix();
	XMMATRIX GetRotationMatrix();

	//Parent transform methods
	void SetParent(Transform * parent);//Set a new parent for this transform
	void RemoveParent();//Remove this transform's current parent
	Transform * GetParent();//Return reference to this transform's parent

	//Child transform methods
	void AddChild(Transform * child);//Add a child to this transform
	void RemoveChild(int index);//Remove a child at a given index
	void RemoveChild(Transform * child);//Search for and remove a given transform from children
	Transform * GetChild(int index);//Return reference to transform at given index
	int GetChildCount();//Get number of children on this transform

protected:
	void NotifyMatrixUpdate();//Set matrixNeedsUpdate to true to notify transform that an updated is needed
	bool matrixNeedsUpdate;//Boolean tracks whether transform has been updated and requires a matrix update

	void UpdateDirectionVectors();

	XMVECTOR UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//Helper unit vector for up
	XMVECTOR FWD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);//Helper unit vector for forward

private:
	//Vectors for position/rotation
	XMFLOAT3 position, rotation;

	//Direction vectors
	XMFLOAT3 forward, right, up;

	Transform * parent;//Pointer to optional parent transform that this transform would center on
	vector<Transform *> children;//Vector of children that can center around this transform

	void StoreParent(Transform * parent);//Store pointer to given parent
	void StoreChild(Transform * child);//Add given child to vector
	void RemoveParentReference();//Set parent to nullptr
	void RemoveChildReference(int index);//Remove child at given index from vector
};

