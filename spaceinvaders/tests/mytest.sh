CFLAGS="-g -fsanitize=address -fsanitize=undefined -std=c99 -Wall -Wextra -Werror"
name="spaceinvaders"

#Test1 hatte versucht tests die deutlich tiefer testen, rückgabe vom mehoden bzw was sie ändern
# allerdings hatten die nie wirklich geklappt :(      

# Compile program
if ! gcc $CFLAGS spaceinvaders.c -o spaceinvaders
then
    printf 'ERROR: Program could not be compiled.\n'
    exit 1
fi    

# Run 1.txt easy
if ! ./spaceinvaders --level level/test1.txt --difficulty easy 
then
    printf 'ERROR: irgendwas ist falsch.\n'
    exit 1
fi 

# Run 1.txt easy with q
if ! ./spaceinvaders < tests/q.txt --level level/test1.txt --difficulty easy 
then
    printf 'ERROR: irgendwas ist falsch.\n'
    exit 1
fi 


# Run 1.txt easy with ddddaaaaq
if ! ./spaceinvaders < tests/ddddaaaaq.txt --level level/test1.txt --difficulty easy 
then
    printf 'ERROR: irgendwas ist falsch.\n'
    exit 1
fi 