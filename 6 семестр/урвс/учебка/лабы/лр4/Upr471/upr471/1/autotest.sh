rm -f temp
./upr470 1 1 > cmps/res1
echo "1"
rm -f temp
./upr470 12 12 > cmps/res2
echo "2"
rm -f temp
./upr470 6 2 > cmps/res3
echo "3"
rm -f temp
./upr470 8 3 > cmps/res4
echo "4"
rm -f temp
./upr470 > cmps/res5
echo "5"
rm -f temp
./upr470 3 > cmps/res6
echo "6"
rm -f temp
./upr470 8 5 2 > cmps/res7
echo "7"
rm -f temp
./upr470 uf pf > cmps/res8
echo "8"
rm -f temp
./upr470 14 8 > cmps/res9
echo "9"
rm -f temp
./upr470 9 10 > cmps/res10
echo "10"
rm -f temp
{
cmp cmps/cpm1 cmps/res1 && echo "Test 1 successfully passed"
} || echo "Test 1 failed!"
{
cmp cmps/cpm2 cmps/res2 && echo "Test 2 successfully passed"
} || echo "Test 2 failed!"
{
cmp cmps/cpm3 cmps/res3 && echo "Test 3 successfully passed"
} || echo "Test 3 failed!"
{
cmp cmps/cpm4 cmps/res4 && echo "Test 4 successfully passed"
} || echo "Test 4 failed!"
{
cmp cmps/cpm5 cmps/res5 && echo "Test 5 successfully passed"
} || echo "Test 5 failed!"
{
cmp cmps/cpm6 cmps/res6 && echo "Test 6 successfully passed"
} || echo "Test 6 failed!"
{
cmp cmps/cpm7 cmps/res7 && echo "Test 7 successfully passed"
} || echo "Test 7 failed!"
{
cmp cmps/cpm8 cmps/res8 && echo "Test 8 successfully passed"
} || echo "Test 8 failed!"
{
cmp cmps/cpm9 cmps/res9 && echo "Test 9 successfully passed"
} || echo "Test 9 failed!"
{
cmp cmps/cpm10 cmps/res10 && echo "Test 10 successfully passed"
} || echo "Test 10 failed!"

