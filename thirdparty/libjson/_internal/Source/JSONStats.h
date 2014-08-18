#ifndef TestSuite_JSONStats_h
#define TestSuite_JSONStats_h

#include "../../JSONOptions.h"

#if defined(JSON_UNIT_TEST) || defined(JSON_DEBUG)
	#define LIBJSON_OBJECT(name)\
		static size_t & getCtorCounter(void){\
			static size_t count = 0;\
			static int i = JSONStats::setCallbacks(getCtorCounter, getCopyCtorCounter, getAssignmentCounter, getDtorCounter, #name);\
			return count;\
		}\
		static size_t & getCopyCtorCounter(void){\
			static size_t count = 0;\
			static int i = JSONStats::setCallbacks(getCtorCounter, getCopyCtorCounter, getAssignmentCounter, getDtorCounter, #name);\
			return count;\
		}\
		static size_t & getAssignmentCounter(void){\
			static size_t count = 0;\
			static int i = JSONStats::setCallbacks(getCtorCounter, getCopyCtorCounter, getAssignmentCounter, getDtorCounter, #name);\
			return count;\
		}\
		static size_t & getDtorCounter(void){\
			static size_t count = 0;\
			static int i = JSONStats::setCallbacks(getCtorCounter, getCopyCtorCounter, getAssignmentCounter, getDtorCounter, #name);\
			return count;\
		}
	#define LIBJSON_CTOR getCtorCounter() += 1
	#define LIBJSON_COPY_CTOR getCopyCtorCounter() += 1
	#define LIBJSON_ASSIGNMENT getAssignmentCounter() += 1
	#define LIBJSON_DTOR getDtorCounter() += 1
	
	#include <map>
	#include <sstream>
	#include <iostream>
	#include <string>
	class JSONStats {
	public:
		~JSONStats(void){ 
			std::map<getCounter_m, objectStructure*> & mymap = getMapper();
			std::map<getCounter_m, objectStructure*>::iterator b = mymap.begin();
			std::map<getCounter_m, objectStructure*>::iterator e = mymap.end();
			std::cout << "Counters for libjson:" << std::endl;
			for(; b != e; ++b){
				std::cout << "  " << b -> second -> _name << std::endl;
				std::cout << "     Constructor:      " << b -> second -> _cTor() << std::endl;
				std::cout << "     Copy Constructor: " << b -> second -> _ccTor() << std::endl;
				std::cout << "     Assignment:       " << b -> second -> _assign() << std::endl;
				std::cout << "     Destructor:       " << b -> second -> _dTor() << std::endl;
				delete b -> second;
			}
		}
		
		typedef size_t & (*getCounter_m)(void);
		struct objectStructure {
			objectStructure(getCounter_m cTor, getCounter_m ccTor, getCounter_m assign, getCounter_m dTor, const std::string & name):
				_cTor(cTor), _ccTor(ccTor), _assign(assign), _dTor(dTor), _name(name){}
			std::string _name;
			getCounter_m _cTor;
			getCounter_m _ccTor;
			getCounter_m _assign;
			getCounter_m _dTor;
		};
		static int setCallbacks(getCounter_m cTor, getCounter_m ccTor, getCounter_m assign, getCounter_m dtor, const std::string & name){
			getMapper()[cTor] = new objectStructure (cTor, ccTor, assign, dtor, name);
			return 0;
		}
		
		static std::map<getCounter_m, objectStructure*> & getMapper(void) {
			static std::map<getCounter_m, objectStructure*> mymap;
			return mymap;
		}
	};
#else
	#define LIBJSON_OBJECT(name)
	#define LIBJSON_CTOR (void)0
	#define LIBJSON_DTOR (void)0
	#define LIBJSON_COPY_CTOR (void)0
	#define LIBJSON_ASSIGNMENT (void)0
	typedef int JSONStats;
#endif

#endif
