#!/usr/bin/env bash

printf '\nINFO: Starting command line argument tests with valgrind.\n'

stderrfile="tests/error.txt"
logfile="output/valgrind_log.txt"
doesnotexist="level/Diese_Datei_sollte_nicht_existieren.txt"
unreadable="level"
empty="level/empty.txt"

rm -f "$doesnotexist" "$stderrfile" "$empty"

touch "$empty"

mkdir -p output level

for level in 1 2 3 4 5
do
    printf '\nINFO: Testing level %s and quit command with valgrind.\n\n' "$level"

    printf 'valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./spaceinvaders_valgrind < input/quit.txt > output/%s.txt\n\n' $level

    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$logfile ./spaceinvaders_valgrind < input/quit.txt > output/$level.txt

    returnvalue=$?

    if ! tests/check_valgrind_log.sh "$logfile"
    then
        printf '\tERROR: valgrind found a problem. See %s for details.\n' $logfile
        exit 1
    fi

    if [ $returnvalue -eq 0 ]
    then
        printf '\tOK: Return value is 0 for input q.\n'
    else
        printf '\tERROR: Return value should be 0 for input q, but return value is %s instead.\n' $returnvalue
        exit 1
    fi
done

function test_valid_args(){
    expected_error_code="0"
    outputname="$1"

    printf 'INFO: Testing command with valid arguments:\n\n\t'
    echo "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$logfile ./spaceinvaders_valgrind" "${@:2}"
    printf '\n'
    
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$logfile ./spaceinvaders_valgrind "${@:2}" < input/quit.txt > output/valgrind_$outputname.txt
    
    return_value=$?

    if ! tests/check_valgrind_log.sh "$logfile"
    then
        printf '\tERROR: valgrind found a problem. See %s for details.\n' $logfile
        exit 1
    fi

    if [ $return_value -eq "$expected_error_code" ]
    then
        printf '\tOK: Return value is %s as expected.\n' "$expected_error_code"
    else
        printf '\tERROR: Return value is not %s, but %s.\n\n' "$expected_error_code" "$return_value"
        exit 1
    fi

    printf '\n'
}

level="level/1.txt"

function test_invalid_args(){
    expected_error_code="$1"

    printf 'INFO: Testing command:\n\n\t'
    echo 'valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./spaceinvaders_valgrind' "${@:2}"
    printf '\n'
    
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=$logfile ./spaceinvaders_valgrind "${@:2}" 2> "$stderrfile"
    
    return_value=$?

    if ! tests/check_valgrind_log.sh "$logfile"
    then
        printf '\tERROR: valgrind found a problem. See %s for details.\n' $logfile
        exit 1
    fi

    if [ $return_value -eq "$expected_error_code" ]
    then
        printf '\tOK: Return value is %s as expected.\n' "$expected_error_code"
    else
        printf '\tERROR: Return value is not %s, but %s.\n\n' "$expected_error_code" "$return_value"
        exit 1
    fi

    if [ -s "$stderrfile" ]
    then
        printf '\tOK: Something has been printed to stderr.\n'
    else
        printf '\tERROR: The correct error code has been returned, but no error message has been printed to stderr.\n'
        exit 1
    fi

    printf '\n'
}

printf 'INFO: Testing valid command line arguments.\n\n'
test_valid_args
test_valid_args 'output_level1' --level level/1.txt
test_valid_args 'output_level1_easy' --level level/1.txt --difficulty easy
test_valid_args 'output_normal_level2' --difficulty normal --level level/2.txt
test_valid_args 'output_hard' --difficulty hard

printf 'INFO: Testing invalid command line arguments.\n\n'
test_invalid_args '1' --level
test_invalid_args '1' --level level/1.txt --difficulty
test_invalid_args '1'  --difficulty easy --level
test_invalid_args '1' --difficulty cheese
test_invalid_args '1' --dooficulty easy
test_invalid_args '1' --level level/1.txt --level level/1.txt
test_invalid_args '1' --difficulty easy --difficulty hard
test_invalid_args '1' --level easy --difficulty level/1.txt

printf 'INFO: Testing non-existing level file.\n\n'
test_invalid_args '2' --level "$doesnotexist"

printf 'INFO: Testing unreadable level file.\n\n'
test_invalid_args '3' --level "$unreadable"

printf 'INFO: Testing empty level file.\n\n'
test_invalid_args '4' --level "$empty"

rm -f "$doesnotexist" "$stderrfile" "$empty" "$logfile"
