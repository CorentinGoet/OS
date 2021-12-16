default: partage nbOctets link

partage: partage.c
	gcc -c partage.c -o partage.o

nbOctets: nbOctets.c
	gcc -c nbOctets.c -o nbOctets.o

link: nbOctets.o partage.o 
	gcc -o main nbOctets.o partage.o
