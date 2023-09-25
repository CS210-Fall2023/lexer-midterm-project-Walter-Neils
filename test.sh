#!/bin/bash
for file in files/*.cci; do
    ./lexer/lexer $file >${file}.lexer
    echo "${file}.ANS vs. ${file}.lexer"
    #diff ${file}.ANS  ${file}.lexer
    if [[ $(diff ${file}.ANS ${file}.lexer) ]]; then #if output of diff command is not zero
        echo "Somthing went wrong"
        # diff ${file}.ANS ${file}.lexer
        cat ${file}.ANS | xclip -selection clipboard
        echo "Copied answer to clipboard"
        read -p "Press enter to continue"
        cat ${file}.lexer | xclip -selection clipboard
        echo "Copied your answer to clipboard"
        echo
        read -p "Press enter to continue"
    else
        echo "You passed the test"
        echo
    fi
    # Wait for enter key

done

for file in files/*.ccx; do
    ./lexer/lexer $file >${file}.lexer
    echo "${file}.ANS vs. ${file}.lexer"
    #diff ${file}.ANS  ${file}.lexer
    if [[ $(diff ${file}.ANS ${file}.lexer) ]]; then #if output of diff command is not zero
        echo "Somthing went wrong"
        # diff ${file}.ANS ${file}.lexer
        cat ${file}.ANS | xclip -selection clipboard
        echo "Copied answer to clipboard"
        read -p "Press enter to continue"
        cat ${file}.lexer | xclip -selection clipboard
        echo "Copied your answer to clipboard"
        read -p "Press enter to continue"
        echo
    else
        echo "You passed the test"
        echo
    fi
done
