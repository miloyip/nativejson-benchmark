# Conformance of C++ REST SDK (C++11)

## 1. Parse Validation

* `../data/jsonchecker/fail13.json` is invalid but was mistakenly deemed valid.
~~~js
{"Numbers cannot have leading zeroes": 013}
~~~

* `../data/jsonchecker/fail15.json` is invalid but was mistakenly deemed valid.
~~~js
["Illegal backslash escape: \x15"]
~~~

* `../data/jsonchecker/fail17.json` is invalid but was mistakenly deemed valid.
~~~js
["Illegal backslash escape: \017"]
~~~

* `../data/jsonchecker/fail26.json` is invalid but was mistakenly deemed valid.
~~~js
["tab\   character\   in\  string\  "]
~~~

* `../data/jsonchecker/fail28.json` is invalid but was mistakenly deemed valid.
~~~js
["line\
break"]
~~~


Summary: 29 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String

* `["\uD834\uDD1E"]`
  * expect: `"𝄞"` (length: 4)
  * actual: `""` (length: 0)


Summary: 8 of 9 are correct.

## 4. Roundtrip

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
[1.2345]
~~~

~~~js
[1.2344999999999999]
~~~

* Fail:
~~~js
[-1.2345]
~~~

~~~js
[-1.2344999999999999]
~~~

* Fail:
~~~js
[5e-324]
~~~

~~~js
[4.9406564584124654e-324]
~~~

* Fail:
~~~js
[2.225073858507201e-308]
~~~

~~~js
[2.2250738585072009e-308]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.7976931348623157e+308]
~~~


Summary: 20 of 27 are correct.

