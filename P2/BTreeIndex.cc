/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1; treeHeight = 0;
}

BTreeIndex::BTreeIndex(const string& indexname, char mode)
{
    treeHeight = 0;
    pf = PageFile(indexname, mode);

    char tmp_buffer[PageFile::PAGE_SIZE];
    bzero(tmp_buffer, PageFile::PAGE_SIZE);
    int* ptr = (int*)tmp_buffer;

    if(mode == 'r'){
	    pf.read(0, tmp_buffer);
	    rootPid = *ptr;
	    treeHeight = ptr[1];
	}
	else{
		if(pf.endPid() == 0){
			rootPid = 1;
			*ptr = rootPid;
			*(ptr+1) = treeHeight;
			pf.write(0, tmp_buffer);

			BTLeafNode* root = new BTLeafNode();
    		root->setNextNodePtr(-1);
    		root->write(rootPid, pf);
		}
		else{
			pf.read(0, tmp_buffer);
	    	rootPid = *ptr;
	    	treeHeight = ptr[1];
		}
	}
}

BTreeIndex::~BTreeIndex()
{
	close();
}

void BTreeIndex::save_tree_info(){
	char tmp_buffer[PageFile::PAGE_SIZE];
    bzero(tmp_buffer, PageFile::PAGE_SIZE);
    int* ptr = (int*)tmp_buffer;

    *ptr = rootPid;
	*(ptr+1) = treeHeight;
	pf.write(0, tmp_buffer);
}


/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{	
	return pf.open(indexname, mode); 
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{	return pf.close();}

RC BTreeIndex::insert_rec(int key, const RecordId& rid, 
							PageId node_pid, int level, 
							PageId& sibling_pid, int& sibling_key)
{
	int rc;
	sibling_pid = -1; sibling_key = -1;

	// If currently at leaf level:
	if (treeHeight == level){
		BTLeafNode* leaf = new BTLeafNode();
		if((rc = leaf->read(node_pid, pf)) < 0) return rc;

		// If the leaf is full, split into two nodes: 
		if((rc = leaf->insert(key, rid)) == RC_NODE_FULL){
			BTLeafNode* sibling = new BTLeafNode();

			//printf("Before insert & split....\n"); print();
			if((rc = leaf->insertAndSplit(key, rid, *sibling, sibling_key)) < 0)
				return rc;

			sibling_pid = pf.endPid();
			PageId tmp_pid = leaf->getNextNodePtr();
			leaf->setNextNodePtr(sibling_pid);
			sibling->setNextNodePtr(tmp_pid);
			if((rc = leaf->write(node_pid, pf)) < 0) return rc;

			// write the new node to disk:
			if((rc = sibling->write(sibling_pid, pf)) < 0) return rc;

			// If at root level, create new root
			if(level == 0){
				BTNonLeafNode* new_root = new BTNonLeafNode();
				if((rc = new_root->initializeRoot(node_pid, sibling_key, 
										sibling_pid)) < 0) return rc;
				PageId new_root_pid = pf.endPid();
				if((rc = new_root->write(new_root_pid, pf)) < 0) return rc;
				rootPid = new_root_pid;
				treeHeight++;
				save_tree_info();

			}
			//printf("After insert & split...., sibling key = %d\n", sibling_key); print();
			
		}
		if((rc = leaf->write(node_pid, pf)) < 0) return rc;
		
	}

	// If currently at non-leaf level:
	else {
		BTNonLeafNode* n = new BTNonLeafNode();
		if((rc = n->read(node_pid, pf)) < 0) return rc;

		PageId child_pid;
		if((rc = n->locateChildPtr(key, child_pid)) < 0) return rc;

		PageId child_sibling_pid;
		int child_sibling_key;
		if((rc = insert_rec(key, rid, child_pid, level+1, 
						child_sibling_pid, child_sibling_key)) < 0) 
			return rc;

		// if overflow occurs at child:
		if(child_sibling_key > 0){
			if((rc = n->insert(child_sibling_key, child_sibling_pid)) == RC_NODE_FULL){
				BTNonLeafNode* sibling = new BTNonLeafNode();

				if((rc = n->insertAndSplit(child_sibling_key, child_sibling_pid, 
											*sibling, sibling_key)) < 0)
					return rc;
				sibling_pid = pf.endPid();

				if((rc = n->write(node_pid, pf)) < 0) return rc;
				if((rc = sibling->write(sibling_pid, pf)) < 0) return rc;

				// If at root level, create new root
				if(level == 0){
					BTNonLeafNode* new_root = new BTNonLeafNode();
					if((rc = new_root->initializeRoot(node_pid, sibling_key, 
											sibling_pid)) < 0) return rc;
					PageId new_root_pid = pf.endPid();
					if((rc = new_root->write(new_root_pid, pf)) < 0) return rc;
					rootPid = new_root_pid;
					treeHeight++;
					save_tree_info();
				}
			}
			if((rc = n->write(node_pid, pf)) < 0) return rc;
			//printf("After insert & split...., child sibling key = %d\n", child_sibling_key); print();
		}

		
		//printf("After insert & split...., sibling key = %d\n", sibling_key); print();
	}
	return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	PageId p;
	int i;
	return insert_rec(key, rid, rootPid, 0, p, i);
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	return locate_rec(searchKey, cursor, 0, rootPid);
}

RC BTreeIndex::locate_rec(int searchKey, IndexCursor& cursor, 
							int level, PageId pid)
{
	int rc;
	if(level == treeHeight){
		cursor.pid = pid;
		BTLeafNode* leaf = new BTLeafNode();
		if((rc = leaf->read(pid, pf)) < 0) return rc;
		if((rc = leaf->locate(searchKey, cursor.eid)) < 0) return rc;
	} else {
		BTNonLeafNode* n = new BTNonLeafNode();
		if((rc = n->read(pid, pf)) < 0) return rc;

		PageId child_pid;
		if((rc = n->locateChildPtr(searchKey, child_pid)) < 0) return rc;
		return locate_rec(searchKey, cursor, level+1, child_pid);
	}
	return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	int rc;
	if(cursor.pid < 0 || cursor.pid >= pf.endPid())
		return RC_INVALID_CURSOR;

	BTLeafNode* n = new BTLeafNode();
	if((rc = n->read(cursor.pid, pf)) < 0) return rc;

	if((rc = n->readEntry(cursor.eid, key, rid)) < 0) return rc;

	cursor.eid++;
	if(cursor.eid >= n->getKeyCount()){
		cursor.pid = n->getNextNodePtr();
		cursor.eid = 0;
	}
    return 0;
}

void BTreeIndex::print(){
	print_rec(rootPid, 0, string(""));
}

void BTreeIndex::print_rec(PageId pid, int level, string space){
	if(level == treeHeight){
		BTLeafNode* n = new BTLeafNode();
		n->read(pid, pf);
		n->rec_print(space, pid);
	} else {
		BTNonLeafNode* n = new BTNonLeafNode();
		n->read(pid, pf);
		n->rec_print(space, pid);
		int key_cnt = n->getKeyCount();
		space.append("			");

		print_rec(n->getFirstChildPtr(), level+1, space);
		PageId child_pid;
		for(int eid=0; eid<key_cnt; eid++){
			int tmp;
			n->readEntry(eid, tmp, child_pid);
			print_rec(child_pid, level+1, space);
		}
	}
}
