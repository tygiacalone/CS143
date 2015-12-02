/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
    fprintf(stdout, "Bruinbase> ");
    
    // set the command line input and start parsing user input
    sqlin = commandline;
    sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
    // SqlParser.y by bison (bison is GNU equivalent of yacc)
    
    return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
    RecordFile rf;   // RecordFile containing the table
    RecordId   rid;  // record cursor for table scanning
    
    RC     rc;
    int    key;
    string value;
    int    count = 0;
    int    diff;
    BTreeIndex btree;
    IndexCursor cursor;
    bool index = true;
    bool strFlag = false;
    
    RC ret = btree.open(table + ".idx", 'r');
    //cout << "file open return is: " << ret << endl;
    // open the index if it exists
    if(ret != 0)
        index = false;
    
    // open the table file
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
        //fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
        return rc;
    }
    
    int valnum;
    if(index) //if using BTree
    {
        //cout << "count INITIAL value is: " << count << endl;
        int place = -1; //keeps track of which condition is not measuring value
        for(int i=0; i<cond.size(); i++) //Look for first non-value condition
        {
            if(cond[i].attr == 2) //if measuring value (which is not how Btree is indexed), skip this condition
            {
                strFlag = true;
                continue;
            }

            if(cond[i].comp == SelCond::EQ)
            {
                valnum = atoi(cond[i].value);
                place = i;
                break;
            }

            if((cond[i].comp == SelCond::GT || cond[i].comp == SelCond::GE) && (place == -1 || atoi(cond[i].value) > atoi(cond[place].value)))
                place = i;
            
        }

        if(place >= 0) {
            //cout << "place >= 0" << endl;
            btree.locate(atoi(cond[place].value), cursor);
            if (cursor.eid != 0)
                cursor.eid = cursor.eid - 1;
        }
        else {
            //cout << "place < 0" << endl;
            btree.locate(0, cursor);
        }


        //cout << "pid, eid INITIAL is: " << cursor.pid << ", " << cursor.eid << endl;
        //cout << "key on 94 is: " << key << endl;
        //cout << "cursor.eid on 95 is: " << cursor.eid << endl;
        //cout << "cursor.pid on 96 is: " << cursor.pid << endl;
        while(btree.readForward(cursor, key, rid) == 0)
        {
            //cout << "key on 98 is: " << key << endl;
            //cout << "count value is: " << count << endl;
            //cout << "cursor.eid on 102 is: " << cursor.eid << endl;
            //cout << "cursor.pid on 103 is: " << cursor.pid << endl;

            //cout << "pid, eid is: " << cursor.pid << ", " << cursor.eid << endl;
            //cout << "value before read: " << value.c_str() << endl;
            //cout << "key before read: " << key << endl;
            RC ret = rf.read(rid, key, value);
            //cout << "value after read: " << value.c_str() << endl;
            //cout << "key after read: " << key << endl;

            if (ret < 0) {
                //cout << "Error reading tuple, now exiting" << endl;
                goto read_done;
            }

            //cout << "pid, eid 99 is: " << cursor.pid << ", " << cursor.eid << endl;
            //cout << "value in select is: " << atoi(cond[0].value) << endl;
            //cout << "key in select is: " << key << endl;

            for (int i = 0; i < cond.size(); i++) {
                switch (cond[i].attr) {
                    case 1: //
                        diff = key - atoi(cond[i].value);
                        //cout << "Case for int comparison" << endl;
                        break;
                    case 2: //
                        diff = strcmp(value.c_str(), cond[i].value);
                        //cout << "Case for str comparison" << endl;
                        break;
                }
                //cout << "diff in select is: " << diff << endl;

                //cout << "pid, eid 111 is: " << cursor.pid << ", " << cursor.eid << endl;
                switch (cond[i].comp) {
                    case SelCond::EQ:
                        if (diff != 0) {
                            //cout << "I'm in here!!! 144 switch comp" << endl;
                            if (cond[i].attr == 1) goto read_done;
                            else {
                                //cout << "I'm continuing!!! 146 switch comp" << endl;
                                goto next_loop;
                            }
                        }
                        break;
                    case SelCond::NE:
                        if (diff == 0) goto next_loop;
                        break;
                    case SelCond::GT:
                        if (diff <= 0) goto next_loop;
                        break;
                    case SelCond::LT:
                        if (diff >= 0) {
                            if (cond[i].attr == 1) goto read_done;
                            else
                                goto next_loop;
                        }
                        break;
                    case SelCond::GE:
                        if (diff < 0) goto next_loop;
                        break;
                    case SelCond::LE:
                        if (diff > 0) {
                            if (cond[i].attr == 1) goto read_done;
                            else
                                goto next_loop;
                        }
                        break;
                }

                //cout << "pid, eid 137 is: " << cursor.pid << ", " << cursor.eid << endl;
            }

            count++;

            switch (attr) {
                case 1: //SELECT key
                    cout << key << endl;
                    break;
                case 2: //SELECT value
                    cout << value.c_str() << endl;
                    break;
                case 3: //SELECT *
                    //cout << "I'm printing a tuple!!!! 193" << endl;
                    cout << key << " '" << value.c_str() << "'" << endl;
                    break;
            }

            next_loop:
                int j;
            //cout << "pid, eid end is: " << cursor.pid << ", " << cursor.eid << endl;
        }
    }
    else{
        // scan the table file from the beginning
        rid.pid = rid.sid = 0;
        count = 0;
        while (rid < rf.endRid()) {
            // read the tuple
            if ((rc = rf.read(rid, key, value)) < 0) {
                fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
                goto exit_select;
            }
            
            // check the conditions on the tuple
            for (unsigned i = 0; i < cond.size(); i++) {
                // compute the difference between the tuple value and the condition value
                switch (cond[i].attr) {
                    case 1:
                        diff = key - atoi(cond[i].value);
                        break;
                    case 2:
                        diff = strcmp(value.c_str(), cond[i].value);
                        break;
                }

                
                // skip the tuple if any condition is not met
                switch (cond[i].comp) {
                    case SelCond::EQ:
                        if (diff != 0) goto next_tuple;
                        break;
                    case SelCond::NE:
                        if (diff == 0) goto next_tuple;
                        break;
                    case SelCond::GT:
                        if (diff <= 0) goto next_tuple;
                        break;
                    case SelCond::LT:
                        if (diff >= 0) goto next_tuple;
                        break;
                    case SelCond::GE:
                        if (diff < 0) goto next_tuple;
                        break;
                    case SelCond::LE:
                        if (diff > 0) goto next_tuple;
                        break;
                }
            }
            
            // the condition is met for the tuple.
            // increase matching tuple counter
            count++;
            
            // print the tuple
            switch (attr) {
                case 1:  // SELECT key
                    fprintf(stdout, "%d\n", key);
                    break;
                case 2:  // SELECT value
                    fprintf(stdout, "%s\n", value.c_str());
                    break;
                case 3:  // SELECT *
                    fprintf(stdout, "%d '%s'\n", key, value.c_str());
                    break;
            }
            
            // move to the next tuple
        next_tuple:
            ++rid;
        }
    }

