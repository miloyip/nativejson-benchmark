# Conformance of hjiang/JSON++ (C++)

## 1. Parse Validation

* `../data/jsonchecker/fail04.json` is invalid but was mistakenly deemed valid.
~~~js
["extra comma",]
~~~

* `../data/jsonchecker/fail05.json` is invalid but was mistakenly deemed valid.
~~~js
["double extra comma",,]
~~~

* `../data/jsonchecker/fail06.json` is invalid but was mistakenly deemed valid.
~~~js
[   , "<-- missing value"]
~~~

* `../data/jsonchecker/fail07.json` is invalid but was mistakenly deemed valid.
~~~js
["Comma after the close"],
~~~

* `../data/jsonchecker/fail08.json` is invalid but was mistakenly deemed valid.
~~~js
["Extra close"]]
~~~

* `../data/jsonchecker/fail09.json` is invalid but was mistakenly deemed valid.
~~~js
{"Extra comma": true,}
~~~

* `../data/jsonchecker/fail10.json` is invalid but was mistakenly deemed valid.
~~~js
{"Extra value after close": true} "misplaced quoted value"
~~~

* `../data/jsonchecker/fail13.json` is invalid but was mistakenly deemed valid.
~~~js
{"Numbers cannot have leading zeroes": 013}
~~~

* `../data/jsonchecker/fail14.json` is invalid but was mistakenly deemed valid.
~~~js
{"Numbers cannot be hex": 0x14}
~~~

* `../data/jsonchecker/fail15.json` is invalid but was mistakenly deemed valid.
~~~js
["Illegal backslash escape: \x15"]
~~~

* `../data/jsonchecker/fail17.json` is invalid but was mistakenly deemed valid.
~~~js
["Illegal backslash escape: \017"]
~~~

* `../data/jsonchecker/fail24.json` is invalid but was mistakenly deemed valid.
~~~js
['single quote']
~~~

* `../data/jsonchecker/fail25.json` is invalid but was mistakenly deemed valid.
~~~js
["	tab	character	in	string	"]
~~~

* `../data/jsonchecker/fail26.json` is invalid but was mistakenly deemed valid.
~~~js
["tab\   character\   in\  string\  "]
~~~

* `../data/jsonchecker/fail27.json` is invalid but was mistakenly deemed valid.
~~~js
["line
break"]
~~~

* `../data/jsonchecker/fail28.json` is invalid but was mistakenly deemed valid.
~~~js
["line\
break"]
~~~


Summary: 18 of 34 are correct.

## 2. Parse Double

* `[1e-10000]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[1e-214748363]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[1e-214748364]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[2.22507385850720113605740979670913197593481954635164564e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `2.2250738585072014e-308 (0x01610000000000000)`

* `[0.999999999999999944488848768742172978818416595458984374]`
  * expect: `0.99999999999999989 (0x0163FEFFFFFFFFFFFFF)`
  * actual: `1 (0x0163FF0000000000000)`

* `[1.00000000000000011102230246251565404236316680908203126]`
  * expect: `1.0000000000000002 (0x0163FF0000000000001)`
  * actual: `1 (0x0163FF0000000000000)`

* `[7205759403792793199999e-5]`
  * expect: `72057594037927928 (0x016436FFFFFFFFFFFFF)`
  * actual: `72057594037927936 (0x0164370000000000000)`

* `[922337203685477529599999e-5]`
  * expect: `9.2233720368547748e+18 (0x01643DFFFFFFFFFFFFF)`
  * actual: `9.2233720368547758e+18 (0x01643E0000000000000)`

* `[1014120480182583464902367222169599999e-5]`
  * expect: `1.0141204801825834e+31 (0x016465FFFFFFFFFFFFF)`
  * actual: `1.0141204801825835e+31 (0x0164660000000000000)`

* `[5708990770823839207320493820740630171355185151999e-3]`
  * expect: `5.7089907708238389e+45 (0x016496FFFFFFFFFFFFF)`
  * actual: `5.7089907708238395e+45 (0x0164970000000000000)`


Summary: 56 of 66 are correct.

## 3. Parse String

* `["\u00A2"]`
  * expect: `"Â¢"` (length: 2)
  * actual: `"¢"` (length: 1)

* `["\u20AC"]`
  * expect: `"â‚¬"` (length: 3)
  * actual: `"¬"` (length: 1)

* `["\uD834\uDD1E"]`
  * expect: `"ð„ž"` (length: 4)
  * actual: `"4"` (length: 2)


Summary: 6 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[null]
~~~

~~~js
[
	null 
] 

~~~

* Fail:
~~~js
[true]
~~~

~~~js
[
	true 
] 

~~~

* Fail:
~~~js
[false]
~~~

~~~js
[
	false 
] 

~~~

* Fail:
~~~js
[0]
~~~

~~~js
[
	0 
] 

~~~

* Fail:
~~~js
["foo"]
~~~

~~~js
[
	"foo" 
] 

~~~

* Fail:
~~~js
[0,1]
~~~

~~~js
[
	0,
	1 
] 

~~~

* Fail:
~~~js
{"foo":"bar"}
~~~

~~~js
{
	"foo": "bar" 
} 

~~~

* Fail:
~~~js
{"a":null,"foo":"bar"}
~~~

~~~js
{
	"a": null,
	"foo": "bar" 
} 

~~~

* Fail:
~~~js
[-1]
~~~

~~~js
[
	-1 
] 

~~~

* Fail:
~~~js
[-2147483648]
~~~

~~~js
[
	-2147483648 
] 

~~~

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[
	-1234567890123456789 
] 

~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[
	-9223372036854775808 
] 

~~~

* Fail:
~~~js
[1]
~~~

~~~js
[
	1 
] 

~~~

* Fail:
~~~js
[2147483647]
~~~

~~~js
[
	2147483647 
] 

~~~

* Fail:
~~~js
[4294967295]
~~~

~~~js
[
	4294967295 
] 

~~~

* Fail:
~~~js
[1234567890123456789]
~~~

~~~js
[
	1234567890123456789 
] 

~~~

* Fail:
~~~js
[9223372036854775807]
~~~

~~~js
[
	9223372036854775807 
] 

~~~

* Fail:
~~~js
[0.0]
~~~

~~~js
[
	0 
] 

~~~

* Fail:
~~~js
[-0.0]
~~~

~~~js
[
	-0 
] 

~~~

* Fail:
~~~js
[1.2345]
~~~

~~~js
[
	1.2345 
] 

~~~

* Fail:
~~~js
[-1.2345]
~~~

~~~js
[
	-1.2345 
] 

~~~

* Fail:
~~~js
[5e-324]
~~~

~~~js
[
	5e-324 
] 

~~~

* Fail:
~~~js
[2.225073858507201e-308]
~~~

~~~js
[
	2.225073858507201e-308 
] 

~~~

* Fail:
~~~js
[2.2250738585072014e-308]
~~~

~~~js
[
	2.2250738585072014e-308 
] 

~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[
	1.7976931348623157e+308 
] 

~~~


Summary: 2 of 27 are correct.

