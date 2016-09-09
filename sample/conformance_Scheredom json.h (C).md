# Conformance of Scheredom json.h (C)

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

* `../data/jsonchecker/fail25.json` is invalid but was mistakenly deemed valid.
~~~js
["	tab	character	in	string	"]
~~~

* `../data/jsonchecker/fail27.json` is invalid but was mistakenly deemed valid.
~~~js
["line
break"]
~~~

* `../data/jsonchecker/fail29.json` is invalid but was mistakenly deemed valid.
~~~js
[0e]
~~~

* `../data/jsonchecker/fail30.json` is invalid but was mistakenly deemed valid.
~~~js
[0e+]
~~~


Summary: 25 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String

* `["Hello\nWorld"]`
  * expect: `"Hello
World"` (length: 11)
  * actual: `"Hello\nWorld"` (length: 12)

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `""` (length: 0)

* `["\"\\/\b\f\n\r\t"]`
  * expect: `""\/
	"` (length: 8)
  * actual: `"\"\\/\b\f\n\r\t"` (length: 15)

* `["\u0024"]`
  * expect: `"$"` (length: 1)
  * actual: `""` (length: 0)

* `["\u00A2"]`
  * expect: `"¢"` (length: 2)
  * actual: `""` (length: 0)

* `["\u20AC"]`
  * expect: `"€"` (length: 3)
  * actual: `""` (length: 0)

* `["\uD834\uDD1E"]`
  * expect: `"𝄞"` (length: 4)
  * actual: `""` (length: 0)


Summary: 2 of 9 are correct.

## 4. Roundtrip


Summary: 27 of 27 are correct.

