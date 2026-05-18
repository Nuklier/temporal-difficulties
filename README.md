# temporal-difficulties
A master of time, so they say...

Репозиторий для небольшого проекта по C++ 2 курса МФТИ, как вы сюда попали вообще?

КАК КОМПИЛИРОВАТЬ:
1. У вас должна стоять SFML 3.1.0
2. это мои настройки компиляции, они для g++, если у вас компилятор от ms visual studio - придется поисать другие команды компилятору



g++ -c main.cpp -I"C:\...\libraries\SFML-3.1.0\include"/

g++ main.o -o sfml-app -L"C:\...\libraries\SFML-3.1.0\lib" -lsfml-graphics -lsfml-window -lsfml-system

./sfml-app



на месте ... полный путь до места, где живет SFML

КАК ИСПОЛЬЗОВАТЬ .exe:
в папке с экзешником должны лежать dll используемых библиотек SFML и шрифт font.ttf
все, с таким набором файлов оно должно запустить
