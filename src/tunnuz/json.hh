#ifndef JSON_HH
#define JSON_HH

#include "json_st.hh" // JSON syntax tree
#include "json.tab.hh" // parser
  
JSON::Value parse_file(const char* filename);
JSON::Value parse_string(const std::string& s);

#endif