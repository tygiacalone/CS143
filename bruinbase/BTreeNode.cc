#include "BTreeNode.h"
#include "RecordFile.h"
#include <iostream>
using namespace std;

BTLeafNode::BTLeafNode()
: maxNumKeys((PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(nEntry)))
{
    memset(buffer, 0, PageFile::PAGE_SIZE);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{
    if(!pf.read(pid, buffer))
        return RC_FILE_READ_FAILED;
    return 0;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{
    if(!pf.write(pid, buffer))
        return RC_FILE_WRITE_FAILED;
    return 0;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
    int total = 0;
    nEntry *ne = (nEntry *) buffer;
    while((total < maxNumKeys) &&( ne->rid.pid != 0))
    {
        total++;
        ne++;
    }
    return total;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid) //Chloe
{
    int totalKeys = getKeyCount();
    if(totalKeys >= maxNumKeys)
        return RC_NODE_FULL;
    
    nEntry* lastEntry = (nEntry *)buffer + totalKeys;
    nEntry* newEntry = new nEntry;
    
    int eid = 0;
    
    //If doesn't find the key:
    if(locate(key, eid))
    {
        newEntry = lastEntry;
    }
    else
    {
        newEntry = (nEntry*)buffer + eid;
        
        //Move previous entries to the right to fit in new entry
        while(lastEntry != newEntry)
        {
            nEntry* nextEntry = lastEntry - 1;
            *lastEntry = *nextEntry;
            lastEntry = nextEntry;
        }
    }
    
    newEntry->key = key;


    //NOTE: NOT SURE ABOUT MEM ALLOCATION FOR RID HERE?!
    RecordId * riid = new RecordId;
    *riid = rid;

    newEntry->rid = *riid;
    
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
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, BTLeafNode& sibling, int& siblingKey) //Ty
{   //TY: When you are inserting rid into nEntry, be careful of the memory allocation. I made a new rid but I'm not sure if I did it properly/ how to test it so lmk what you think

    cout << "in func" << endl;
    cout << "key count: " << getKeyCount() << endl;
    cout << "max keys: " << maxNumKeys << endl;
    // sibling MUST be empty
    if (sibling.getKeyCount() != 0) {
        return RC_INVALID_ATTRIBUTE;
    }

    // Not time to split yet
    if (getKeyCount() < maxNumKeys) {
        return RC_FILE_WRITE_FAILED;
    }

    // Create entry to insert
    nEntry * e = new nEntry;

    e->rid.pid = rid.pid;
    e->rid.sid = rid.sid;
    e->key = key;

    int keyCount = getKeyCount();
    int half = maxNumKeys/2;

    int offset = half * sizeof(nEntry);
    int max = maxNumKeys * sizeof(nEntry);
    int incr = sizeof(nEntry);
    bool firstLoop = true;

    // Copy right half of node to sibling
    while(offset < max) {

        nEntry * siblingNode = new nEntry;
        memcpy(siblingNode, buffer+offset, sizeof(nEntry));

        sibling.insert(siblingNode->key, siblingNode->rid);

        if(firstLoop) {
            siblingKey = siblingNode->key;
            firstLoop = false;
        }

        offset += incr;
    }

    // Reset right half to 0
    offset = half * sizeof(nEntry);
    memset(buffer+offset, 0, PageFile::PAGE_SIZE - offset);

    // If key is less than the leftMost sibling key, insert into original node
    if(key < siblingKey)
        insert(key, rid);
    else
        sibling.insert(key, rid);

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
RC BTLeafNode::locate(int searchKey, int& eid) //Chloe
{
    int totalKeys = getKeyCount();
    nEntry * ne = (nEntry*) buffer;
    
    for(eid=0; eid < totalKeys; eid++)
    {
        if(ne->key >= searchKey)
            break;
        else
            ne++;
    }
    if(eid >= totalKeys)
    {
        return RC_NO_SUCH_RECORD;
    }
    return 0;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid) //Ty
{
    // If you can't find the file to read, return err
    int maxEntries = getKeyCount();
    int offset = sizeof(nEntry) * eid;
    nEntry read;

    /*
    cout << "maxEntries: " << maxEntries << endl;
    cout << "eid: " << eid << endl;
    cout << "offset: " << offset << endl;
    */

    // Print out all the key, rid pairs for testing
    int count = 0;
    while (count < maxEntries) {
        nEntry tmp;
        memcpy(&tmp, buffer + count * sizeof(nEntry), sizeof(nEntry));

        cout << "key: " << tmp.key << endl;
        cout << "rid: sid: " << tmp.rid.pid << " pid: " << tmp.rid.sid << endl;

        count++;
    }

    if (eid > maxEntries || eid < 0) return RC_INVALID_CURSOR;

    memcpy(&read, buffer + offset, sizeof(nEntry));

    RecordId * read_rid = new RecordId;
    read_rid = &read.rid;

    //NOTE: NOT SURE ABOUT MEM ALLOCATION FOR RID HERE?!
    RecordId * riid = new RecordId;
    riid = &read.rid;

    key = read.key;
    rid.pid = read_rid->pid;
    rid.sid = read_rid->sid;

    //cout << "read: " << read.key << " saved: " << key << endl;

    return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr() //Chloe
{
    PageId* pid = (PageId *)(buffer + PageFile::PAGE_SIZE) - 1;
    return *pid;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid) //Chloe
{
    PageId* next = (PageId *) (buffer + PageFile::PAGE_SIZE) - 1;
    *next = pid;
    return 0;
}

BTNonLeafNode::BTNonLeafNode()
: maxNumKeys((PageFile::PAGE_SIZE - sizeof(PageId)) / (sizeof(nEntry)))
{
    memset(buffer, 0, PageFile::PAGE_SIZE);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf) //Ty
{
    if(!pf.read(pid, buffer))
        return RC_FILE_READ_FAILED;
    return 0;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf) //Chloe
{
    if(!pf.write(pid, buffer))
        return RC_FILE_WRITE_FAILED;
    return 0;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount() //Ty
{
    int total = 0;
    nEntry *ne = (nEntry *) buffer;
    while((total < maxNumKeys) &&( ne->pid != 0))
    {
        total++;
        ne++;
    }
    return total;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid) //Chloe
{
    int totalKeys = getKeyCount();
    if(totalKeys >= maxNumKeys)
        return RC_NODE_FULL;
    
    nEntry* lastEntry = (nEntry *)buffer + totalKeys;
    nEntry* newEntry = new nEntry;
    
    int eid = 0;
    
    //If doesn't find the key:
    if(locateChildPtr(key, eid))
    {
        newEntry = lastEntry;
    }
    else
    {
        newEntry = (nEntry*)buffer + eid;
        
        //Move previous entries to the right to fit in new entry
        while(lastEntry != newEntry)
        {
            nEntry* nextEntry = lastEntry - 1;
            *lastEntry = *nextEntry;
            lastEntry = nextEntry;
        }
    }
    
    newEntry->key = key;

    //NOTE: NOT SURE ABOUT MEM ALLOCATION FOR PID HERE?!
    PageId * piid = new PageId;
    *piid = pid;

    newEntry->pid = *piid;

    cout << "\nInserted: " << endl;
    int count1 = 0;
    while (count1 < maxNumKeys) {
        nEntry tmp;
        memcpy(&tmp, buffer + count1 * sizeof(nEntry), sizeof(nEntry));

        if(tmp.key == 0)
            break;
        cout << "key: " << tmp.key << endl;
        cout << "pid: " << tmp.pid << endl;
        cout << endl;

        count1++;
    }


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
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey) //Ty
{
    cout << "in func" << endl;
    cout << "key count: " << getKeyCount() << endl;
    cout << "max keys: " << maxNumKeys << endl;
    // sibling MUST be empty
    if (sibling.getKeyCount() != 0) {
        return RC_INVALID_ATTRIBUTE;
    }

    // Not time to split yet
    if (getKeyCount() < maxNumKeys) {
        return RC_FILE_WRITE_FAILED;
    }


    cout << "\nBefore: " << endl;
    int count1 = 0;
    while (count1 < maxNumKeys) {
        nEntry tmp;
        memcpy(&tmp,  sizeof(PageId)+buffer + count1 * sizeof(nEntry), sizeof(nEntry));

        cout << "key: " << tmp.key << endl;
        cout << "pid: " << tmp.pid << endl;

        count1++;
    }

    // Create entry to insert
    nEntry * e = new nEntry;

    e->pid = pid;
    e->key = key;

    int keyCount = getKeyCount();
    int half = maxNumKeys/2;
    ;
    int offset = half * sizeof(nEntry);
    int max = maxNumKeys * sizeof(nEntry);
    int incr = sizeof(nEntry);
    bool firstLoop = true;

    // Copy right half of node to sibling
    while(offset < max) {

        nEntry * siblingNode = new nEntry;
        memcpy(siblingNode, sizeof(PageId)+buffer+offset, sizeof(nEntry));

        //cout << "key: " << siblingNode->key << endl;
        if(firstLoop) {
            midKey = siblingNode->key;
            firstLoop = false;
        }

        sibling.insert(siblingNode->key, siblingNode->pid);

        offset += incr;
    }

    // Reset right half to 0
    offset = half * sizeof(nEntry);
    memset(buffer+offset, 0, PageFile::PAGE_SIZE - offset);

    // If key is less than the leftMost sibling key, insert into original node
    if(key < midKey)
        insert(key, pid);
    else
        sibling.insert(key, pid);


    int count = 0;
    cout << "\nAfter: " << endl;\
    while (count < maxNumKeys) {
        nEntry tmp;
        memcpy(&tmp, sizeof(PageId)+ buffer + count * sizeof(nEntry), sizeof(nEntry));

        cout << "key: " << tmp.key << endl;
        cout << "pid: " << tmp.pid << endl;

        count++;
    }

    count = 0;
    cout << "\nSibling: " << endl;
    while (count < maxNumKeys) {
        nEntry tmp;
        memcpy(&tmp, sizeof(PageId)+sibling.buffer + count * sizeof(nEntry), sizeof(nEntry));

        cout << "key: " << tmp.key << endl;
        cout << "pid: " << tmp.pid << endl;

        count++;
    }

    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid) //Chloe
{
    pid = getKeyCount() - 1;
    nEntry* ne = (nEntry*)buffer + pid;

    for(; pid >= 0; pid--)
    {
        if(ne->key <= searchKey)
            break;
        else
            ne--;
    }
    
    if(pid < 0)
        return RC_NO_SUCH_RECORD;
    
    return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2) //Ty
{
    int pid_size = sizeof(PageId);

    // Reset buffer to 0 for initialization
    memset(buffer, 0, PageFile::PAGE_SIZE);

    // Set first pid
    memset(buffer, pid1, pid_size);

    // Insert key, pid2 pair check for error.
    if (insert(key, pid2))
        return RC_FILE_WRITE_FAILED;

    return 0;
}
