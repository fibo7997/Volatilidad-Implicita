# Volatilidad-Implicita

Este programa busca calcular la volatilidad implicita de una serie de tiempo de calls de Galicia. 

La volatilidad implicita es aquella que esta representada por el precio del call. Es decir, lo que el mercado esta sobre especulando sobre el activo. Estos swings de volatilidad se dan en un contexto electoral, en donde el riesgo de Galicia es representado por el riesgo del pais (aparte de que el beta de galicia suele rondar 1). 

Este trabajo demuestra empiricamente lo que es estudiado en la teoria, la volatilidad implicita siempre revierte a la media (o a su volatilidad realizada)

Estrategias de opciones: Como se pueden ver en los graficos (Imagen 1/2/3), esta brecha entre la volatilidad realizada y la implicita representa una oportunidad de trade.

Se podria shortear calls y puts y delta hedgear la posicion. Los puntos de entrada (imagen 3), muestran los puntos en donde se puede inicar la venta de estas opciones para aporvechar la compresion de esta brecha. En caso de que la volatilidad implicita siga subiendo se puede aumentar la posicion (por ejemplo empezar cuando la volatilidad implicita este en 70% y aumentar la exposicion cuando este en 120%)

El orden en el que se desarrollo este programa es el siguiente:

1. Importar el archivo csv. y parsearlo. En el proceso, se convirtio la columna created_at en formato "%m/%d/%Y %H:%M"
2. Se definieron los componentes del modelo de BS (distribucion normal para insertar en el calculo de BS)
3. Se calculo la diferencia entre el precio de BS y el precio de mercado para poder encontrar la volatilidad implicita.
4. Se setearon parametros adaptados al mercado argentino. Un minimo de 10% de volatilidad implicita, y un maximo de 500%.
5. Se calculo el time to maturity como la diferencia entre el vencimiento del call de Octubre 2023 de Galicia, y la fecha/hora/minuto de cada registro.
6. SE corrio el programa con el dataframe actualizado (con las nuevas columnas precioCall y precioSubyacente)
7. Se exporto el archivo a un csv.

La volatilidad realizada fue calculada en el archivo de python, en donde uni el csv. de c++ y grafique las imagenes correspondientes.
