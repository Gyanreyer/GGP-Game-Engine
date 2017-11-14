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

	vector<GameObject*> GetObjects();
	vector<GameObject*> GetAllContainedObjects();

	void AddObject(GameObject * obj);
	void RemoveObject(GameObject * obj);

	bool IsLeafNode();

	void Update();

	OctreeNode * GetParent();
	vector<OctreeNode> GetChildren();

private:
	vector<GameObject*> objects;

	OctreeNode * parent;
	vector<OctreeNode> children;

	XMFLOAT3 center;
	float radius;

	int GetContainingChildIndex(XMFLOAT3 pos, XMFLOAT3 dimensions);

	XMFLOAT3 GetMinPt();
	XMFLOAT3 GetMaxPt();

	int maxItems = 8;
	float minBoundsSize = 1;
};

