# Conformance of ArduinoJson (C++)

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

* `../data/jsonchecker/pass02.json` is valid but was mistakenly deemed invalid.
~~~js
[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
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


Summary: 15 of 34 are correct.

## 2. Parse Double

* `[1E+10]`
  * expect: `10000000000 (0x0164202A05F20000000)`
  * actual: `0 (0x0160)`

* `[-1E+10]`
  * expect: `-10000000000 (0x016C202A05F20000000)`
  * actual: `0 (0x0160)`

* `[1.234E+10]`
  * expect: `12340000000 (0x0164206FC2BA8000000)`
  * actual: `0 (0x0160)`

* `[1.79769e+308]`
  * expect: `1.7976900000000001e+308 (0x0167FEFFFFC57CA82AE)`
  * actual: `0 (0x0160)`

* `[-1.79769e+308]`
  * expect: `-1.7976900000000001e+308 (0x016FFEFFFFC57CA82AE)`
  * actual: `0 (0x0160)`

* `[1.7976931348623157e+308]`
  * expect: `1.7976931348623157e+308 (0x0167FEFFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[0.017976931348623157e+310]`
  * expect: `1.7976931348623157e+308 (0x0167FEFFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`


Summary: 59 of 66 are correct.

## 3. Parse String

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `"Hellou0000World"` (length: 15)

* `["\u0024"]`
  * expect: `"$"` (length: 1)
  * actual: `"u0024"` (length: 5)

* `["\u00A2"]`
  * expect: `"¢"` (length: 2)
  * actual: `"u00A2"` (length: 5)

* `["\u20AC"]`
  * expect: `"€"` (length: 3)
  * actual: `"u20AC"` (length: 5)

* `["\uD834\uDD1E"]`
  * expect: `"𝄞"` (length: 4)
  * actual: `"uD834uDD1E"` (length: 10)


Summary: 4 of 9 are correct.

## 4. Roundtrip


Summary: 27 of 27 are correct.

