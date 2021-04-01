rm R.bsig R.data R.in R.info R.psig R.tsig
./create R catc 10000 6 1000
./gendata 10000 6 1234567 13 > R.in
./insert R < R.in

