# Conformance of ujson (C++)

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


Summary: 33 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String


Summary: 9 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-2147483648]
~~~

~~~js
[-2.147483648e9]
~~~

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[-1.2345678901234568e18]
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9.223372036854776e18]
~~~

* Fail:
~~~js
[2147483647]
~~~

~~~js
[2.147483647e9]
~~~

* Fail:
~~~js
[4294967295]
~~~

~~~js
[4.294967295e9]
~~~

* Fail:
~~~js
[1234567890123456789]
~~~

~~~js
[1.2345678901234568e18]
~~~

* Fail:
~~~js
[9223372036854775807]
~~~

~~~js
[9.223372036854776e18]
~~~

* Fail:
~~~js
[0.0]
~~~

~~~js
[0]
~~~

* Fail:
~~~js
[-0.0]
~~~

~~~js
[-0]
~~~


Summary: 18 of 27 are correct.

