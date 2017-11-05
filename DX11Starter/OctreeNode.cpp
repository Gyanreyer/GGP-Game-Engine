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
	if(children) delete[] children;
}

GameObject * OctreeNode::GetObjects()
{
	return *objects.data();
}

unsigned int OctreeNode::GetObjectCount()
{
	return objects.size();
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
			children = new OctreeNode[8];

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

				children[i] = OctreeNode(newCenter, newRadius, this);
			}

			vector<GameObject*>::iterator iter;
			//Transfer all objects stored in this node to its children
			for (iter = objects.begin(); iter != objects.end();)
			{
				if (MoveObject(*iter))
					objects.erase(iter);
				else
					++iter;
			}
		}
	}
	//Attempt to place object in child, if doesn't fit then store in this node
	else if (!MoveObject(obj))
	{
		objects.push_back(obj);
	}

}

//Return whether node has children
bool OctreeNode::IsLeafNode()
{
	return children == nullptr;
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
		//Check if object's transform has been updated and if so, if it needs to be moved
		if ((*iter)->GetTransform()->MatrixNeedsUpdate() && MoveObject(*iter))
			objects.erase(iter);//If object was moved, erase it from this node
		else
			++iter;//Otherwise, move on to next obj
	}
}

//Get index of child that contains given position
int OctreeNode::GetContainingChildIndex(XMFLOAT3 pos, XMFLOAT3 dimensions)
{
	//Get extents of object's collider
	XMFLOAT3 minExtents = XMFLOAT3(pos.x - dimensions.x, pos.y - dimensions.y, pos.z - dimensions.z);
	XMFLOAT3 maxExtents = XMFLOAT3(pos.x + dimensions.x, pos.y + dimensions.y, pos.z + dimensions.z);

	//If this node has children, check if object fits into any of them
	if (!IsLeafNode())
	{
		//Loop through child nodes and see if object fits inside any
		for (unsigned int i = 0; i < 8; i++) {
			OctreeNode oct = children[i];

			XMFLOAT3 min = oct.GetMinPt();
			XMFLOAT3 max = oct.GetMaxPt();

			//Check if in bounds of this node
			if (minExtents.x >= min.x && maxExtents.x <= max.x &&
				minExtents.y >= min.y && maxExtents.y <= max.y &&
				minExtents.z >= min.z && maxExtents.z <= max.z)
			{
				return i;//If in bounds of a node, return that node's index
			}
		}
	}

	//Min and max bounds of this box
	XMFLOAT3 thisMin = GetMinPt();
	XMFLOAT3 thisMax = GetMaxPt();

	//Check if object still fits within this node
	if (minExtents.x >= thisMin.x && maxExtents.x <= thisMax.x &&
		minExtents.y >= thisMin.y && maxExtents.y <= thisMax.y &&
		minExtents.z >= thisMin.z && maxExtents.z <= thisMax.z)
	{
		return -1;//Return -1 if fits within this node but not child nodes
	}

	return -2;//Return -2 if object didn't fit into this node (need to move up to parent)
}

//Moves object to other node as appropriate and returns whether moved or not
bool OctreeNode::MoveObject(GameObject * obj)
{
	//Get index of child node object fits in
	int childIndex = GetContainingChildIndex(obj->GetTransform()->GetPosition(), obj->GetCollider()->dimensions);

	//If index is valid, add object to child
	if (childIndex >= 0)
	{
		children[childIndex].AddObject(obj);
		return true;
	}
	//If index is -2 it doesn't fit in this node, move up to parent
	else if (childIndex == -2)
	{
		parent->AddObject(obj);
		return true;
	}

	return false;//Return false if shouldn't be removed from this node
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
