# Conformance of Configuru (C++11)

## 1. Parse Validation


Summary: 34 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String


Summary: 9 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.7976931348623157e+308]
~~~


Summary: 26 of 27 are correct.

