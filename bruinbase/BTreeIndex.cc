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


struct nEntryNonLeaf
{
    int key;
    PageId pid;

    nEntryNonLeaf()
    {
        pid = 0;
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

int global_root = 0;
int split_count = 0;
RC BTreeIndex::insertAtLevel(int key, const RecordId& rid, PageId currPid, int height, int& parentKey, PageId& parentPid) //Ty
{
    //cout<< "$$$$$$$$$$$$$$$$$$ ENTER INSERTATLEVEL $$$$$$$$$$$$$$$$$$$$$$" << endl;
    //cout<< "treeHeight: " << treeHeight << " == " << height << ": height" << endl;
    if (treeHeight == height) {// If we're at the leaf level
        BTLeafNode node;
        RC insert_err = 0;
        RC write_err = 0;
        //cout<< "Key I'm inserting: " << key << endl;
        //cout<< "Pid I'm inserting into: " << currPid << endl;
        //cout<< "rid.pid I'm inserting into: " << rid.pid << endl;
        node.read(currPid, pf);
        insert_err = node.insert(key, rid);
        //node.setNextNodePtr(pf.endPid());

        if (insert_err) { // If node is full
            split_count++;
            //cout<< "##########INSERT ERRRERERER ######################################## I split " << split_count <<  endl;
            BTLeafNode sibling;
            write_err = 0;
            write_err = node.insertAndSplit(key, rid, sibling, parentKey);

            if (!write_err) {
                parentPid = pf.endPid();
                //cout << "sibling getNextNodePtr() should be: " << node.getNextNodePtr() << endl;
                sibling.setNextNodePtr(node.getNextNodePtr());
                //cout << "sibling getNextNodePtr() actually is: " << sibling.getNextNodePtr() << endl;
                //cout << "getNextNodePtr() should be: " << parentPid << endl;
                node.setNextNodePtr(parentPid);
                //cout << "getNextNodePtr() actually is: " << node.getNextNodePtr() << endl;

                //cout<< "Writing to pid parentPid 163: " << parentPid << endl;
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
        //cout<< "Writing to pid currPid 172: " << currPid << endl;
        write_err = node.write(currPid, pf);
        ////cout<< "Value of currPid 176 is: " << currPid << endl;

        if (write_err) {
            return write_err;
        }

        /*
        int maxNumKeys1 = node.getKeyCount();
        //cout<< "\n Value after insert: " << endl;
        int count1 = 0;
        while (count1 < maxNumKeys1) {
            nEntry tmp;
            memcpy(&tmp, node.getBuffer() + count1 * sizeof(nEntry), sizeof(nEntry));

            if(tmp.key == 0)
                break;
            //cout<< "key: " << tmp.key << endl;
            //cout<< "pid: " << tmp.rid.pid << endl;
            //cout<< "sid: " << tmp.rid.sid << endl;
            //cout<< endl;

            count1++;
        }
        */
        //cout<< "Value of global_root: " << global_root << endl;
        //cout<< "Value in root node BEFORE WRITE: " << endl;
        BTNonLeafNode rootnode;
        rootnode.read(global_root, pf);
        int maxNumKeys = rootnode.getKeyCount();
        int count2 = 0;
        while (count2 < maxNumKeys) {
            nEntryNonLeaf tmp;
            memcpy(&tmp, rootnode.getBuffer() + count2 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

            if(tmp.key == 0)
                break;
            //cout<< "key: " << tmp.key << endl;
            //cout<< "pid: " << tmp.pid << endl;
            //cout<< endl;

            count2++;
        }
    }
    else { // Not at leaf yet
        BTNonLeafNode currNode;
        BTNonLeafNode nextNode;
        PageId next = 0;
        //cout<< "CurrPid to read is: " << currPid << endl;
        currNode.read(currPid, pf);

        int eid = 0;
        int maxNumKeys = currNode.getKeyCount();
/*
        while (eid < maxEntries) { // Find correct eid that corresponds to nEntry with key <= key we want to insert
            nEntry tmp;
            memcpy(&tmp, node.getBuffer() + eid * sizeof(nEntry), sizeof(nEntry));

            if (tmp.key > key)
                break;

            eid++;
        }
        */

        //cout<< "Value in root node: " << endl;
        BTNonLeafNode rootnode;
        rootnode.read(global_root, pf);
        int count2 = 0;
        while (count2 < maxNumKeys) {
            nEntryNonLeaf tmp;
            memcpy(&tmp, rootnode.getBuffer() + count2 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

            if(tmp.key == 0)
                break;
            //cout<< "key: " << tmp.key << endl;
            //cout<< "pid: " << tmp.pid << endl;
            //cout<< endl;

            count2++;
        }

        int count1 = 0;
        bool firstLoop = true;
        while (count1 < maxNumKeys) {
            nEntryNonLeaf tmp;
            memcpy(&tmp, currNode.getBuffer() + count1 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

            if (firstLoop) {
                next = tmp.pid;
                firstLoop = false;
            }

            //cout<< "pid is now: " << next << endl;
            //cout<< "key is now: " << tmp.key << endl;
            //cout<< "searchKey is now: " << key << endl;

            if (tmp.key > key) {
                ////cout<< "Breaking with value: " << pid << endl;
                break;
            }
            else {
                next = tmp.pid;
            }
            // 1 | 2, 4 | 3, 5| 4, 9
            // insert 3
            // 1
            // 2 > 3
            // 2
            // 5 > 3
            //
            count1++;

            if(tmp.key == 0) {
                break;
            }

        }
        //cout<< "insertAtLevel into pid: " << next << endl;
        //cout<< "height is: " << height + 1 << endl;
        //cout<< "treeHeight is: " << treeHeight << endl;

        nextNode.read(next, pf); // Read in next node to try to insert into

        //cout<< "parentKey before insertAtLevel 260: " << parentKey << endl;
        //cout<< "parentPid before insertAtLevel 261: " << parentPid << endl;
        insertAtLevel(key, rid, next, height + 1, parentKey, parentPid);
        //cout<< "parentKey after insertAtLevel 262: " << parentKey << endl;
        //cout<< "parentPid after insertAtLevel 263: " << parentPid << endl;

        if (parentKey > 0) { // If we split at child's level, insert into parent
            RC insert_err = 0;
            insert_err = currNode.insert(parentKey, parentPid);

            if (insert_err) { // If we couldn't insert into the parent
                //cout<< "Increasing tree height!" << endl;
                BTNonLeafNode sibling;
                int newParentKey = -1;
                insert_err = 0;
                insert_err = currNode.insertAndSplit(parentKey, parentPid, sibling, newParentKey);

                if (!insert_err) {
                    parentKey = newParentKey;
                    parentPid = pf.endPid();
                    RC write_err = 0;

                    //cout<< "Writing to pid parentPid 278: " << parentPid << endl;
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

        //cout<< "Writing to pid currPid 296: " << currPid << endl;
        currNode.write(currPid, pf); // Write changes
    }

    BTNonLeafNode rootnode;
    //cout<< "Value in root node END WRITE: " << endl;
    rootnode.read(global_root, pf);
    int maxNumKeys = rootnode.getKeyCount();
    int count2 = 0;
    while (count2 < maxNumKeys) {
        BTLeafNode tmpnode;
        nEntryNonLeaf tmp;
        memcpy(&tmp, rootnode.getBuffer() + count2 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

        if(tmp.key == 0)
            break;
        tmpnode.read(tmp.pid, pf);
        //cout<< "key: " << tmp.key << endl;
        //cout<< "pid: " << tmp.pid << endl;
        //cout << "getNextNodePtr() value: " << tmpnode.getNextNodePtr() << endl;
        //cout<< endl;

        count2++;
    }


    //cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    //cout<< "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    //cout<< "$$$$$$$$$$$$$$$$$$$ END INSERTATLEVEL $$$$$$$$$$$$$$$$$$$$$$$" << endl;


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
            //cout<< "Writing to pid rootPid 343 insert: " << rootPid << endl;
            write_err = root.write(rootPid, pf);
            treeHeight++;
            global_root = rootPid;

            if (write_err) {
                //cout<< "write err" << endl;
                return -3;
            }
            else {
                return 0;
            }
        }
    }
    else {

        int insKey = -1;
        PageId insPid = 0;
        RC insert_err = 0;
        //cout<< "\nRootPid to insert value: " << rootPid << endl;
        insert_err = insertAtLevel(key, rid, rootPid, 1, insKey, insPid);

        if (insert_err) {
            //cout<< insert_err << endl;
            return 1;
        }

        if (insKey > 0) {
            BTNonLeafNode node;
            RC init_err = 0;
            init_err = node.initializeRoot(rootPid, insKey, insPid);
            //cout<< "new root pid: " << rootPid << endl;
            //cout<< "new root key, pid pair: " << insKey << ", " << insPid << endl;
            global_root = rootPid;

            if (init_err) {
                return  RC_FILE_WRITE_FAILED;
            }
            else {
                treeHeight++;
                //cout<< "Tree height is: " << treeHeight << endl;
                rootPid = pf.endPid();
                //cout<< "Writing to pid rootPid 384 insert: " << rootPid << endl;
                node.write(rootPid, pf);

                //cout<< "NEW ROOT ADDED!" << endl;
                global_root = rootPid;
                //cout << "Global_root is now: " << global_root << endl;
                //cout<< "Value in new root node: " << endl;
                int maxNumKeys = node.getKeyCount();
                int count1 = 0;

                BTNonLeafNode rootnew;
                rootnew.read(global_root, pf);

                nEntryNonLeaf tmp;
                memcpy(&tmp, rootnew.getBuffer(), sizeof(nEntryNonLeaf));
                //cout<< "First pid: " << tmp.pid << endl;

                while (count1 < maxNumKeys) {
                    nEntryNonLeaf tmp;
                    memcpy(&tmp, rootnew.getBuffer() + count1 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

                    if(tmp.key == 0)
                        break;
                    //cout<< "key: " << tmp.key << endl;
                    //cout<< "pid: " << tmp.pid << endl;
                    //cout<< endl;

                    count1++;
                }
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
    //cout<< "treeHeight: " << treeHeight << endl;
    if(treeHeight > 0)
    {
        for(int i = treeHeight; i > 1; i--)
        {
            RC ret;
            ret = nonleaf.read(pid, pf);

            if(ret < 0)
                return ret;//RC_NO_SUCH_RECORD;

            eid = 0;
            int maxEntries = leaf.getKeyCount();

            while (eid < maxEntries) { // Find correct eid that corresponds to nEntry with key <= key we want to insert
                nEntry tmp;
                memcpy(&tmp, leaf.getBuffer() + eid * sizeof(nEntry), sizeof(nEntry));

                if (tmp.key > searchKey)
                    break;

                eid++;
            }

            //cout<< "\nCurrent pid of node: " << pid << endl;

            //nonleaf.locateChildPtr(searchKey, pid);
            int maxNumKeys = nonleaf.getKeyCount();
            int count1 = 0;
            bool firstLoop = true;
            while (count1 < maxNumKeys) {
                nEntryNonLeaf tmp;
                memcpy(&tmp, nonleaf.getBuffer() + count1 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

                if (firstLoop) {
                    pid = tmp.pid;
                    firstLoop = false;
                }

                //cout<< "pid is now: " << pid << endl;
                //cout<< "key is now: " << tmp.key << endl;
                //cout<< "searchKey is now: " << searchKey << endl;

                if (tmp.key > searchKey) {
                    ////cout<< "Breaking with value: " << pid << endl;
                    break;
                }
                else {
                    pid = tmp.pid;
                }
                // 1 | 2, 4 | 3, 5| 4, 9
                // insert 3
                // 1
                // 2 > 3
                // 2
                // 5 > 3
                //
                count1++;

                if(tmp.key == 0) {
                    break;
                }

            }
            //cout<< "Breaking with value: " << pid << endl;

            //nonleaf.readEntry(eid, pid);


            //cout<< "pid after readEntry: " << pid << endl;


            //cout<< "Value in current node: " << endl;
            count1 = 0;
            while (count1 < maxNumKeys) {
                nEntryNonLeaf tmp;
                memcpy(&tmp, nonleaf.getBuffer() + count1 * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));

                if(tmp.key == 0)
                    break;
                //cout<< "key: " << tmp.key << endl;
                //cout<< "pid: " << tmp.pid << endl;
                //cout<< endl;

                count1++;
            }


            //cout<< "pid end value: " << pid << endl;
        }
    }

    //cout<< "Pid about to read: " << pid << endl;
    RC leaf_ret = leaf.read(pid, pf);

    if(leaf_ret) // If there's no leaf node
    {
        eid = 0;
        int maxEntries = nonleaf.getKeyCount();

        while (eid < maxEntries) { // Find correct eid that corresponds to nEntry with key <= key we want to insert
            nEntryNonLeaf tmp;
            memcpy(&tmp, leaf.getBuffer() + eid * sizeof(nEntryNonLeaf), sizeof(nEntryNonLeaf));
            //cout << "tmp.key in locate: " << tmp.key << endl;
            //cout << "searchKey in locate: " << searchKey << endl;
            if (tmp.key > searchKey)
                break;

            eid++;
            //cout << "eid in locate after add: " << tmp.key << endl;
        }

        cursor.pid = pid;
        cursor.eid = eid;
        return 0;
        //return leaf_ret;//RC_NO_SUCH_RECORD;
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

        //cout<< "\nValue in current node: " << endl;
        int maxNumKeys = leaf.getKeyCount();
        int count1 = 0;
        while (count1 < maxNumKeys) {
            nEntry tmp;
            memcpy(&tmp, leaf.getBuffer() + count1 * sizeof(nEntry), sizeof(nEntry));

            if(tmp.key == 0)
                break;
            //cout<< "key: " << tmp.key << endl;
            //cout<< "pid: " << tmp.rid.pid << endl;
            //cout<< endl;

            count1++;
        }


        cursor.pid = pid;
        cursor.eid = eid;
    }

    return 0;
    
}

static char* slotPtr(char* page, int n)
{
    // compute the location of the n'th slot in a page.
    // remember that the first four bytes in a page is used to store
    // # records in the page and each slot consists of an integer and
    // a string of length MAX_VALUE_LENGTH
    return (page+sizeof(int)) + (sizeof(int)+RecordFile::MAX_VALUE_LENGTH)*n;
}

static void readSlot(const char* page, int n, int& key, std::string& value)
{
    // compute the location of the record
    char *ptr = slotPtr(const_cast<char*>(page), n);

    // read the key
    memcpy(&key, ptr, sizeof(int));

    // read the value
    value.assign(ptr + sizeof(int));
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


    //cout<< "About to read pid: " << pid << endl;
    read_err = node.read(pid, pf);
    //node.readEntry(cursor.eid, key, rid);
    if (read_err) { // If we can't read in the specified node
        //cout<< "read_err" << endl;
        return read_err;
    }


    char page[PageFile::PAGE_SIZE];
    string value = "";

    //cout<< "\nValue in current node: " << endl;
    //cout << "getNextNodePtr() pid will be: " << node.getNextNodePtr() << endl;

    int maxNumKeys = node.getKeyCount();
    int count1 = 0;
    while (count1 < maxNumKeys) {
        nEntry tmp;
        memcpy(&tmp, node.getBuffer() + count1 * sizeof(nEntry), sizeof(nEntry));

        if(tmp.key == 0)
            break;
        //cout << "key: " << tmp.key << endl;
        //cout << "pid: " << tmp.rid.pid << endl;
        //cout << "eid: " << eid << endl;
        //rf.read(page, tmp.rid.sid, tmp.key, value);
        ////cout << "string: " << value.c_str() << endl;
        //cout << endl;

        count1++;
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
