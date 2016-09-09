# Conformance of Parson (C)

## 1. Parse Validation

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


Summary: 31 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `"Hello"` (length: 5)


Summary: 8 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[-1234567890123456768.000000]
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9223372036854775808.000000]
~~~

* Fail:
~~~js
[4294967295]
~~~

~~~js
[4294967295.000000]
~~~

* Fail:
~~~js
[1234567890123456789]
~~~

~~~js
[1234567890123456768.000000]
~~~

* Fail:
~~~js
[9223372036854775807]
~~~

~~~js
[9223372036854775808.000000]
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
[0]
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
[179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000]
~~~


Summary: 14 of 27 are correct.

