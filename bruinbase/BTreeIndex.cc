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
#include "RecordFile.h"
#include <iostream>

using namespace std;


struct nEntry
{
    int key;
    RecordId rid;

    nEntry()
    {
        rid.pid = 0;
        rid.sid = 0;
    }

};

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


RC BTreeIndex::insertAtLevel(int key, const RecordId& rid, PageId currPid, int height, int& parentKey, PageId& parentPid) //Ty
{

    if (treeHeight == height) {// If we're at the leaf level
        BTLeafNode node;
        RC insert_err = 0;
        RC write_err = 0;

        node.read(currPid, pf);
        insert_err = node.insert(key, rid);

        if (insert_err) { // If node is full
            BTLeafNode sibling;
            write_err = 0;
            write_err = node.insertAndSplit(key, rid, sibling, parentKey);

            if (!write_err) {
                sibling.setNextNodePtr(node.getNextNodePtr());
                parentPid = pf.endPid();
                node.setNextNodePtr(parentPid);

                write_err = sibling.write(parentPid, pf);

                if (write_err) {
                    return write_err;
                }
            }
            else {
                return write_err;
            }
        }
        write_err = 0;
        write_err = node.write(currPid, pf);

        if (write_err) {
            return write_err;
        }
    }
    else { // Not at leaf yet
        BTNonLeafNode node;
        PageId next;

        node.read(currPid, pf);

        int eid = 0;
        int maxEntries = node.getKeyCount();

        while (eid < maxEntries) { // Find correct eid that corresponds to nEntry with key <= key we want to insert
            nEntry tmp;
            memcpy(&tmp, node.getBuffer() + eid * sizeof(nEntry), sizeof(nEntry));

            if (tmp.key > key)
                break;

            eid++;
        }

        node.readEntry(eid, next); // Read in next node to try to insert into


        insertAtLevel(key, rid, next, height + 1, parentKey, parentPid);

        if (parentKey > 0) { // If we split at child's level, insert into parent
            RC insert_err = 0;
            insert_err = node.insert(parentKey, parentPid);

            if (insert_err) { // If we couldn't insert into the parent
                cout << "Increasing tree height!" << endl;
                BTNonLeafNode sibling;
                int newParentKey = -1;
                insert_err = 0;
                insert_err = node.insertAndSplit(parentKey, parentPid, sibling, newParentKey);

                if (!insert_err) {
                    parentKey = newParentKey;
                    parentPid = pf.endPid();
                    RC write_err = 0;
                    write_err = sibling.write(parentPid, pf);

                    if (write_err) {
                        return write_err;
                    }
                }
                else {
                    return insert_err;
                }

            }
            else { // If we inserted into parent without issue, we no longer need to insert parentKey value so reset it
                parentKey = -1;
            }
        }

        node.write(currPid, pf); // Write changes
    }

    return 0;
}


/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */

