rm R.bsig R.data R.in R.info R.psig R.tsig
time ./create R catc 10100 4 1000
time ./gendata 10000 4 7654321 23 > R.in
time ./insert R < R.in
./stats R
./select R '7663852,?,?,?'
./select R '7663852,?,?,?' t
./select R '7663852,?,?,?' p
./select R '7663852,?,?,?' b
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?'
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' t
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' p 
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' b
./select R '?,?,a3-242,a4-242'
./select R '?,?,a3-242,a4-242' t
./select R '?,?,a3-242,a4-242' p
./select R '?,?,a3-242,a4-242' b
./select R '8765432,?,?,?'
./select R '8765432,?,?,?' t
./select R '8765432,?,?,?' p
./select R '8765432,?,?,?' b
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?'
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' t
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' p
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' b