read_done:
    if (attr == 4) {
        fprintf(stdout, "%d\n", count);
    }
    rc = 0;

    // close the table file and return
exit_select:
    rf.close();
    return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
    /* your code here */
    
    //Check this syntax:
    const char * c = loadfile.c_str();
    ifstream infile(c);
    
    string tblname = table + ".tbl";
    RecordFile r;
    r.open(tblname, 'w');
    
    BTreeIndex btree;

    RecordId rid = r.endRid();

    if(index)
    {
        if(btree.open(table + ".idx", 'w') < 0)
            return -1;
    }
    
    int key;
    string value = "";
    string line = "";

    int count = 0;
    if(index) {
        while (count <= 8)
        {
            //cout << "Value INITIAL of rid.pid in LOOP load: " << rid.pid << endl;
            if(r.append( key, value, rid) < 0)
                return -1;
            //cout << "Value AFTER APPEND of rid.pid in LOOP load: " << rid.pid << endl;

            count++;
        }
    }


    while (getline(infile, line))
    {
        //cout << "line is: " << line << endl;

        if(parseLoadLine(line, key, value) < 0)
            return -1;

        //cout << "Value INITIAL of rid.pid in load: " << rid.pid << endl;
        if(r.append( key, value, rid) < 0)
            return -1;
        //cout << "Value AFTER APPEND of rid.pid in load: " << rid.pid << endl;
        if(index)
            btree.insert(key, rid);


        //cout << "Value FINAL of rid.pid in load: " << rid.pid << endl;
    }
    if(index)
        btree.close();
    
    r.close();
    
    
    
    
    return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }
    
    // get the integer key value
    key = atoi(s);
    
    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }
    
    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }
    
    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }
    
    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }
    
    return 0;
}
