ge=/home/jplank/cs360/labs/Lab-4-Fakemake/Gradescript-Examples
rm -f testmain.c tf1.c tf2.c tf3.c tf4.c tf0.c s1_header.h s2_header.h
rm -f testmain.o tf1.o tf2.o tf3.o tf4.o tf0.o testexec testlib.a
cp $ge/allfile.c testmain.c
sed 's/first/first/' $ge/oneline-1.c > tf0.c
sed 's/first/second/' $ge/using_s1.c > tf1.c
cp $ge/s1_header-0.h s1_header.h
sed 's/first/third/' $ge/oneline-0.c > tf2.c
sed 's/first/fourth/' $ge/oneline-5.c > tf3.c
sed 's/first/fifth/' $ge/oneline-4.c > tf4.c
gcc -c testmain.c
touch -t 01011100.30 testmain.c
touch -t 01011100.35 testmain.o
gcc -c tf0.c
touch -t 01011101.30 tf0.c
touch -t 01011101.35 tf0.o
gcc -c tf1.c
touch -t 01011102.30 tf1.c
touch -t 01011102.35 tf1.o
gcc -c tf2.c
touch -t 01011103.30 tf2.c
touch -t 01011103.35 tf2.o
gcc -c tf3.c
touch -t 01011104.30 tf3.c
touch -t 01011104.35 tf3.o
gcc -c tf4.c
touch -t 01011105.30 tf4.c
touch -t 01011105.35 tf4.o
touch -t 01011000.30 s1_header.h
gcc -o testexec testmain.c tf1.c tf2.c tf3.c tf4.c tf0.c -lm
touch -t 01011106.35 testexec
touch -t 01011100.25 testmain.o
rm -f tf0.o
rm -f tf1.o
touch -t 01011103.25 tf2.o
rm -f tf3.o
touch -t 01011105.25 tf4.o
if ./fakemake $ge/033.fm; then
  ./testexec
fi
