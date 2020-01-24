build :
        scl enable devtoolset-7 'g++ -c *.cpp -std=c++17'
        scl enable devtoolset-7 'g++ *.o -o simulator'
        echo "Available	to be run at ./simulator"
clean :
       	rm *.o

run :
     	./simulator

