#!/bin/sh
./scanner tests/t0 > tests/t0.out
./scanner tests/t1 > tests/t1.out
./scanner tests/t2 > tests/t2.out
./scanner tests/t3 > tests/t3.out
./scanner tests/t4 > tests/t4.out
./scanner tests/t5 > tests/t5.out
diff tests/t0.out tests/t0.expected
diff tests/t1.out tests/t1.expected
diff tests/t2.out tests/t2.expected
diff tests/t3.out tests/t3.expected
diff tests/t4.out tests/t4.expected
diff tests/t5.out tests/t5.expected