RC BTreeIndex::insert(int key, const RecordId& rid) //Ty
{

    if (treeHeight == 0) { // If just created tree
        BTLeafNode root;
        RC insert_err = 0;
        RC write_err = 0;
        insert_err = root.insert(key, rid);

        if (insert_err) {
            return -2;
        }
        else {
            rootPid = pf.endPid();
            write_err = root.write(rootPid, pf);
            treeHeight++;

            if (write_err) {
                //cout << "write err" << endl;
                return -3;
            }
            else {
                return 0;
            }
        }
    }
    else {

        int insKey = -1;
        PageId insPid;
        RC insert_err = 0;
        insert_err = insertAtLevel(key, rid, rootPid, 1, insKey, insPid);

        if (insert_err) {
            cout << insert_err << endl;
            return 1;
        }

        if (insKey > 0) {
            BTNonLeafNode node;
            RC init_err = 0;
            init_err = node.initializeRoot(rootPid, insKey, insPid);

            if (init_err) {
                return RC_FILE_WRITE_FAILED;
            }
            else {
                treeHeight++;
                cout << "Tree height is: " << treeHeight << endl;
                rootPid = pf.endPid();
                node.write(rootPid, pf);
            }

        }

    }

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
    
    if(treeHeight > 0)
    {
        for(int i=treeHeight; i > 0; i--)
        {
            RC ret;
            ret = nonleaf.read(pid, pf);
            if(ret < 0)
                return ret;//RC_NO_SUCH_RECORD;
            cout << "Looking for pid: " << pid << endl;

            if(nonleaf.locateChildPtr(searchKey, pid) < 0)
                return -1; //not sure which error message to send here!

        }
    }
    
    if(leaf.read(pid,pf)<0)
    {
        return  -3333;//RC_NO_SUCH_RECORD;
    }
    else
    {
        eid = 0;
        int maxEntries = leaf.getKeyCount();

        while (eid < maxEntries) { // Find correct eid that corresponds to nEntry with key <= key we want to insert
            nEntry tmp;
            memcpy(&tmp, leaf.getBuffer() + eid * sizeof(nEntry), sizeof(nEntry));

            if (tmp.key > searchKey)
                break;

            eid++;
        }

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

/* call this function from the test file. since we shouldn't have
 * direct access to rootPid, this function calls another function
 * that does have access to rootPid */
int BTreeIndex::print_tree()
{
    return print_tree_helper(rootPid);
}

// this is a modified locate()
// we had to have a helper and print_tree because our test
// program doesn't have access to rootPid
int BTreeIndex::print_tree_helper(PageId pid)
{
    int enteries = 0;
    RC rc = 0;
    // read the buffer of pid
    char buf[PageFile::PAGE_SIZE];
    rc = pf.read(pid, buf);

    // get the type of the pid we're currently at
    // 0 = non-leaf node, 1 = leaf node
    int type = 0;
    if(buf[0] == '0')
    {
        type = 0;
    }
    else
    {
        type = 1;
    }
    //memcpy(&type, buf, sizeof(int));

    if(type == 0)   // non-leaf node, so recurse
    {
        //RC rc = 0;

        // get the node at pid
        //char buf[PageFile::PAGE_SIZE];
        //rc = pf.read(pid, buf);
        int size = 0;
        memcpy(&size, buf+4, sizeof(int));

        char next[PageFile::PAGE_SIZE];
        int next_index = 0;

        cout << endl;
        cout << "Non Leaf Node " << endl;
        cout << "pid: \t" << pid << endl;
        cout << "count: \t" << size << endl;
        cout << endl;

        int par = 0;
        memcpy(&par, buf+8, sizeof(int));
        cout << "parent: " << par << endl;

        for(int i = 0; i < 2*size+1; i++)
        {
            int elem = 0;
            memcpy(&elem, buf+16+(4*i), sizeof(int));
            if(i%2 == 0)
            {
                next[next_index] = elem;
                next_index++;
            }
            if(i%2 == 0)
            {
                if(i == 2*size )
                {
                    cout << " PageId :  " << elem;
                }
                else
                {
                    cout << " PageId :  " << elem << " | ";
                }
            }
            else
            {
                cout <<" Key : "<< elem << " | ";
            }
        }
        cout << endl <<endl;

        for(int i = 0; i < next_index; i++)
        {
            enteries =  enteries + print_tree_helper(next[i]);
        }
    }

    else            // leaf node, so find the record and return
    {
        //RC rc = 0;

        // get the node at pid
        //char buf[PageFile::PAGE_SIZE];
        //rc = pf.read(pid, buf);
        int size = 0;
        memcpy(&size, buf+4, sizeof(int));

        cout << endl;
        cout << " Type Leaf Node "<<endl;
        cout << "pid: \t" << pid << endl;
        cout << "count: \t" << size << endl;
        enteries = size;
        cout << endl;

        int par = 0;
        memcpy(&par, buf+8, sizeof(int));
        cout << "parent: " << par << endl;

        //int which = 0;

        for(int i = 0; i < size; i++)
        {
            int elem = 0;
            RecordId rid;

            memcpy(&rid, buf+16+(12*i), 8);
            memcpy(&elem, buf+16+(12*i) + 8 , sizeof(int));
            if( i >= size - 1)
            {
                cout << " Record PageId : " << rid.pid << " | ";
                cout << " Key value : " << elem;
            }
            else
            {
                cout << " Record PageId : " << rid.pid << " | ";
                cout << " Key value : " << elem << " | ";
            }
        }
        int nextpage;
        memcpy(&nextpage, buf+1020, sizeof(int));
        cout << endl;
        cout << " Next Pageid Id: " << nextpage ;

        cout << endl;
        cout << endl;
    }
    return enteries;
}

void BTreeIndex::print_nodes()
{
    cout<<"number of nodes : " << pf.endPid() - 1 <<endl;

    char     page[PageFile::PAGE_SIZE];
    char     nodeType;
    int     entryCount;
    PageId     parentPtr, nextNodePtr;

    for(int i = 1; i < pf.endPid() ; i++)
    {
        pf.read(i, page);
        //memcpy(&nodeType, page, sizeof(char));
        memcpy(&entryCount, page+4, sizeof(int));
        memcpy(&parentPtr, page+8, sizeof(int));

        nodeType = page[0];
        //nodeType = nodeType + '0';

        if(nodeType == '0')
        {
            cout<<"pid: " << i << "\t node type: nonLeaf \t parent: "     << parentPtr << "\t entryCount: " << entryCount << endl;
        }
        else
        {
            memcpy(&nextNodePtr, page+1020, sizeof(int));
            cout<<"pid: " << i << "\t node type: leaf     \t parent: "     << parentPtr << "\t entryCount: " << entryCount
            <<"\t nextNodePtr: " << nextNodePtr << endl;
        }
    }
}

PageFile BTreeIndex::getpf()
{
    return pf;
}
