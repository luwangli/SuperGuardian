# SuperGuardian
## Intro
SuperGuardian enhance existing sketches by indentify and guardian superspreaders efficiently in cardinality estimation.
We apply SuperGuardian to three typical sketches (cSkt, bSkt, rSkt) under real world datasets.
because the cardinality estimator has much impact on sketches, we have achieved sketches with carious estiamtor, including Linear Counter, LogLog Counter and HyperLogLog counter. 
## File
- cSkt/. the implementation of cSkt with three estimator.
- bSkt/. the implementation of bSkt with three estimator.
- rSkt/. the implementation of rSkt with three estimator.
- SF/. the implementation of SuperGuardian.
- LC/. the main function to comparing these algorithms with LC estimator
- LL/. the main function to comparing these algorithms with LL estimator
- HLL/. the main function to comparing these algorithms with HLL estimator
- data/. a test case
- Estimator/. the implementation of three estimators.

## Requirement
Ensure g++ and make are installed. Our experimental platform is equipped with Ubuntu 16, g++ 5.4 and make 4.1

## Compile and Run

### impact of SuperGuardian on LC estimator
```
$cd LC
$make clean
$make
$./main。out
```
Then input parameter according to the guideline. You will get the result of cSkt(LC), SG-cSkt(LC), bSkt(LL), SG-bSkt(LL), rSkt(HLL), SG-rSkt(HLL). 

The operation is similar to LL and HLL.



