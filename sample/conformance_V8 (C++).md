# Conformance of V8 (C++)

## 1. Parse Validation


Summary: 34 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String


Summary: 9 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-1234567890123456789]
~~~

~~~js
[-1234567890123456800]
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9223372036854776000]
~~~

* Fail:
~~~js
[1234567890123456789]
~~~

~~~js
[1234567890123456800]
~~~

* Fail:
~~~js
[9223372036854775807]
~~~

~~~js
[9223372036854776000]
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
[1.7976931348623157e308]
~~~

~~~js
[1.7976931348623157e+308]
~~~


Summary: 20 of 27 are correct.

