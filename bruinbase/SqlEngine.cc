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
    int    count;
    int    diff;
    BTreeIndex btree = BTreeIndex();
    IndexCursor cursor = IndexCursor();
    bool index = true;
    
    
    // open the index if it exists
    if(btree.open(table + ".tbl", 'r') < 0)
        index = false;
    
    // open the table file
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
        fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
        return rc;
    }
    
    int valnum;
    if(index) //if using BTree
    {
        int place = -1; //keeps track of which condition is not measuring value
        for(int i=0; i<cond.size(); i++) //Look for first non-value condition
        {
            if(cond[i].attr == 2) //if measuring value (which is not how Btree is indexed), skip this condition
                continue;
            else if(cond[i].comp == SelCond::EQ)
            {
                valnum = atoi(cond[i].value);
                place = i;
                break;
            }
            else if((cond[i].comp == SelCond::GE || cond[i].comp == SelCond::GE) && (place == -1 || atoi(cond[i].value) > atoi(cond[place].value)))
                place = i;
            
        }
        if(place >= 0)
            btree.locate(atoi(cond[place].value), cursor);
        else
            btree.locate(0, cursor);
        
        
        while(btree.readForward(cursor, key, rid) == 0)
        {
            RC ret = rf.read(rid, key, value);
            for(int i=0; i<cond.size(); i++)
            {
                switch(cond[i].attr)
                {
                    case 1: //
                        diff = key - atoi(cond[i].value);
                        break;
                    case 2: //
                        diff = strcmp(value.c_str(), cond[i].value);
                        break;
                }
                
                switch(cond[i].comp)
                {
                    case SelCond::EQ:
                        if(diff != 0)
                        {
                            if(cond[i].attr == 1) goto print_values;
                            else continue;
                        }
                        break;
                    case SelCond::NE:
                        if(diff == 0) continue;
                        break;
                    case SelCond::GT:
                        if(diff <= 0) continue;
                        break;
                    case SelCond::LT:
                        if(diff >= 0)
                        {
                            if(cond[i].attr == 1) goto print_values;
                            else
                                continue;
                        }
                        break;
                }
            }
            count++;
            
            switch(attr)
            {
                case 1: //SELECT key
                    cout << key << endl;
                    break;
                case 2: //SELECT value
                    cout<< value.c_str()<< endl;
                    break;
                case 3: //SELECT *
                    cout << key << "'" << value.c_str() << "'" << endl;
                    break;
            }
        }
    print_values:
        if(attr == 4) //Count
            cout<< count <<endl;
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
    
    // print matching tuple count if "select count(*)"
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
    RecordFile r(tblname, 'w');
    r.open(tblname, 'w');
    
    BTreeIndex btree = BTreeIndex();
    if(index)
    {
        if(btree.open(table + ".idx", 'w') < 0)
            return -1;
    }
    
    int key;
    string value;
    string line;
    while (getline(infile, line))
    {
        if(parseLoadLine(line, key, value) < 0)
            return -1;
        RecordId rid;
        if(r.append( key, value, rid) < 0)
            return -1;
        if(index)
            btree.insert(key, rid);
        
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
