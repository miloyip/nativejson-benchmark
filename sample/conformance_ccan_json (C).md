# Conformance of ccan/json (C)

## 1. Parse Validation


Summary: 34 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `""` (length: 0)


Summary: 8 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[-1.234567890123457e+18]
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9.223372036854776e+18]
~~~

* Fail:
~~~js
[1234567890123456789]
~~~

~~~js
[1.234567890123457e+18]
~~~

* Fail:
~~~js
[9223372036854775807]
~~~

~~~js
[9.223372036854776e+18]
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
[4.940656458412465e-324]
~~~

* Fail:
~~~js
[2.2250738585072014e-308]
~~~

~~~js
[2.225073858507201e-308]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.797693134862316e+308]
~~~


Summary: 18 of 27 are correct.

