#pragma once
#include <DirectXMath.h>
#include <vector>

using std::vector;
using namespace DirectX;

class Transform
{
public:
	Transform();//Default constructor defaults to 0,0,0 pos and rot
	Transform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);//Constructor takes position, rotation, and scale vectors
	~Transform();

	//Sets position of object, overloaded so may pass in individual x,y,z coords or an XMVECTOR
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 pos);
	void SetPosition(XMVECTOR pos);

	//Sets rotation of object, overloaded so may pass roll,pitch,yaw or an XMVECTOR
	void SetRotation(float roll, float pitch, float yaw);
	void SetRotation(XMFLOAT3 rot);
	void SetRotation(XMVECTOR rot);

	void SetScale(float scalar);
	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3 scl);
	void SetScale(XMVECTOR scl);

	//Methods to modify current position/rotation by a given amount
	void Move(float x, float y, float z);
	void Move(XMFLOAT3 moveVec);
	void Move(XMVECTOR moveVec);
	void Rotate(float roll, float pitch, float yaw);
	void MoveRelative(float fwdSpeed, float sideSpeed, float upSpeed);//Move relative to forward and left dir vectors
	void MoveRelativeAxes(float fwdSpeed, float sideSpeed, float upSpeed);//Move relative to direction where fwd is always on xz plane and up is always y axis

	//Getter methods return vectors/matrices of object
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();

	//Getters for direction vectors
	XMFLOAT3 GetForward();
	XMFLOAT3 GetForwardXZ();//Get forward vector with only x/z components
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();

	bool MatrixNeedsUpdate();//Whether any important variables in transform have changed
	void DoneUpdating();//Call to mark that matrix updates from this transform are complete

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

private:
	//Vectors for position/rotation/scale
	XMFLOAT3 position, rotation, scale;

	//Direction vectors
	XMFLOAT3 forward, right, up;

	//Matrix for object's world mat or camera's view mat
	XMFLOAT4X4 matrix;

	Transform * parent;//Pointer to optional parent transform that this transform would center on
	vector<Transform *> children;//Vector of children that can center around this transform

	void StoreParent(Transform * parent);//Store pointer to given parent
	void StoreChild(Transform * child);//Add given child to vector
	void RemoveParentReference();//Set parent to nullptr
	void RemoveChildReference(int index);//Remove child at given index from vector

	void NotifyMatrixUpdate();//Set matrixNeedsUpdate to true to notify transform that an updated is needed
	bool matrixNeedsUpdate;//Boolean tracks whether transform has been updated and requires a matrix update

	void UpdateDirectionVectors();

	XMVECTOR UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//Helper unit vector for up
	XMVECTOR FWD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);//Helper unit vector for forward
};

