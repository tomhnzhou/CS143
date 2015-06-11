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
#include <string>
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

bool SqlEngine::check_condition(const vector<SelCond>& cond, int key, string value)
{
	int diff;
	for(int i = 0; i < cond.size(); i++){
	switch (cond[i].attr) {
        case 1:
      diff = key - atoi(cond[i].value);
      break;
        case 2:
      diff = strcmp(value.c_str(), cond[i].value);
      break;
        }

        switch (cond[i].comp) {
        case SelCond::EQ:
      if (diff != 0) return false;
      break;
        case SelCond::NE:
      if (diff == 0) return false;
      break;
        case SelCond::GT:
      if (diff <= 0) return false;
      break;
        case SelCond::LT:
      if (diff >= 0) return false;
      break;
        case SelCond::GE:
      if (diff < 0) return false;
      break;
        case SelCond::LE:
      if (diff > 0) return false;
      break;
        }
	}
	return true;
      
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value = "";
  int    count = 0;
  int    diff;
  bool use_index = false;
  int n_value_cond = 0;

	int min_key = -1; int max_key = INT_MAX;
	vector<SelCond> check_cond;


  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

string index_name = table + ".idx";
//printf("index name is %s\n", index_name.c_str());
FILE* fd;
if(fd = fopen(index_name.c_str(), "r")){
	fclose(fd);

	//SelCond* min_value = NULL; SelCond* max_value = NULL;
	//SelCond* equal_value = NULL; 
	int cond_value;
	for(int i = 0; i < cond.size(); i++){
		if(cond[i].attr == 1){	// key cond
			/*
			switch(cond[i].comp){
				case SelCond::EQ:
					if(!equal_key)
						equal_key = &cond[i];
					else
						check_cond.push_back(cond[i]);
					break;
				case SelCond::NE:
					check_cond.push_back(cond[i]);
					break;
				case SelCond::LT:
					if(!max_key)
						max_key = &cond[i];
					else{
						int old_max = atoi(max_key->value);
						int new_max = atoi(cond[i].value);
						if(old_max > new_max
						|| (old_max == new_max && max_key->comp == SelCond::LE))
							max_key = &cond[i];
					}
					break;
				case SelCond::GT:
					if(!min_key)
						min_key = &cond[i];
					else{
						int old_min = atoi(min_key->value);
						int new_min = atoi(cond[i].value);
						if(old_min < new_min
						|| (old_min == new_min && min_key->comp == SelCond::GE))
							min_key = &cond[i];
					}
					break;
				case SelCond::LE:
					if(!max_key)
						max_key = &cond[i];
					else{
						int old_max = atoi(max_key->value);
						int new_max = atoi(cond[i].value);
						if(old_max > new_max)
							max_key = &cond[i];
					}
					break;
				case SelCond::GE:
					if(!min_key)
						min_key = &cond[i];
					else{
						int old_min = atoi(min_key->value);
						int new_min = atoi(cond[i].value);
						if(old_min < new_min)
							min_key = &cond[i];
					}
					break;
			}
			*/
			cond_value = atoi(cond[i].value);
			switch(cond[i].comp){
				case SelCond::EQ:
					min_key = max_key = cond_value;
					break;
				case SelCond::NE:
					check_cond.push_back(cond[i]);
					break;
				case SelCond::LT:
					if(cond_value -1 < max_key)
						max_key = cond_value - 1; 
					break;
				case SelCond::GT:
					if(cond_value + 1 > min_key)
						min_key = cond_value + 1;
					break;
				case SelCond::LE:
					if(cond_value < max_key)
						max_key = cond_value;
					break;
				case SelCond::GE:
					if(cond_value > min_key)
						min_key = cond_value;
					break;
			}

		}
		else{	//value cond
			/*switch(cond[i].comp){
				case SelCond::EQ:
					if(!equal_value)
						equal_value = &cond[i];
					else
						check_cond.push_back(cond[i]);
					break;
				case SelCond::NE:
					check_cond.push_back(cond[i]);
					break;
				case SelCond::LT:
					if(!max_value)
						max_value = &cond[i];
					else{
						int diff = strcmp(max_value->value, cond[i].value);
						if(diff > 0 
						|| (diff == 0 && max_value->comp == SelCond::LE))
							max_value = &cond[i];
					}
					break;
				case SelCond::GT:
					if(!min_value)
						min_value = &cond[i];
					else{
						int diff = strcmp(min_value->value, cond[i].value);
						if(diff < 0 
						|| (diff == 0 && min_value->comp == SelCond::LE))
							min_value = &cond[i];
					}
					break;
				case SelCond::LT:
					if(!max_value)
						max_value = &cond[i];
					else{
						int diff = strcmp(max_value->value, cond[i].value);
						if(diff > 0)
							max_value = &cond[i];
					}
					break;
				case SelCond::GT:
					if(!min_value)
						min_value = &cond[i];
					else{
						int diff = strcmp(min_value->value, cond[i].value);
						if(diff < 0)
							min_value = &cond[i];
					}
					break;
			}
			*/
			n_value_cond++;
			check_cond.push_back(cond[i]);
		}
	}

	//printf("Key > min = %d, max = %d\n", 
		//min_key, max_key);

	if(min_key > -1 || max_key < INT_MAX || attr == 1 || attr == 4)
		use_index = true;
}

if(use_index){

	BTreeIndex* tree = new BTreeIndex(index_name, 'r');
	IndexCursor cursor;
	
	tree->locate(min_key, cursor);
	//printf("After locate: pid=%d, eid=%d\n", cursor.pid, cursor.eid); 
	//RecordId index_rid;
	int rc;
	for(;;){
		if((rc = tree->readForward(cursor, key, rid)) < 0) break;
		if(key > max_key)
			break;

		if(n_value_cond > 0 || attr == 2 || attr == 3)
			if((rc = rf.read(rid, key, value)) < 0) 
				break;
		//printf("Read from record: key=%d, value=%s\n", key, value.c_str());
		if(check_condition(check_cond, key, value)){
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
    		count++;
		}
	}
}
else {


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
  RC     rc;
  int    key;     
  string value;
  int linelen;

  FILE* file = fopen(loadfile.c_str(), "r");
  if(file == NULL) return RC_FILE_OPEN_FAILED;

  RecordFile rf;
  if((rc = rf.open(table + ".tbl", 'w')) < 0) {fclose(file); return rc;}
  RecordId rid = rf.endRid();

  char* line = NULL;
  size_t len = 0;
  ssize_t n_read;

	BTreeIndex* tree_index;
  if(index){
    string index_name = table + ".idx";
     tree_index = new BTreeIndex(index_name, 'w');
  }

  while ((n_read = getline(&line, &len, file)) > 0) {
    linelen = strlen(line);
    line[linelen-1] = '\0';
    if(line[linelen-2] == '\r')
      line[linelen-2] = '\0';

    //printf("Retrieved line of length %d :", strlen(line));
    //printf("%s\n", line);

    rc = parseLoadLine(string(line), key, value);
    if(rc < 0){fclose(file); rf.close(); return rc;}
    //printf("Parsed line: key=%d, value=%s\n\n",key,value.c_str());

    if((rc = rf.append(key, value, rid)) < 0) 
      {fclose(file); rf.close(); return rc;}

    if(index){
      if((rc = tree_index->insert(key, rid)) < 0)
      {
        printf("ERROR: Failed to insert key into index tree: %d\n", rc);
        rf.close(); tree_index->close(); fclose(file); 
        return rc;
      }
    }

    rid++;
  }
  fclose(file);
  //tree_index->print();
  rf.close(); tree_index->close(); 
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
