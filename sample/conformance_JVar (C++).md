# Conformance of JVar (C++)

## 1. Parse Validation

* `../data/jsonchecker/fail25.json` is invalid but was mistakenly deemed valid.
~~~js
["	tab	character	in	string	"]
~~~

* `../data/jsonchecker/fail27.json` is invalid but was mistakenly deemed valid.
~~~js
["line
break"]
~~~


Summary: 32 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String

* `["\uD834\uDD1E"]`
  * expect: `"ùÑû"` (length: 4)
  * actual: `"Ì†¥Ì¥û"` (length: 6)


Summary: 8 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[-1.23457e+18]
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9.22337e+18]
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

* Fail:
~~~js
[5e-324]
~~~

~~~js
[4.94066e-324]
~~~

* Fail:
~~~js
[2.225073858507201e-308]
~~~

~~~js
[2.22507e-308]
~~~

* Fail:
~~~js
[2.2250738585072014e-308]
~~~

~~~js
[2.22507e-308]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.79769e+308]
~~~


Summary: 19 of 27 are correct.

