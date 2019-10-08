# Практическая работа за 3-й курс на факультете ПМИ, НГТУ

### Постановка задачи
Решить задачу **кросс-докинга** посредством полного перебора и реализовать приложение для работы, при:  
* разных наборах входящих/выходящих транспортных средств;  
*	заданной таблице планов разгрузок;  
*	заданных типах товаров;  
*	заданных нормах погрузки/выгрузки для каждого типа товара;  
*	минимальном времени обработки товаров.  

### Основной алгоритм полного перебора
Основной алгоритм решения задачи заключается в последовательном переборе всех возможных перестановок транспортных средства и товаров, расположенных в них.  

Представим начальную последовательность транспортных средств и товаров как последовательность чисел от 0 до n, где n – количество товаров. Разобьём последовательность на m равных отрезков, где m – количество автомобилей.  

Пусть имеется 3 типа товаров, каждый из которых загружен в отдельный автомобиль в числе 5 единиц (количество товаров в каждом транспортном средстве может быть произвольным). Начальная последовательность транспортных средств и товаров будет выглядеть следующим образом:

| Машина 1  | Машина 2   | Машина 3   |  
|:---------:|:----------:|:----------:|  
| 0 0 0 0 0 | 1 1 1 1 1  | 2 2 2 2 2  |  
 
где 0 – первый товар, 1 – второй товар, 2 – третий товар; в случае если в машине есть свободное под товар место, оно будет помечено как “-”.  
Указанная последовательность (000001111122222) будет начальной последовательностью (последовательность автомобилей для приёмки), на основе этой последовательности будет произведён полный перебор всех её возможных перестановок и поиск оптимальной по времени конечной последовательности (последовательности отправки). 

### Алгоритм сопоставления одинаковых перестановок  
Для поиска минимальной по времени перестановки (последовательности отправки) будет использован метод **“ядра”** – из транспортного средства будет выгружена та часть груза, что адресована в иное направление, а на освободившееся место загружается другой груз, идущий в тот же пункт назначения.  

Каждая новая перестановка проверяется на совпадение с конечной перестановкой (последовательностью отправки) по содержимому каждого транспортного средства, вне зависимости от их расположения, но с учётом положения товаров внутри транспортных средств.  

В случае совпадения перестановок проводится сравнение позиций товаров. Eсли единица товара машины разгрузки на проверяемом месте не равна единице товара на том же самом месте в машине погрузки (исключаем разгрузку и погрузку одного и того же товара или пустые места в транспортных средствах): проверяем, что место в машине разгрузки не пустое и прибавляем к времени обслуживания время разгрузки товара; проверяем, что место в машине погрузки не пустое и прибавляем к времени обслуживания время погрузки товара; По окончанию просмотра перестановок сравниваем  минимальное время с найденным и переходим к следующей новой перестановке, пока не будут перебраны все.
