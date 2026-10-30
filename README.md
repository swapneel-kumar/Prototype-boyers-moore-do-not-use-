# Prototype-boyers-moore-do-not-use-
It is a algorithm that finds a word in a text file
It is not thoroughly tested 
It uses the boyers moore horsepool algorithm to search patterns
It can only run only on posix compliant system(macos(not tested),linux) since it uses sys calls like write(),read()
currently only searches for lowercase ascii words or patterns

Todo 
1. Planning on replacing read() with a even more fundamental function to avoid context switching between kernel and user mode
2. Making it support utf-8

