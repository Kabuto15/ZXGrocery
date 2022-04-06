zcc +zx -vn -O3 -m -startup=31 -clib=new src/*.c src/resources/*.asm -o zxgrocv1 -create-app -Cz--screen=tools/zxgrocery_loading.scr -Cz--sZxGrocery
#use tapir to convert from tape to wav

if [[ $1 == "run" ]]
    then
    cd speccy
    ./speccy -48 ../zxgrocv1.tap
fi

# BACKUP COMMANDS
#zcc +zx -vn -O3 -m -startup=31 -clib=new src/*.c src/resources/*.asm -o cklspeccy40 -create-app -zorg=32767
#./tools/bas2tap -sName -a10 tools/loader.bas tools/loader.tap
#z88dk-appmake +zx -b tools/zxgrocery_loading.scr -o screen.tap --org 16384 --noloader --blockname art
#z88dk-appmake +zx -b cklspeccy40_CODE.bin -o cklspeccy40.tap --org 32767 --noloader --blockname game
#cat tools/loader.tap screen.tap cklspeccy40.tap > zxgrocery.tap

