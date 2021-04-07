rm R.bsig R.data R.in R.info R.psig R.tsig
time ./create R simc 100000 5 100
time ./gendata 90000 5 123456 13 > R.in
time ./insert R
