# find_substr_in_big_file
Практическая часть экзамена 1С

Задача 1:

Необходимо реализовать эффективный алгоритм поиска по строке в очень большом текстовом файле. Файл загружается один раз, потом происходит большое количество запросов на поиск.

Идея:

Отсортировываем суффиксы в порядке возрастания. Для этого сортируем по степеням двойки, чтобы на i-ой итерации делать сравнение за О(1).

Таким образом предпроцессинг занимает: $N*log^2(N)$, где N - это размер исходного текста.

Поиск подстроки делаем обычным бинарным поиском:

Итого поиск подстроки занимает: $T*log(N)$, где T - размер подстроки

