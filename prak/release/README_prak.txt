Выполнено:
	Коваленко Диана 321гр
Операционная система: Ubuntu 14.04 LTS
Оборудование: Intel(R) Core(TM) i3-2367M CPU @ 1.40GHz 3,8 GiB

Характеристики видеокарты:
sudo lshw -c video
PCI (sysfs)  

  *-display               
       description: VGA compatible controller
       product: 2nd Generation Core Processor Family Integrated Graphics Controller
       vendor: Intel Corporation
       width: 64 bits
       resources: irq:45 memory:c4000000-c43fffff memory:b0000000-bfffffff ioport:5000(size=64)

Задание выполнено с помощью openCL

Как собирать:
собирать из корня проекта, запускать из папки bin соответственно raytracer 
	и clraytracer
make nocl - для сборки программы неиспользующей параллелизм 
make cl - для сборки с openCL
make - для сборки всего сразу

файлы с настройками лежат в bin
Запускается примерно так:
./raytracer cx.txt

Примечание:
В релизной версии от пользователя требуется ввести номер платформы, используемой для вычислений
Для замеров времени эта проверка была выключена
На моем ноуте нет отдельной видеокарты, зато получилось неплохо распараллелиться на CPU.
(Там, благо, 4 ядра) 

Итак, результаты замеров полного времени работы программы с /usr/bin/time :
Настройка: c1.txt
0:27.97 - без openCL
0:11.25 - с openCL

Настройка: c2.txt
1:35:23 - без openCL
29:15.75 - с openCL

Настройка: c3.txt
9:09.29 - без openCL
2:47.79 - с openCL

Настройка: c4.txt
13:37.93 - без openCL
4:09.07 - с openCL