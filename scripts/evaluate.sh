# Usage is : ./evaluate.sh testSetDir modelsDir dictionary.yml 
testDir="$1"
modelsDir="$2"
dictionary="$3"
date="`date +%H-%m-%S`.log"
total=`find $testDir/* -type f | wc -l`
counter=0
forks=0
testOne(){
    category="`echo $1 | rev | cut -d'/' -f2 | rev`"
    catRes=`./scripts/inference.sh "$1" "$modelsDir" "$dictionary" | tail -1 | cut -f1`
    echo -ne "$category:$catRes\t "
    if [ "$category" = "$catRes" ]
    then 
        echo 1 >> ./$date ;
    else
        echo 0 >> ./$date ;
    fi
}
for i in `find $testDir/* -type f`
do 
    category="`echo $i | rev | cut -d'/' -f2 | rev`"
    catRes=`./scripts/inference.sh "$i" "$modelsDir" "$dictionary" | tail -1 | cut -f1`
    echo -ne "$category:$catRes\t "
    if [ "$category" = "$catRes" ]
    then 
        echo 1 >> ./$date ;
    else
        echo 0 >> ./$date ;
    fi
    #testOne $i# &
    #PID=$!
    #((forks++))
    #if [ "$forks" = "8" ]
    #then
    #    wait $PID
    #fi
    echo -n "$counter/$total"
    ((counter++))
    echo -ne "\r"
done 
