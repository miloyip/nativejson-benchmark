#include <iostream>
#include "json.hh"

using namespace std;
using namespace JSON;

int main()
{
    // Load JSON file
    
    Value v = parse_file("tests/comp.json");    
    cerr << v << endl;
    cerr << "---" << endl;
    
    // Build object programmatically
    Object obj;
    
    obj["foo"] = true;
    obj["bar"] = 3LL;
    obj["bar"] = 3L;
    obj["bar"] = 3;
    
    Object o;
    o["failure"] = true;
    o["success"] = "no way";
    
    obj["baz"] = o;
    
    Array a;
    a.push_back(true);
    a.push_back("asia");
    a.push_back("europa");
    a.push_back(55LL);
    a.push_back(3.12L);
    a.push_back(3.12);
        
    obj["beer"] = a;

    cerr << obj << endl;
    
    return 0;
}