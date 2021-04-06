rm R.bsig R.data R.in R.info R.psig R.tsig
time ./create R simc 90000 6 1000
time ./gendata 90000 6 1234567 13 > R.in
time ./insert R < R.in
