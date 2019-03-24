#/bin/bash

./make.sh 

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

# Run on vaio
ssh ash@vaio "./get_taskie.sh" 
