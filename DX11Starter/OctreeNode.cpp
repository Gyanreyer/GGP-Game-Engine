#include "OctreeNode.h"

OctreeNode::OctreeNode()
{
}

OctreeNode::OctreeNode(XMFLOAT3 centerPoint, float boundingRadius, OctreeNode* parentNode)
{
	center = centerPoint;
	radius = boundingRadius;

	parent = parentNode;
}

OctreeNode::~OctreeNode()
{
}

vector<GameObject*> OctreeNode::GetObjects()
{
	return objects;
}

vector<GameObject*> OctreeNode::GetAllContainedObjects()
{
	vector<GameObject*> allObjects(objects);

	for (int i = 0; i < children.size(); i++)
	{
		vector<GameObject*> childObjects = children[i].GetAllContainedObjects();
		allObjects.insert(allObjects.end(), childObjects.begin(), childObjects.end());
	}
	
	return allObjects;
}

void OctreeNode::AddObject(GameObject * obj)
{
	//If this node is currently a leaf w/ no children
	if (IsLeafNode())
	{
		if (objects.size() < maxItems ||//Don't create new octant until surpass max # of items
			(radius / 2) < minBoundsSize) {//Prevent creating new octants if they'll be too small
			objects.push_back(obj);//Store object in objects vector
			return;
		}
		else//Create child octants
		{
			//Radius for child is half as wide as parent
			float newRadius = radius / 2;

			for (unsigned int i = 0; i < 8; i++)
			{
				/*Children organized like so:
				child: 0 1 2 3 4 5 6 7
				x:     - - - - + + + +
				y:     - - + + - - + +
				z:     - + - + - + - +
				*/
				XMFLOAT3 newCenter = center;//Calculate new center for child node
				newCenter.x += radius * (i & 4 ? 0.5f : -0.5f);
				newCenter.y += radius * (i & 2 ? 0.5f : -0.5f);
				newCenter.z += radius * (i & 1 ? 0.5f : -0.5f);

				children.push_back(OctreeNode(newCenter, newRadius, this));
			}

			vector<GameObject*>::iterator iter;
			//Transfer all objects stored in this node to its children
			for (iter = objects.begin(); iter != objects.end();)
			{
				int ind = GetContainingChildIndex((*iter)->GetTransform()->GetPosition(), (*iter)->GetCollider()->dimensions);

				if (ind >= 0)
				{
					children[ind].AddObject(*iter);
					iter = objects.erase(iter);
					continue;
				}
				else if (ind <= -2 && parent)
				{
					parent->AddObject(*iter);
					iter = objects.erase(iter);
					continue;
				}

				++iter;
			}
		}
	}
	//Attempt to place object in a child node, if it doesn't fit then store in this node or parent
	else
	{
		int ind = GetContainingChildIndex(obj->GetTransform()->GetPosition(), obj->GetCollider()->dimensions);

		if (ind >= 0)
		{
			children[ind].AddObject(obj);
		}
		else if (ind <= -2 && parent)
		{
			parent->AddObject(obj);
		}
		else
		{
			objects.push_back(obj);
		}
	}
}

//Return whether node has children
bool OctreeNode::IsLeafNode()
{
	return children.size() <= 0;
}

//Call every frame to make sure all objects are stored in right place still
void OctreeNode::Update()
{
	//If this isn't a leaf node, just update the children
	if (!IsLeafNode()) {
		for (unsigned int i = 0; i < 8; i++) {
			children[i].Update();
		}
	}

	if (objects.empty()) return;//Return early if no objects stored in this node

	//Iterate through all GameObjects in this node and check if they should remain in this node
	for (vector<GameObject*>::iterator iter = objects.begin(); iter != objects.end();) {
		//if ((*iter)->GetTransform()->MatrixNeedsUpdate()) {
			int ind = GetContainingChildIndex((*iter)->GetTransform()->GetPosition(), (*iter)->GetCollider()->dimensions);

			//Move object down to child node
			if (ind >= 0)
			{
				children[ind].AddObject(*iter);
				iter = objects.erase(iter);
				continue;
			}
			//Move object up to parent node
			else if (ind <= -2 && parent)
			{
				parent->AddObject(*iter);
				iter = objects.erase(iter);
				continue;
			}
		//}

		++iter;//Otherwise, move on to next obj
	}
}

OctreeNode * OctreeNode::GetParent()
{
	return parent;
}

vector<OctreeNode> OctreeNode::GetChildren()
{
	return children;
}

//Get index of child that contains given position
int OctreeNode::GetContainingChildIndex(XMFLOAT3 pos, XMFLOAT3 dimensions)
{
	//Get extents of object's collider
	XMFLOAT3 minExtents = XMFLOAT3(pos.x - dimensions.x, pos.y - dimensions.y, pos.z - dimensions.z);
	XMFLOAT3 maxExtents = XMFLOAT3(pos.x + dimensions.x, pos.y + dimensions.y, pos.z + dimensions.z);

	if (!IsLeafNode())
	{
		int centerIndex = ((pos.x > center.x)? 4:0) + ((pos.y > center.y)? 2:0) + ((pos.z > center.z)? 1:0);

		XMFLOAT3 octMin = children[centerIndex].GetMinPt();
		XMFLOAT3 octMax = children[centerIndex].GetMaxPt();

		if (minExtents.x >= octMin.x && maxExtents.x <= octMax.x &&
			minExtents.y >= octMin.y && maxExtents.y <= octMax.y &&
			minExtents.z >= octMin.z && maxExtents.z <= octMax.z)
		{
			return centerIndex;//Return calculated index if object fully fits into that node
		}
	}

	//Min and max bounds of this box
	XMFLOAT3 thisMin = GetMinPt();
	XMFLOAT3 thisMax = GetMaxPt();

	//Check if object fits within this node
	if (minExtents.x >= thisMin.x && maxExtents.x <= thisMax.x &&
		minExtents.y >= thisMin.y && maxExtents.y <= thisMax.y &&
		minExtents.z >= thisMin.z && maxExtents.z <= thisMax.z)
	{
		return -1;//Return -1 if fits within this node but not child nodes
	}

	return -2;//Return -2 if object didn't fit into this node (need to move up to parent)
}

//Get min bounds of this box
XMFLOAT3 OctreeNode::GetMinPt()
{
	return XMFLOAT3(center.x - radius, center.y - radius, center.z - radius);
}

//Get max bounds of this box
XMFLOAT3 OctreeNode::GetMaxPt()
{
	return XMFLOAT3(center.x + radius, center.y + radius, center.z + radius);
}
