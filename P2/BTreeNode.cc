#include "BTreeNode.h"

using namespace std;


BTLeafNode::BTLeafNode(){
	memset(buffer, 0, PageFile::PAGE_SIZE);
	PageId* pid = (PageId*) (buffer + PageFile::PAGE_SIZE - sizeof(PageId));
}

BTLeafNode::~BTLeafNode(){
	delete[] buffer;
}


/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ return pf.read(pid, buffer); }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ return pf.write(pid, buffer); }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer;
	for(int i = 0; i < MAX_NUM_ENTRY; i++){
		if(ep->key == 0)
			return i;
		ep++;
	}  
	return MAX_NUM_ENTRY;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{
	int key_cnt = getKeyCount();
	if(key_cnt >= MAX_NUM_ENTRY)
		return RC_NODE_FULL;

	int eid = 0;
	if(key_cnt > 0)
		locate(key, eid);

	LeafNodeEntry* ep = (LeafNodeEntry*) buffer + eid;
	if(key_cnt > 0 && eid != key_cnt)
		memmove(ep+1, ep, (key_cnt - eid)*sizeof(LeafNodeEntry));

	ep->key = key;
	ep->rid = rid;
	return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ 
	int key_cnt = getKeyCount();
	int half_key_cnt = (key_cnt+1)/2;
	int insert_current = 0;
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer + half_key_cnt;

	if(key < ep[-1].key)
	{	half_key_cnt--; ep--; insert_current=1;	}

	siblingKey = ep->key;
	for(int i = 0; i < key_cnt - half_key_cnt; i++){
		sibling.insert(ep[i].key, ep[i].rid);
	}
	memset(ep, 0, (key_cnt - half_key_cnt) * sizeof(LeafNodeEntry));

	if(insert_current == 1)
		insert(key, rid);
	else {
		sibling.insert(key,rid);
		if(key < siblingKey)
			siblingKey = key;
	}

	return 0; 
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer;
	int key_count = getKeyCount();
	for(int i = 0; i < key_count; i++){
		if(ep->key == searchKey){
			eid = i;
			return 0;
		}
		else if(ep->key > searchKey){
			eid = i;
			return RC_NO_SUCH_RECORD;
		}
		ep++;
	}
	eid = key_count;
	return RC_NO_SUCH_RECORD; 
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	if(eid >= getKeyCount())
		return RC_INVALID_EID;
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer + eid;
	key = ep->key;
	rid = ep->rid;
	return 0; 
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	int offset = PageFile::PAGE_SIZE - sizeof(PageId);
	PageId *p = (PageId*) (buffer + offset); 
	return *p;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 	
	int offset = PageFile::PAGE_SIZE - sizeof(PageId);
	PageId *p = (PageId*) (buffer + offset); 
	*p = pid;
	return 0;
}

void BTLeafNode::print()
{
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer;
	int key_cnt = getKeyCount();
	printf("----------Print this Leaf Node --------------\n");
	printf("Key Count = %d\n", key_cnt);
	for(int i = 0; i < key_cnt; i++){
		printf("%d | key: %d\n", 
				i, ep->key);
		ep++;
	}
	PageId *p = (PageId*) (buffer + PageFile::PAGE_SIZE - sizeof(PageId));
	printf("pid: %d\n\n", *p);
	printf("---------------------------------------------\n");
}

void BTLeafNode::rec_print(string space, PageId pid){
	LeafNodeEntry* ep = (LeafNodeEntry*) buffer;
	int key_cnt = getKeyCount();
	printf("%s--Leaf Node ---\n", space.c_str());
	printf("%sNode Pid = %d\n", space.c_str(), pid);
	printf("%sKey Count = %d\n", space.c_str(), key_cnt);

	for(int i = 0; i < key_cnt; i++){
		printf("%s%d. rid: %d | key: %d\n", 
				space.c_str(), i, ep[i].rid.pid, ep[i].key);
	}

	PageId *p = (PageId*) (buffer + PageFile::PAGE_SIZE - sizeof(PageId));
	printf("%slast pid: %d\n", space.c_str(), *p);
	printf("%s---------------\n\n", space.c_str());
}

