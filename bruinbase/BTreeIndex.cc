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
    rootPid = -1;
    treeHeight = 0;
    
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */

RC BTreeIndex::open(const string& indexname, char mode) //Ty
{

    RC ret = pf.open(indexname, mode);
    if(ret != 0) {
        return ret;
    }

    char storedData[PageFile::PAGE_SIZE];
    
    //If the file is empty:
    if(pf.endPid() == 0)
    {
        rootPid = -1;
        treeHeight = 0;

        ret = pf.write(0, storedData);
        if(ret != 0) {
            return ret;
        }
    }
    //If file is not empty:
    else
    {

        ret = pf.read(0, storedData);
        if(ret != 0) {
            return ret;
        }
        //Read in previously stored data of treeHeight and rootPid
        rootPid = *((PageId *) storedData);
        treeHeight = *((int *) (storedData + sizeof(PageId)));
    }
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */

RC BTreeIndex::close() //Chloe
{
    //Store extra variables to disk
    char storedData[PageFile::PAGE_SIZE];
    *((PageId *) storedData) = rootPid;
    *((int *) (storedData + sizeof(PageId))) = treeHeight;
    
    if(!pf.write(0, storedData)) {
        return RC_FILE_WRITE_FAILED;
    }
    
    return pf.close();
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */

RC BTreeIndex::insert(int key, const RecordId& rid) //Ty
{

    return 0;
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

RC BTreeIndex::locate(int searchKey, IndexCursor& cursor) //Chloe

{
    PageId pid = rootPid;
    int eid = -1;
    
    BTLeafNode leaf = BTLeafNode();
    BTNonLeafNode nonleaf = BTNonLeafNode();
    
    if(treeHeight > 1)
    {
        for(int i=0; i<treeHeight - 1; i++)
        {
            if(nonleaf.read(pid, pf) < 0)
                return RC_NO_SUCH_RECORD;
            
            if(nonleaf.locateChildPtr(searchKey, pid) < 0)
                return -1; //not sure which error message to send here!
        }
    }
    
    if(leaf.read(pid,pf)<0)
    {
        return RC_NO_SUCH_RECORD;
    }
    else
    {
        cursor.pid = pid;
        cursor.eid = eid;
    }

    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move forward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */

RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid) //Ty
{
    BTLeafNode node;
    RC read_err;
    RC readEntry_err;
    PageId pid = cursor.pid;
    int eid = cursor.eid;

    read_err = node.read(pid, pf);
    if (read_err) { // If we can't read in the specified node
        return read_err;
    }

    readEntry_err = node.readEntry(eid, key, rid);
    if (!readEntry_err) { // If we find next entry exists in the current node
        eid++;

        if (eid >= node.getKeyCount()) { // If we moved eid past the end
            eid = 0;
            pid = node.getNextNodePtr();
        }
    }
    else {
        // Set eid, pid to beginning of next node
        eid = 0;
        pid = node.getNextNodePtr();
    }

    cursor.eid = eid;
    cursor.pid = pid;

    return 0;
}
