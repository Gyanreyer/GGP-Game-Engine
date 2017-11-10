#pragma once
#include <vector>
#include "Projectile.h"
#include "Player.h"

class OctreeNode
{
public:
	OctreeNode();
	OctreeNode(XMFLOAT3 centerPoint, float boundingRadius, OctreeNode* parentNode);
	~OctreeNode();

	GameObject * GetObjects();
	unsigned int GetObjectCount();

	void AddObject(GameObject * obj);

	bool IsLeafNode();

	void Update();

private:
	vector<GameObject*> objects;

	OctreeNode * parent;
	OctreeNode * children;

	XMFLOAT3 center;
	float radius;

	int GetContainingChildIndex(XMFLOAT3 pos, XMFLOAT3 dimensions);
	bool MoveObject(GameObject* obj);

	XMFLOAT3 GetMinPt();
	XMFLOAT3 GetMaxPt();

	int maxItems = 7;
	float minBoundsSize = 1;
};