//====================================================================
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//====================================================================

BTNonLeafNode::BTNonLeafNode(){
	memset(buffer, 0, PageFile::PAGE_SIZE);
}

BTNonLeafNode::~BTNonLeafNode(){
	delete[] buffer;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return pf.read(pid, buffer); }
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return pf.write(pid, buffer); }

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 	
	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	for(int i = 0; i < MAX_NUM_ENTRY; i++){
		if(ep->key == 0)
			return i;
		ep++;
	}  
	return MAX_NUM_ENTRY; 
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ 	
	int key_cnt = getKeyCount();
	if(key_cnt >= MAX_NUM_ENTRY)
		return RC_NODE_FULL;

	int eid = 0;
	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	for(int i = 0; i < key_cnt; i++){
		if(ep->key > key)
			break;
		ep++; eid++;
	}

	memmove(ep+1, ep, (key_cnt - eid) * sizeof(NonLeafNodeEntry));

	ep->key = key;
	ep->pid = pid;

	return 0;
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, 
									BTNonLeafNode& sibling, int& midKey)
{
	int key_cnt = getKeyCount();
	int midkey_eid = (key_cnt-1)/2;
	int insert_current = 1;
	NonLeafNodeEntry* first_ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	NonLeafNodeEntry* ep = first_ep + midkey_eid;
	NonLeafNodeEntry* last_ep = first_ep + key_cnt;
	//PageId* first_pid = (PageId*) buffer;

	if(key > ep[1].key)
	{	midkey_eid++; ep++; insert_current=0;	}

	midKey = ep->key;
	sibling.initializeRoot(ep->pid, ep[1].key, ep[1].pid);
	int i;
	for(i = midkey_eid+2; i < key_cnt; i++){
		sibling.insert(first_ep[i].key, first_ep[i].pid);
	}
	memset(ep, 0, (key_cnt - midkey_eid-1) * sizeof(NonLeafNodeEntry));

	if(insert_current == 1)
		insert(key, pid);
	else 
		sibling.insert(key,pid);

	return 0; 
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ 
	PageId* p = (PageId*) buffer;
	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	int key_count = getKeyCount();
	for(int i = 0; i < key_count; i++){
		if(ep[i].key > searchKey){
			pid = (i==0) ? *p : ep[i-1].pid;
			return 0;
		}
	}
	pid = ep[key_count-1].pid;
	return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
	memset(buffer, 0, PageFile::PAGE_SIZE);
	PageId* first = (PageId*) buffer;
	*first = pid1;

	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	ep->pid = pid2;
	ep->key = key;

	return 0;
}

RC BTNonLeafNode::readEntry(int eid, int& key, PageId& pid)
{
	if(eid >= getKeyCount())
		return RC_INVALID_EID;
	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId)) + eid;
	key = ep->key;
	pid = ep->pid;
	return 0;
}

PageId BTNonLeafNode::getFirstChildPtr()
{
	PageId* p = (PageId*) buffer;
	return *p;
}

void BTNonLeafNode::print()
{
	int key_cnt = getKeyCount();
	printf("Key Count = %d\n", key_cnt);

	PageId *p = (PageId*) buffer;
	printf("First pid: %d\n", *p);

	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));
	for(int i = 0; i < key_cnt; i++){
		printf("%d| key: %d | pid: %d\n", 
				i, ep->key, ep->pid);
		ep++;
	}
}

void BTNonLeafNode::rec_print(string space, PageId pid){
	int key_cnt = getKeyCount();
	printf("%s--Non Leaf---\n", space.c_str());
	printf("%s Node Pid: %d\n", space.c_str(), pid);
	printf("%sKey Count = %d\n", space.c_str(), key_cnt);

	PageId *p = (PageId*) buffer;
	printf("%s| First pid: %d\n", space.c_str(), *p);

	NonLeafNodeEntry* ep = (NonLeafNodeEntry*) (buffer+sizeof(PageId));

	for(int i = 0; i < key_cnt; i++){
		printf("%s%d. key: %d | pid: %d\n", space.c_str(), i, ep[i].key, ep[i].pid);
	}
	printf("%s--------------\n\n", space.c_str());
}

