# Conformance of nbsdx_SimpleJSON (C++11)

## 1. Parse Validation

* `../data/jsonchecker/pass01.json` is valid but was mistakenly deemed invalid.
~~~js
[
    "JSON Test Pattern pass1",
    {"object with 1 member":["array with 1 element"]},
    {},
    [],
    -42,
    true,
    false,
    null,
    {
        "integer": 1234567890,
        "real": -9876.543210,
        "e": 0.123456789e-12,
        "E": 1.234567890E+34,
        "":  23456789012E66,
        "zero": 0,
        "one": 1,
        "space": " ",
        "quote": "\"",
        "backslash": "\\",
        "controls": "\b\f\n\r\t",
        "slash": "/ & \/",
        "alpha": "abcdefghijklmnopqrstuvwyz",
        "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
        "digit": "0123456789",
        "0123456789": "digit",
        "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
        "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
        "true": true,
        "false": false,
        "null": null,
        "array":[  ],
        "object":{  },
        "address": "50 St. James Street",
        "url": "http://www.JSON.org/",
        "comment": "// /* <!-- --",
        "# -- --> */": " ",
        " s p a c e d " :[1,2 , 3

,

4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
        "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
        "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
        "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
: "A key can be any string"
    },
    0.5 ,98.6
,
99.44
,

1066,
1e1,
0.1e1,
1e-1,
1e00,2e+00,2e-00
,"rosebud"]
~~~

* `../data/jsonchecker/fail02.json` is invalid but was mistakenly deemed valid.
~~~js
["Unclosed array"
~~~

* `../data/jsonchecker/fail03.json` is invalid but was mistakenly deemed valid.
~~~js
{unquoted_key: "keys must be quoted"}
~~~

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

* `../data/jsonchecker/fail11.json` is invalid but was mistakenly deemed valid.
~~~js
{"Illegal expression": 1 + 2}
~~~

* `../data/jsonchecker/fail12.json` is invalid but was mistakenly deemed valid.
~~~js
{"Illegal invocation": alert()}
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

* `../data/jsonchecker/fail16.json` is invalid but was mistakenly deemed valid.
~~~js
[\naked]
~~~

* `../data/jsonchecker/fail17.json` is invalid but was mistakenly deemed valid.
~~~js
["Illegal backslash escape: \017"]
~~~

* `../data/jsonchecker/fail19.json` is invalid but was mistakenly deemed valid.
~~~js
{"Missing colon" null}
~~~

* `../data/jsonchecker/fail20.json` is invalid but was mistakenly deemed valid.
~~~js
{"Double colon":: null}
~~~

* `../data/jsonchecker/fail21.json` is invalid but was mistakenly deemed valid.
~~~js
{"Comma instead of colon", null}
~~~

* `../data/jsonchecker/fail22.json` is invalid but was mistakenly deemed valid.
~~~js
["Colon instead of comma": false]
~~~

* `../data/jsonchecker/fail23.json` is invalid but was mistakenly deemed valid.
~~~js
["Bad value", truth]
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

* `../data/jsonchecker/fail29.json` is invalid but was mistakenly deemed valid.
~~~js
[0e]
~~~

* `../data/jsonchecker/fail31.json` is invalid but was mistakenly deemed valid.
~~~js
[0e+-1]
~~~

* `../data/jsonchecker/fail32.json` is invalid but was mistakenly deemed valid.
~~~js
{"Comma instead if closing brace": true,
~~~

* `../data/jsonchecker/fail33.json` is invalid but was mistakenly deemed valid.
~~~js
["mismatch"}
~~~


Summary: 3 of 34 are correct.

## 2. Parse Double

* `[1E10]`
  * expect: `10000000000 (0x0164202A05F20000000)`
  * actual: `1 (0x0163FF0000000000000)`

* `[1e10]`
  * expect: `10000000000 (0x0164202A05F20000000)`
  * actual: `1 (0x0163FF0000000000000)`

* `[1E-10]`
  * expect: `1e-10 (0x0163DDB7CDFD9D7BDBB)`
  * actual: `1 (0x0163FF0000000000000)`

* `[-1E10]`
  * expect: `-10000000000 (0x016C202A05F20000000)`
  * actual: `-1 (0x016BFF0000000000000)`

* `[-1e10]`
  * expect: `-10000000000 (0x016C202A05F20000000)`
  * actual: `-1 (0x016BFF0000000000000)`

* `[-1E-10]`
  * expect: `-1e-10 (0x016BDDB7CDFD9D7BDBB)`
  * actual: `-1 (0x016BFF0000000000000)`

* `[1.234E-10]`
  * expect: `1.2340000000000001e-10 (0x0163DE0F5C0635643A8)`
  * actual: `1.234 (0x0163FF3BE76C8B43958)`

* `[2.22507e-308]`
  * expect: `2.2250699999999998e-308 (0x016FFFFE2E8159D0)`
  * actual: `2.2250700000000002e-08 (0x0163E57E439D0EA38E8)`

* `[-2.22507e-308]`
  * expect: `-2.2250699999999998e-308 (0x016800FFFFE2E8159D0)`
  * actual: `-2.2250700000000002e-08 (0x016BE57E439D0EA38E8)`

* `[4.9406564584124654e-324]`
  * expect: `4.9406564584124654e-324 (0x0161)`
  * actual: `4.940656458412465e-24 (0x0163B17E43C8800759B)`

* `[2.2250738585072009e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `2.2250738585072012e-08 (0x0163E57E43C8800759B)`

* `[2.2250738585072014e-308]`
  * expect: `2.2250738585072014e-308 (0x01610000000000000)`
  * actual: `2.2250738585072015e-08 (0x0163E57E43C8800759C)`

* `[1e-10000]`
  * expect: `0 (0x0160)`
  * actual: `1 (0x0163FF0000000000000)`

* `[18446744073709551616]`
  * expect: `1.8446744073709552e+19 (0x01643F0000000000000)`
  * actual: `0 (0x0160)`

* `[-9223372036854775809]`
  * expect: `-9.2233720368547758e+18 (0x016C3E0000000000000)`
  * actual: `0 (0x0160)`

* `[123e34]`
  * expect: `1.23e+36 (0x016476D9C75D3AC072B)`
  * actual: `1230000 (0x0164132C4B000000000)`

* `[2.2250738585072011e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `2.2250738585072012e-08 (0x0163E57E43C8800759B)`

* `[0.017976931348623157e+310]`
  * expect: `1.7976931348623157e+308 (0x0167FEFFFFFFFFFFFFF)`
  * actual: `inf (0x0167FF0000000000000)`

* `[2.2250738585072012e-308]`
  * expect: `2.2250738585072014e-308 (0x01610000000000000)`
  * actual: `2.2250738585072015e-08 (0x0163E57E43C8800759C)`

* `[2.22507385850720113605740979670913197593481954635164564e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `2.2250738585072012e-08 (0x0163E57E43C8800759B)`

* `[2.22507385850720113605740979670913197593481954635164565e-308]`
  * expect: `2.2250738585072014e-308 (0x01610000000000000)`
  * actual: `2.2250738585072012e-08 (0x0163E57E43C8800759B)`

* `[7205759403792793199999e-5]`
  * expect: `72057594037927928 (0x016436FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[7205759403792793200001e-5]`
  * expect: `72057594037927936 (0x0164370000000000000)`
  * actual: `0 (0x0160)`

* `[922337203685477529599999e-5]`
  * expect: `9.2233720368547748e+18 (0x01643DFFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[922337203685477529600001e-5]`
  * expect: `9.2233720368547758e+18 (0x01643E0000000000000)`
  * actual: `0 (0x0160)`

* `[10141204801825834086073718800384]`
  * expect: `1.0141204801825834e+31 (0x016465FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[10141204801825835211973625643008]`
  * expect: `1.0141204801825835e+31 (0x0164660000000000000)`
  * actual: `0 (0x0160)`

* `[10141204801825834649023672221696]`
  * expect: `1.0141204801825835e+31 (0x0164660000000000000)`
  * actual: `0 (0x0160)`

* `[1014120480182583464902367222169599999e-5]`
  * expect: `1.0141204801825834e+31 (0x016465FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[1014120480182583464902367222169600001e-5]`
  * expect: `1.0141204801825835e+31 (0x0164660000000000000)`
  * actual: `0 (0x0160)`

* `[5708990770823838890407843763683279797179383808]`
  * expect: `5.7089907708238389e+45 (0x016496FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[5708990770823839524233143877797980545530986496]`
  * expect: `5.7089907708238395e+45 (0x0164970000000000000)`
  * actual: `0 (0x0160)`

* `[5708990770823839207320493820740630171355185152]`
  * expect: `5.7089907708238395e+45 (0x0164970000000000000)`
  * actual: `0 (0x0160)`

* `[5708990770823839207320493820740630171355185151999e-3]`
  * expect: `5.7089907708238389e+45 (0x016496FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[5708990770823839207320493820740630171355185152001e-3]`
  * expect: `5.7089907708238395e+45 (0x0164970000000000000)`
  * actual: `0 (0x0160)`

* `[100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]`
  * expect: `1e+308 (0x0167FE1CCF385EBC8A0)`
  * actual: `0 (0x0160)`

* `[2.225073858507201136057409796709131975934819546351645648023426109724822222021076945516529523908135087914149158913039621106870086438694594645527657207407820621743379988141063267329253552286881372149012981122451451889849057222307285255133155755015914397476397983411801999323962548289017107081850690630666655994938275772572015763062690663332647565300009245888316433037779791869612049497390377829704905051080609940730262937128958950003583799967207254304360284078895771796150945516748243471030702609144621572289880258182545180325707018860872113128079512233426288368622321503775666622503982534335974568884423900265498198385487948292206894721689831099698365846814022854243330660339850886445804001034933970427567186443383770486037861622771738545623065874679014086723327636718751234567890123456789012345678901e-308]`
  * expect: `2.2250738585072014e-308 (0x01610000000000000)`
  * actual: `2.2250738585072012e-08 (0x0163E57E43C8800759B)`


Summary: 29 of 66 are correct.

## 3. Parse String

* `["Hello\nWorld"]`
  * expect: `"Hello
World"` (length: 11)
  * actual: `"Hello\nWorld"` (length: 12)

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `"Hello\\u0000World"` (length: 17)

* `["\"\\/\b\f\n\r\t"]`
  * expect: `""\/
	"` (length: 8)
  * actual: `"\"\\/\b\f\n\r\t"` (length: 15)

* `["\u0024"]`
  * expect: `"$"` (length: 1)
  * actual: `"\\u0024"` (length: 7)

* `["\u00A2"]`
  * expect: `"¢"` (length: 2)
  * actual: `"\\u00A2"` (length: 7)

* `["\u20AC"]`
  * expect: `"€"` (length: 3)
  * actual: `"\\u20AC"` (length: 7)

* `["\uD834\uDD1E"]`
  * expect: `"𝄞"` (length: 4)
  * actual: `"\\uD834\\uDD1E"` (length: 14)


Summary: 2 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[0,1]
~~~

~~~js
[0, 1]
~~~

* Fail:
~~~js
{"foo":"bar"}
~~~

~~~js
{
  "foo" : "bar"
}
~~~

* Fail:
~~~js
{"a":null,"foo":"bar"}
~~~

~~~js
{
  "a" : null,
  "foo" : "bar"
}
~~~

* Fail:
~~~js
[0.0]
~~~

~~~js
[0.000000]
~~~

* Fail:
~~~js
[-0.0]
~~~

~~~js
[-0.000000]
~~~

* Fail:
~~~js
[1.2345]
~~~

~~~js
[1.234500]
~~~

* Fail:
~~~js
[-1.2345]
~~~

~~~js
[-1.234500]
~~~

* Fail:
~~~js
[5e-324]
~~~

~~~js
[0.000000]
~~~

* Fail:
~~~js
[2.225073858507201e-308]
~~~

~~~js
[0.000000]
~~~

* Fail:
~~~js
[2.2250738585072014e-308]
~~~

~~~js
[0.000000]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[179769313.486232]
~~~


Summary: 16 of 27 are correct.

