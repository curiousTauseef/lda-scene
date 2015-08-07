# Usage: ./chooseTrainSet CategoriesDIR numPerCategory outDir
catDir="$1" #consists one folder per category, each consisting images.
perCat="$2" #number of images per category.
outDir="$3" #output directory in which training sets will be copied each in the same name directory as input 
#TODO make soft links insted 

for i in `find $catDir/* -maxdepth 1 -type d`
do
    category="${i##*/}"
    mkdir "$outDir/$category" 2>/dev/null
done

for i in `find $catDir/* -maxdepth 1 -type d`
do 
    category="${i##*/}"
    echo choosing from category $category ...
    find "$i" -iname "*.jpg"  | shuf | tail -$perCat | xargs -I{} cp -v {} "$outDir/$category/"
done

