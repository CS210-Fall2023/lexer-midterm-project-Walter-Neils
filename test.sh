#!/bin/bash

# Usage: print_success <message>
print_success() {
    printf "\e[0;32m  [✔] %s\e[0m\n" "$1"
}

# Usage: print_error <message>
print_error() {
    printf "\e[0;31m  [✖] %s %s\e[0m\n" "$1" "$2"
}

# Usage: print_info <message>
print_info() {
    printf "\e[0;34m  [i] %s\e[0m\n" "$1"
}

for file in files/*.cci; do
    cat ${file} | ./lexer/lexer $file >${file}.lexer
    print_info "${file}.ANS vs. ${file}.lexer"
    #diff ${file}.ANS  ${file}.lexer
    if [[ $(diff ${file}.ANS ${file}.lexer) ]]; then #if output of diff command is not zero
        print_error "Test failed"
        vimdiff ${file}.ANS ${file}.lexer
    else
        print_success "You passed the test"
    fi
done

for file in files/*.ccx; do
    cat ${file} | ./lexer/lexer $file >${file}.lexer
    print_info "${file}.ANS vs. ${file}.lexer"
    #diff ${file}.ANS  ${file}.lexer
    if [[ $(diff ${file}.ANS ${file}.lexer) ]]; then #if output of diff command is not zero
        print_error "Test failed"
        vimdiff ${file}.ANS ${file}.lexer
    else
        print_success "You passed the test"
    fi
done
