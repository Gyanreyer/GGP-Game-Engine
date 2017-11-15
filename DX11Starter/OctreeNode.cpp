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
	for (byte i = 0; i < children.size(); i++)
	{
		children[i]->~OctreeNode();
		delete children[i];
	}

	children.clear();
}

vector<GameObject*> OctreeNode::GetObjects()
{
	return objects;
}

vector<GameObject*> OctreeNode::GetAllSubObjects(bool includeOwn)
{
	vector<GameObject*> allObjects;

	if (includeOwn) allObjects = objects;

	for (int i = 0; i < children.size(); i++)
	{
		vector<GameObject*> childObjects = children[i]->GetAllSubObjects(true);
		allObjects.insert(allObjects.end(), childObjects.begin(), childObjects.end());
	}
	
	return allObjects;
}

vector<GameObject*> OctreeNode::GetAllObjectsForCollision()
{
	vector<GameObject*> allObjects;//Initialize with this node's objects

	//Traverse up to top level parent to get any higher level objects we should check against
	OctreeNode * currentParentNode = parent;

	while (currentParentNode != nullptr)
	{
		vector<GameObject*> parentObjects = currentParentNode->GetObjects();
		allObjects.insert(allObjects.end(), parentObjects.begin(), parentObjects.end());//Insert objects from higher levels inton vector

		currentParentNode = currentParentNode->GetParent();
	}

	//Insert all contained objects from this node and children
	vector<GameObject *> subObjects = GetAllSubObjects(true);
	allObjects.insert(allObjects.end(), subObjects.begin(), subObjects.end());

	return allObjects;//Return vector of all objects to check against
}

void OctreeNode::AddObject(GameObject * obj)
{
	objects.push_back(obj);//Store object in objects vector
	obj->SetOctNode(this);

	//If this node is currently a leaf w/ no children
	if (IsLeafNode() &&
		objects.size() > maxItems &&//Create new octant when surpass max # of items
		(radius / 2) > minBoundsSize)//Prevent creating new octants if they'll be too small
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

			children.push_back(new OctreeNode(newCenter, newRadius, this));
		}

		UpdateAll();
	}
	else
	{
		UpdateObject((objects.end() - 1));
	}
}

void OctreeNode::RemoveObject(GameObject * obj)
{
	for (vector<GameObject *>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
	{
		if (*iter == obj)
		{
			objects.erase(iter);
			return;
		}
	}
}

//Return whether node has children
bool OctreeNode::IsLeafNode()
{
	return children.size() <= 0;
}

//Call every frame to make sure all objects are stored in right place still
void OctreeNode::UpdateAll()
{
	//Iterate through all GameObjects in this node and check if they should remain in this node
	for (vector<GameObject*>::iterator iter = objects.begin(); iter != objects.end();) {
		if ((*iter)->GetTransform()->MatrixNeedsUpdate())
		{
			iter = UpdateObject(iter);
		}
		else
		{
			++iter;//Otherwise, move on to next obj
		}
	}

	//If this isn't a leaf node, update the children
	if (!IsLeafNode()) {
		//Get all sub objects excluding this node's
		vector<GameObject *> contained = GetAllSubObjects(false);

		//Get rid of children if not enough objects in them
		if (contained.size() + objects.size() < maxItems)
		{
			//Free memory of all child nodes and then clear them
			for (byte i = 0; i < children.size(); i++)
			{
				children[i]->~OctreeNode();
				delete children[i];
			}

			children.clear();

			//Shift contained objects from children to this node instead
			for (byte i = 0; i < contained.size(); i++)
			{
				AddObject(contained[i]);
			}
		}
		else//Otherwise, update objects in children
		{
			for (unsigned int i = 0; i < 8; i++) {
				children[i]->UpdateAll();//Update all the objects in child node
			}
		}
	}
}

vector<GameObject *>::iterator OctreeNode::UpdateObject(vector<GameObject *>::iterator objIter)
{
	int ind = GetContainingChildIndex((*objIter)->GetTransform()->GetPosition(), (*objIter)->GetCollider()->dimensions);

	//If ind is 0+ then place it in the corresponding child node
	if (ind >= 0)
	{
		children[ind]->AddObject(*objIter);
		return objects.erase(objIter);
	}

	//If ind is -2 then move up to parent if possible
	else if (ind == -2 && parent != nullptr)
	{
		parent->AddObject(*objIter);
		return objects.erase(objIter);
	}

	//If ind was -1 then don't move, return next iterator
	return objIter + 1;
}

OctreeNode * OctreeNode::GetParent()
{
	return parent;
}

vector<OctreeNode *> OctreeNode::GetChildren()
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

		XMFLOAT3 octMin = children[centerIndex]->GetMinPt();
		XMFLOAT3 octMax = children[centerIndex]->GetMaxPt();

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
