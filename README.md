# MapReduce

Лабораторная работа в рамках курса учебной практики. ФПМИ БГУ, 2020.

На данный момент репозиторий содержит простейшую реализацию модели MapReduce,
пока без распределения вычислений или каких-либо сложных операций.

Работоспособность решения под Windows не гарантируется (оно создавалось для
Linux-систем).

# Текущая реализация Map и Reduce

Предоставленные реализации Map и Reduce подсчитывают количество слов в
переданных строках.

## Формат входного файла Map

Каждая строка входного файла должна иметь вид:

`key \t <string1> <string2> ... <stringN>`

Первым словом должно быть слово `key`, далее после отступа (`\t`) можно
перечислить любое количество произвольных строк, разделенных пробелами. Строки
не должны содержать пробелы либо отступы (`\t`).

Строки файла (в том числе его последняя строка) должны оканчиваться стандартным
переводом строки (`\n`).

## Формат выходного файла Map

Каждая строка будет иметь вид:

`<string> \t <value>`

Первым словом идет произвольный ключ (строка), далее после отступа (`\t`) 
находится некоторое числовое значение.

Строки файла (в том числе его последняя строка) будут оканчиваться стандартным
переводом строки (`\n`).

## Формат входного файла Reduce

Каждая строка должна иметь вид:

`<string> \t <value>`

Первым словом идет произвольный ключ (строка), далее после отступа (`\t`) 
находится некоторое числовое значение.

Строки файла (в том числе его последняя строка) должны оканчиваться стандартным
переводом строки (`\n`).

**Важно**: все строки в файле должны соответствовать одному ключу!

Предыдущее замечание влечет за собой следующее: хотя форматы выходного файла
Map и входного файла Reduce, как вы можете заметить, совпадают, сами эти файлы
будут различаться (перед запуском Reduce данные будут отсортированы и 
разделены).

## Формат выходного файла Reduce

Совпадает с форматом выходного файла Map, но на этот раз все ключи будут
уникальны (любая строка-ключ будет встречаться в файле не более одного раза).

# Инструкции по сборке

При написании программы использовались некоторые механизмы библиотеки Boost,
а также библиотека `std::filesystem`.

Для сборки с использованием предоставленного CMake-скрипта рекомендуется 
использовать компилятор `gcc` версии 9.2 или выше, компилятор `clang` версии
9 или выше.

Убедитесь также, что у вас установлены компоненты библиотеки Boost. Установить
их можно, например, с использованием вашего пакетного менеджера:

`sudo apt install libboost-dev libboost-filesystem-dev`

# Инструкции по запуску

Скомпилируйте файлы `map.cpp` (далее будем называть полученный бинарный файл
скриптом Map), `reduce.cpp` (аналогично, полученный бинарный файл будем
называть скриптом Reduce) и `main.cpp` (сама программа MapReduce, будем
предполагать, что название бинарного файла - `mapreduce`).

Для запуска операции Map используйте команду:

`./mapreduce map <map_script> <src_file> <dst_file>`

Для запуска операции Reduce используйте команду:

`./mapreduce reduce <reduce_script> <src_file> <dst_file>`

Возможна работа с произвольным скриптом Map или Reduce (он даже необязательно
должен быть бинарным файлом), если:
* Формат выходного файла Map и входного файла Reduce будет соответствовать 
описанному ранее.
* Вызвать скрипт можно будет командой: `./script`
* Скрипт будет считывать данные из *stdin* и записывать данные в *stdout*.
