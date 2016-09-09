# Conformance of YAJL (C)

## 1. Parse Validation


Summary: 34 of 34 are correct.

## 2. Parse Double

* `[2.22507e-308]`
  * expect: `2.2250699999999998e-308 (0x016FFFFE2E8159D0)`
  * actual: `0 (0x0160)`

* `[-2.22507e-308]`
  * expect: `-2.2250699999999998e-308 (0x016800FFFFE2E8159D0)`
  * actual: `0 (0x0160)`

* `[4.9406564584124654e-324]`
  * expect: `4.9406564584124654e-324 (0x0161)`
  * actual: `0 (0x0160)`

* `[2.2250738585072009e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[1e-10000]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[2.2250738585072011e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`

* `[1e-214748363]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[1e-214748364]`
  * expect: `0 (0x0160)`
  * actual: `0 (0x0160)`

* `[2.22507385850720113605740979670913197593481954635164564e-308]`
  * expect: `2.2250738585072009e-308 (0x016FFFFFFFFFFFFF)`
  * actual: `0 (0x0160)`


Summary: 57 of 66 are correct.

## 3. Parse String

* `["Hello\u0000World"]`
  * expect: `"Hello\0World"` (length: 11)
  * actual: `"Hello"` (length: 5)


Summary: 8 of 9 are correct.

## 4. Roundtrip

* Fail:
~~~js
[-9223372036854775808]
~~~

~~~js
[-9223372036854775808.0]
~~~

* Fail:
~~~js
[1.2345]
~~~

~~~js
[1.2344999999999999307]
~~~

* Fail:
~~~js
[-1.2345]
~~~

~~~js
[-1.2344999999999999307]
~~~

* Fail:
~~~js
[2.225073858507201e-308]
~~~

~~~js
[2.2250]
~~~

* Fail:
~~~js
[2.2250738585072014e-308]
~~~

~~~js
[2.2250738585072013831e-308]
~~~

* Fail:
~~~js
[1.7976931348623157e308]
~~~

~~~js
[1.7976931348623157081e+308]
~~~


Summary: 21 of 27 are correct.

