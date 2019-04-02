admin: bookkeeper admin.c common.h
	gcc -o admin admin.c -I.

bookkeeper: bookkeeper.c common.h
	gcc -o bookkeeper bookkeeper.c -I.