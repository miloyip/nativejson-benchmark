# Conformance of jsoncons (C++)

## 1. Parse Validation


Summary: 34 of 34 are correct.

## 2. Parse Double


Summary: 66 of 66 are correct.

## 3. Parse String


Summary: 9 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
{"foo":"bar"}
~~~

~~~js
{"foo": "bar"}
~~~

* Fail:
~~~js
{"a":null,"foo":"bar"}
~~~

~~~js
{"a": null,"foo": "bar"}
~~~

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9223372036854775808.0]
~~~

* Fail:
~~~js
[5e-324]
~~~

~~~js
[4.9406564584124654417656879286822137236505980261432476442558568250067550727020875186529983636163599237979656469544571773092665671035593979639877479601078187812630071319031140452784581716784898210368871863605699873072305000638740915356498438731247339727317e-324]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.7976931348623157e+308]
~~~


Summary: 22 of 27 are correct.

