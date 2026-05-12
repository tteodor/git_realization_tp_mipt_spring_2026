#!/usr/bin/env bash

gitpta_completions() {
    local curr="${COMP_WORDS[COMP_CWORD]}"
    local prev="${COMP_WORDS[COMP_CWORD-1]}"
    local opts="init add commit status show switch log pure_merge"

    case "${prev}" in
        switch)
            COMPREPLY=( $(compgen -W "-c" -- ${curr}) )
            return 0
            ;;
        commit)
            COMPREPLY=( $(compgen -W "-m" -- ${curr}) )
            return 0
            ;;
        show)
            COMPREPLY=( $(compgen -W "--format" -- ${curr}) )
            return 0
            ;;
        *)
            ;;
    esac

    COMPREPLY=( $(compgen -W "${opts}" -- ${curr}) )
    return 0
}

complete -F gitpta_completions gitpta

