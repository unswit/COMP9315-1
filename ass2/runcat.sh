rm R.bsig R.data R.in R.info R.psig R.tsig
time ./create R catc 10100 4 1000
time ./gendata 10000 4 7654321 23 > R.in
time ./insert R < R.in

echo "./stats R"
./stats R
echo "./select R '7663852,?,?,?'"
./select R '7663852,?,?,?'

echo "./select R '7663852,?,?,?' t"
./select R '7663852,?,?,?' t

echo "./select R '7663852,?,?,?' p"
./select R '7663852,?,?,?' p

echo "./select R '7663852,?,?,?' b"
./select R '7663852,?,?,?' b

echo "./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?'"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?'

echo "./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' t"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' t

echo "./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' p "
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' p 

echo "./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' b"
./select R '7664096,PjZZsBYoEYAMzgpCgRKg,?,?' b

echo "./select R '?,?,a3-242,a4-242'"
./select R '?,?,a3-242,a4-242'

echo "./select R '?,?,a3-242,a4-242' t"
./select R '?,?,a3-242,a4-242' t

echo "./select R '?,?,a3-242,a4-242' p"
./select R '?,?,a3-242,a4-242' p

echo "./select R '?,?,a3-242,a4-242' b"
./select R '?,?,a3-242,a4-242' b

echo "./select R '8765432,?,?,?'"
./select R '8765432,?,?,?'

echo "./select R '8765432,?,?,?' t"
./select R '8765432,?,?,?' t

echo "./select R '8765432,?,?,?' p"
./select R '8765432,?,?,?' p

echo "./select R '8765432,?,?,?' b"
./select R '8765432,?,?,?' b

echo "./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?'"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?'

echo "./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' t"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' t

echo "./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' p"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' p

echo "./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' b"
./select R '7664096,tRzgWRUlUEdoYPZjofYr,?,?' b
